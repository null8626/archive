import discord
from discord.ext import commands
import imdb
import time
from decorators import *
from random import randint, choice
import wikipediaapi
from googletrans import Translator, LANGUAGES

class apps(commands.Cog):
    def __init__(self):
        self.translator = Translator(service_urls=['translate.googleapis.com'])
        self.Wikipedia = wikipediaapi.Wikipedia('en')
        self.ia = imdb.IMDb()

    def _resolve_album(self, data, ctx):
        return {
            "title": data['name'],
            "fields": {
                "Total Tracks": f"{data['total_tracks']} track{'' if (data['total_tracks'] == 1) else 's'}",
                "Release Date": ctx.bot.util.timestamp(data['release_date'], '%Y-%m-%d') if (data['release_date'].count('-') == 2) else data['release_date'],
                "Artists": ', '.join([f"[{artist['name']}]({artist['external_urls'].get('spotify', 'https://youtube.com/watch?v=dQw4w9WgXcQ')})" for artist in data['artists']])
            },
            "thumbnail": data['images'][0]['url'] if data.get('images') else None,
            "url": data['external_urls']['spotify']
        }

    def _resolve_artist(self, data):
        return {
            "title": data['name'],
            "url": data['external_urls']['spotify'],
            "thumbnail": data['images'][0]['url'] if data.get('images') else None,
            "fields": {
                "Genres": ', '.join(data['genres']) if data['genres'] else 'No genres.',
                "Popularity": f"{data['popularity']}%",
                "Followers": f"{data['followers']['total']:,} follower{'' if (data['followers']['total'] == 1) else 's'}"
            }
        }

    def _resolve_playlist(self, data):
        return {
            "title": data['name'],
            "url": data['external_urls']['spotify'],
            "fields": {
                "Owner": f"[{data['owner']['display_name']}]({data['owner']['external_urls']['spotify']})",
                "Description": data["description"] or "This playlist is without description.",
                "Total Tracks": f'{data["tracks"]["total"]} track{"" if (data["tracks"]["total"] == 1) else "s"}'
            },
            "thumbnail": data["images"][0]["url"] if data["images"] else None
        }

    def _resolve_track(self, data, ctx):
        res = {
            "title": f"{(str(data['track_number']) + '. ') if data.get('track_number') else ''}{data['name'][:95]}{' [Explicit]' if data['explicit'] else ''}",
            "url": None if (data['explicit'] and (not ctx.channel.nsfw)) else data['external_urls']['spotify'],
            "fields": {
                "Popularity": f"{data['popularity']}%",
                "Duration": time.strftime("%M:%S", time.gmtime(data['duration_ms'] // 1000))
            },
            "thumbnail": data['album']['images'][0]['url'] if (data.get('album') and data['album'].get('images')) else None
        }
        
        if data.get('album'):
            res['fields']['Album'] = f"**Album Name:** {data['album']['name']}\n**Album URL: **[Click Here]({data['album']['external_urls'].get('spotify', 'https://youtube.com/watch?v=dQw4w9WgXcQ')})\n**Total Tracks: **{data['album']['total_tracks']}\n**Release Date: **{ctx.bot.util.timestamp(data['album']['release_date'], '%Y-%m-%d') if (data['album']['release_date'].count('-') == 2) else data['album']['release_date']}"

        if data.get('artists'):
            res['fields']['Artists'] = ', '.join([f"[{artist['name']}]({artist['external_urls'].get('spotify', 'https://youtube.com/watch?v=dQw4w9WgXcQ')})" for artist in data['artists']])

        if data.get('preview_url'):
            res['fields']['Preview URL'] = f"[Click Here]({data['preview_url']})"

        return res

    @command(["song"])
    @cooldown(10)
    @require_args()
    async def deezer(self, ctx, *args):
        parser = ctx.bot.Parser(args)
        parser.parse(('search',))
        
        try:
            result = await ctx.bot.util.request(f'https://api.deezer.com/search/track/autocomplete', limit=20, q=' '.join(args), json=True)
            
            if not ctx.channel.nsfw:
                result['data'] = list(filter(lambda x: not x['explicit_lyrics'], result['data']))
            assert result['data']
            
            if parser.has('search'):
                choose_embed = ctx.bot.ChooseEmbed(ctx, result['data'], key=lambda x: f"*{x['title']}* by {x['artist'].get('name', 'Anonymous')}")
                res = await choose_embed.run()
                del choose_embed
                
                if not res:
                    return
                data = res
            else:
                data = result['data'][0]
            
            await ctx.embed(
                title=data['title'],
                url=data['link'],
                author_name=f"{data['artist'].get('name', 'Anonymous')}",
                author_icon=data['artist']['picture_medium'],
                author_url=data['artist']['link'],
                fields={
                    "Song ID": f"`{data['id']}`",
                    "Duration": time.strftime("%M:%S", time.gmtime(data['duration'])),
                    "Preview": f"[Click Here]({data['preview']})" if data['readable'] else '`preview not readable`',
                    "Album": f"**Album Title: **{data['album']['title']}\n**Album ID: **`{data['album']['id']}`"
                },
                thumbnail=data['album']['cover_xl']
            )
            del data, result, parser
            
        except ctx.error_message:
            raise
        except:
            raise ctx.error_message(f'Did not found anything with the query of "{" ".join(args)}".')

    @command(["weather-forecast", "forecast"])
    @cooldown(10)
    @permissions(bot=['add_reactions'])
    async def weather(self, ctx, *args):
        parser = ctx.bot.Parser(args)
        parser.parse(('imperial',))
        
        try:
            _format = "F" if parser.has("imperial") else "C"
            assert bool(parser.other)
            location, nl = " ".join(parser.other)[:50], "\n"
            data = await ctx.bot.util.request(
                "https://weather.service.msn.com/find.aspx",
                src="outlook",
                weadegreetype=_format,
                culture="en-US",
                weasearchstr=location,
                xml=True
            )
            location_name = data['weatherdata']['weather'][0]['@weatherlocationname']
            embeds = [discord.Embed.from_dict(ctx.bot.Embed(
                ctx,
                title=f"Weather Forecast for {location_name[:70]} (Today)",
                fields={
                    "Current Condition": data['weatherdata']['weather'][0]['current']['@skytext'],
                    "Temperature": f"**Temperature: **{data['weatherdata']['weather'][0]['current']['@temperature']} °{_format}\n**Feels Like: **{data['weatherdata']['weather'][0]['current']['@feelslike']} °{_format}",
                    "Humidity": f"{data['weatherdata']['weather'][0]['current']['@humidity']}%",
                    "Wind Speeds": data['weatherdata']['weather'][0]['current']['@winddisplay']
                }
            ).dict)]
            
            if parser.has_multiple("forecast", "forecasts", "include-forecast", "include-forecasts"):
                for forecast in data['weatherdata']['weather'][0]['forecast']:
                    _embed = discord.Embed(
                        title=f"Weather Forecast for {location_name[:70]} ({forecast['@day']}, {forecast['@date']})",
                        color=ctx.me.color
                    ).add_field(name="Temperature", value=f"**Minimum Temperature: **{forecast['@low']} °{_format}\n**Maximum Temperature: **{forecast['@high']} °{_format}", inline=False
                    ).add_field(name="Condition", value=forecast['@skytextday'], inline=False
                    ).add_field(name="Precipitation", value=f"{(forecast['@precip'] if forecast['@precip'] else '0')}%", inline=False
                    ).add_field(name="Approximate Date", value=f"{forecast['@day']}, {forecast['@date']}", inline=False
                    ).add_useless_stuff(ctx)
                    embeds.append(_embed)
            else:
                del location_name, data, location, nl, parser
                return await ctx.reply(embed=embeds[0])
            del location_name, data, location, nl, parser
            paginator = ctx.bot.EmbedPaginator(ctx, embeds=embeds, show_page_count=True)
            del embeds
            return await paginator.execute()
        except (AssertionError, KeyError):
            raise ctx.error_message("Invalid location.")

    @command(['trending', 'msn'])
    @cooldown(7)
    async def news(self, ctx):
        try:
            data = await ctx.bot.util.request(
                "https://cdn.content.prod.cms.msn.com/singletile/summary/alias/experiencebyname/today",
                market="en-GB",
                source="appxmanifest",
                tenant="amp",
                vertical="news",
                xml=True
            )
            image = data["tile"]["visual"]["binding"][0].get("image", {}).get("@src", None)
            await ctx.embed(title=data["tile"]["visual"]["binding"][0]["text"]["#text"], image=(data["tile"]["visual"]["@baseUri"] + image if image else image), footer=(None if image else "<image not available>"))
            del data, image
        except:
            raise ctx.error_message("Oopsies, there was an error on searching the news.")

    @command(['urban-dictionary', 'define'])
    @cooldown(8)
    @require_args()
    async def urban(self, ctx, *args):
        search = False
        parser = ctx.bot.Parser(args)
        parser.parse(('search',))
        
        try:
            if parser.has("search"):
                args, search = tuple(parser.other), True
            
            data = await ctx.bot.util.request(
                "https://api.urbandictionary.com/v0/define",
                term=' '.join(args),
                json=True
            )
            
            if search:
                embed_list = ctx.bot.ChooseEmbed(ctx, data["list"][:10], key=(lambda x: f'{x["word"]} by {x["author"]} [{x["thumbs_up"]:,} :+1:, {x["thumbs_down"]:,} :-1:]'))
                result = await embed_list.run()
                del embed_list
                if not result:
                    return
            else:
                result = data["list"][0]
            
            await ctx.embed(title=result["word"], fields={
                "Post Info": f"**Author: **{result['author']}" + "\n" + f"{result['thumbs_up']:,} :+1: | {result['thumbs_down']:,} :-1:",
                "Definition": result["definition"].replace("\\\\", "\\"),
                "Written in": ctx.bot.util.timestamp(result['written_on']),
                "Example": result["example"]
            }, url=result['permalink'])
            del result, search, data
            
        except:
            raise ctx.error_message("Did not found anything corresponding to your search.")

    @command(['git'])
    @cooldown(10)
    @require_args(2)
    @permissions(bot=['add_reactions'])
    async def github(self, ctx, *args):
        try:
            await ctx.trigger_typing()
            nl = "\n" 
            if (args[0].lower() in ("user", "users", "profile")):
                data = await ctx.bot.util.request(
                    "https://api.github.com/users/" + "-".join(args[1:]),
                    github=True,
                    json=True
                )
                
                await ctx.embed(title=data["name"] or data["login"], fields={
                    "General": f"{('**Login Username: **' + data['login'] + nl if data['name'] else '')}**ID: **`{data['id']}`\n**Created at: **{ctx.bot.util.timestamp(data['created_at'])}\n**Updated at: **{ctx.bot.util.timestamp(data['updated_at'])}",
                    "Bio": data["bio"] if data.get("bio") else "`<no bio>`",
                    "Stats": f"**Followers: **{data['followers']}\n**Following: **{data['following']}\n**Public Repositories: **{data['public_repos']}\n**Public Gists: **{data['public_gists']}"
                }, thumbnail=data["avatar_url"], url=data["html_url"])
                del data, nl
                return
            elif (args[0].lower() in ("repos", "repositories")):
                data = await ctx.bot.util.request(
                    "https://api.github.com/users/" + " ".join(args[1:]) + "/repos",
                    github=True,
                    json=True
                )
                
                desc = [f"{'[ '+repo['language']+' ]' if repo['language'] else '[ ??? ]'} [{repo['full_name']}]({repo['html_url']}){' :fork_and_knife:' if repo['fork'] else ''}" for repo in data]
                paginator = ctx.bot.EmbedPaginator.from_long_array(ctx, desc, {
                    "title": ' '.join(args[1:]) + f"'s repositories [{len(data):,}]",
                    "thumbnail": { "url": data[0]["owner"]["avatar_url"] }
                })
                
                if not paginator:
                    await ctx.embed(title=' '.join(args[1:]) + f"'s repositories [{len(data):,}]", description="\n".join(desc), thumbnail=data[0]["owner"]["avatar_url"])
                    del desc, data, nl, paginator
                    return
                del desc, data, nl
                return await paginator.execute()
            elif (args[0].lower() == "gists"):
                data = await ctx.bot.util.request(
                    f"https://api.github.com/users/{'-'.join(args[1:])}/gists",
                    github=True,
                    json=True
                )
                
                if not data:
                    raise ctx.error_message("This user does not have any gists.")
                
                get_first_key = lambda x: x[list(x.keys())[0]]
                gists = list(map(lambda x: f'[{x.get("description", "<no desc>")}]({x.get("html_url", "https://google.com/")}) [**{get_first_key(x["files"]).get("language", "???")}**]', data))[:1900]
                paginator = ctx.bot.EmbedPaginator.from_long_array(ctx, gists, {
                    "thumbnail": { "url": data[0].get("owner").get("avatar_url") },
                    "title": data[0].get("owner").get("login") or "<no name>"
                })
                
                if not paginator:
                    await ctx.embed(title=(data[0].get("owner").get("login") or "<no name>") + f"'s GitHub Gists ({len(data):,})", description="\n".join(gists), thumbnail=data[0].get("owner").get("avatar_url"))
                    del get_first_key, gists, embed, data
                    return
                
                del get_first_key, gists, data
                return await paginator.execute()
            elif (args[0].lower() == "gist"):
                assert len(args[1]) >= 25
                data = await ctx.bot.util.request(
                    "https://api.github.com/gists/" + args[1],
                    github=True,
                    json=True
                )
                
                await ctx.embed(title=data.get("description", "<no description>"), thumbnail=data["owner"]["avatar_url"], url=data.get("html_url", "https://google.com/"), fields={
                    "General": f"**Created By: **[{data['owner']['login']}]({data['owner']['html_url']})\n**Created At: **{ctx.bot.util.timestamp(data['created_at'])}\n**Last Updated: **{ctx.bot.util.timestamp(data['updated_at'])}\n**Comments: **[{data['comments']}]({data['comments_url']})\n**Revisions: **{len(data['history']):,}",
                    "Files": "\n".join(map(lambda filename: f"[{filename}]({data['files'][filename]['raw_url']}) [{data['files'][filename].get('language', '???')}, {data['files'][filename]['size'] / 1000} MB]", data['files'].keys()))[:1000]
                })
                del data
                return
            elif (args[0].lower() in ("repo", "repository")):
                assert " ".join(args[1:]).count("/")
                data = await ctx.bot.util.request(
                    "https://api.github.com/repos/" + " ".join(args[1:]),
                    github=True,
                    json=True
                )
                
                await ctx.embed(title=data["full_name"] + (' [Fork of '+data['parent']['full_name']+']' if data['fork'] else ''), url=data["html_url"], fields={
                    'General': f"**Created at: **{ctx.bot.util.timestamp(data['created_at'])}\n**Updated at: **{ctx.bot.util.timestamp(data['updated_at'])}\n**Pushed at: **{ctx.bot.util.timestamp(data['pushed_at'])}\n**Programming Language: **{data['language'] if data.get('language') else '???'}{nl + '**License: **' + data['license']['name'] if data.get('license') else ''}",
                    'Description': data['description'] if data.get('description') else 'This repo is without description.', # haha nice reference there null
                    'Stats': f"**Stars: **{data['stargazers_count']}\n**Forks: **{data['forks_count']}\n**Watchers: **{data['watchers_count']}\n**Open Issues: **{data['open_issues_count']}"
                }, thumbnail=data["owner"]["avatar_url"])
                del data, nl
                return
            assert False
        except ctx.error_message as error:
            raise error
        except:
            return await ctx.bot.cmds.invalid_args(ctx)

    @command()
    @cooldown(5)
    @require_args()
    async def tv(self, ctx, *args):
        try:
            data = await ctx.bot.util.request(
                f'http://api.tvmaze.com/singlesearch/shows',
                json=True,
                q=' '.join(args)
            )
            assert bool(data)
        except:
            raise ctx.error_message("Did not found anything corresponding to your query.")
        
        try:
            star = str(':star:'*round(data['rating']['average'])) if data['rating']['average'] else 'No star rating provided.'
            await ctx.embed(title=data['name'], url=data['url'], description=ctx.bot.Parser.html_to_markdown(data['summary']), fields={
                'General Information': '**Status: **'+data['status']+'\n**Premiered at: **'+data['premiered']+'\n**Type: **'+data['type']+'\n**Language: **'+data['language']+'\n**Rating: **'+str(data['rating']['average'] if data['rating']['average'] else '`<not available>`')+'\n'+star,
                'TV Network': data['network']['name']+' at '+data['network']['country']['name']+' ('+data['network']['country']['timezone']+')',
                'Genre': str(', '.join(data['genres']) if len(data['genres']) else 'no genre avaliable'),
                'Schedule': ', '.join(data['schedule']['days'])+' at '+data['schedule']['time']
            }, image=data['image']['original'])
        except:
            raise ctx.error_message("There was an error on fetching the info.")

    @command(['spy', 'sp'])
    @cooldown(7, server_wide=True)
    @require_args(2)
    async def spotify(self, ctx, *args):
        await ctx.trigger_typing()
        parser = ctx.bot.Parser(args[1:])
        parser.parse(('card', 'search'))
        query = ' '.join(parser.other)[:50].lower()
        if not query:
            raise ctx.error_message('Please input a query.')
        
        if ("song" in args[0].lower()) or ("track" in args[0].lower()) or ("music" in args[0].lower()):
            result = await ctx.bot.spotify.request("/search", q=query, limit=20 if parser.has('search') else 1, offset=0, type='track')
            if (not result) or (not result['tracks']['items']) or (not result['tracks']['items'][0]):
                raise ctx.error_message(f"No such track found with the name of {query}.")
            
            result = result['tracks']['items']
            if not parser.has('search'):
                if parser.has('card') and result[0]['album'].get('images'):
                    panel = ctx.bot.Panel(ctx, title=result[0]['name'], subtitle=f"By {', '.join([i['name'] for i in result[0]['artists']]) if result[0]['artists'] else 'Unknown'}", description=f"On {result[0]['album']['name']}" if result[0]['album'] else 'No album', icon=result[0]['album']['images'][0]['url'])
                    await panel.draw()
                    await panel.send_as_attachment()
                    return panel.close()
                return await ctx.embed(**self._resolve_track(result[0], ctx))
            
            c = ctx.bot.ChooseEmbed(ctx, result, key=lambda x: f"*{x['name']}* by {x['artists'][0]['name'] if x['artists'] else 'Unknown'}")
            r = await c.run()
            
            if not r:
                return
            elif parser.has('card') and r['album'].get('images'):
                panel = ctx.bot.Panel(ctx, title=r['name'], subtitle=f"By {', '.join([i['name'] for i in r['artists']]) if r['artists'] else 'Unknown'}", description=f"On {r['album']['name']}" if r['album'] else 'No album', icon=r['album']['images'][0]['url'])
                await panel.draw()
                await panel.send_as_attachment()
                return panel.close()
            return await ctx.embed(**self._resolve_track(r, ctx))
        
        elif ("album" in args[0].lower()):
            result = await ctx.bot.spotify.request("/search", q=query, limit=20 if parser.has('search') else 1, offset=0, type='album')
            if (not result) or (not result['albums']['items']) or (not result['albums']['items'][0]):
                raise ctx.error_message(f"No such album found with the name of {query}.")
            result = result['albums']['items']
            if not parser.has('search'):
                if parser.has('card') and result[0]['images']:
                    panel = ctx.bot.Panel(ctx, title=result[0]['name'], subtitle=f"By {', '.join([i['name'] for i in result[0]['artists']]) if result[0]['artists'] else 'Unknown'}", description=f"{result[0]['total_tracks']} track{'' if (result[0]['total_tracks'] == 1) else 's'}", icon=result[0]['images'][0]['url'])
                    await panel.draw()
                    await panel.send_as_attachment()
                    return panel.close()
                return await ctx.embed(**self._resolve_album(result[0], ctx))
            
            c = ctx.bot.ChooseEmbed(ctx, result, key=lambda x: f"*{x['name']}* by {x['artists'][0]['name'] if x['artists'] else 'Unknown'}")
            r = await c.run()
            
            if not r:
                return
            elif parser.has('card') and r['images']:
                panel = ctx.bot.Panel(ctx, title=r['name'], subtitle=f"By {', '.join([i['name'] for i in r['artists']]) if r['artists'] else 'Unknown'}", description=f"{r['total_tracks']} track{'' if (r['total_tracks'] == 1) else 's'}", icon=r['images'][0]['url'])
                await panel.draw()
                await panel.send_as_attachment()
                return panel.close()
            return await ctx.embed(**self._resolve_album(r, ctx))
        
        elif ("artist" in args[0].lower()):
            result = await ctx.bot.spotify.request("/search", q=query, limit=20 if parser.has('search') else 1, offset=0, type='artist')
            if (not result) or (not result['artists']['items']) or (not result['artists']['items'][0]):
                raise ctx.error_message(f"No such artist found with the name of {query}.")
            result = result['artists']['items']
            if not parser.has('search'):
                if parser.has('card') and result[0]['images']:
                    panel = ctx.bot.Panel(ctx, title=result[0]['name'], subtitle=f"{result[0]['followers']['total']:,} follower{'' if (result[0]['followers']['total'] == 1) else 's'}", description=f"{result[0]['popularity']}% Popularity", icon=result[0]['images'][0]['url'])
                    await panel.draw()
                    await panel.send_as_attachment()
                    return panel.close()
                return await ctx.embed(**self._resolve_artist(result[0]))
            
            c = ctx.bot.ChooseEmbed(ctx, result, key=lambda x: x['name'])
            r = await c.run()
            
            if not r:
                return
            elif parser.has('card') and r['images']:
                panel = ctx.bot.Panel(ctx, title=r['name'], subtitle=f"{r['followers']['total']:,} follower{'' if (r['followers']['total'] == 1) else 's'}", description=f"{r['popularity']}% Popularity", icon=r['images'][0]['url'])
                await panel.draw()
                await panel.send_as_attachment()
                return panel.close()
            return await ctx.embed(**self._resolve_artist(r))
        
        elif ("playlist" in args[0].lower()):
            result = await ctx.bot.spotify.request("/search", q=query, limit=20 if parser.has('search') else 1, offset=0, type='playlist')
            if (not result) or (not result['playlists']['items']) or (not result['playlists']['items'][0]):
                raise ctx.error_message(f"No such playlist found with the name of {query}.")
            result = result['playlists']['items']
            if not parser.has('search'):
                if parser.has('card') and result[0]['images']:
                    panel = ctx.bot.Panel(ctx, title=result[0]['name'], subtitle=f"By {result[0]['owner']['display_name']}", description=result[0]["description"] or "This playlist is without description.", icon=result[0]['images'][0]['url'])
                    await panel.draw()
                    await panel.send_as_attachment()
                    return panel.close()
                return await ctx.embed(**self._resolve_playlist(result[0]))
            
            c = ctx.bot.ChooseEmbed(ctx, result, key=lambda x: x['name'])
            r = await c.run()
            
            if not r:
                return
            elif parser.has('card') and r['images']:
                panel = ctx.bot.Panel(ctx, title=r['name'], subtitle=f"By {r['owner']['display_name']}", description=r["description"] or "This playlist is without description.", icon=r['images'][0]['url'])
                await panel.draw()
                await panel.send_as_attachment()
                return panel.close()
            return await ctx.embed(**self._resolve_playlist(r))
        return await ctx.bot.cmds.invalid_args(ctx)

    @command()
    @cooldown(5)
    @require_args()
    async def itunes(self, ctx, *args):
        await ctx.trigger_typing()
        data = await ctx.bot.util.request(
            'https://itunes.apple.com/search',
            json=True,
            term=' '.join(args),
            media='music',
            entity='song',
            limit=10,
            explicit='no'
        )
        if not data['results']:
            raise ctx.error_message('No music found... oop')
        choose = ctx.bot.ChooseEmbed(ctx, data['results'], key=(lambda x: "["+x["trackName"]+"]("+x["trackViewUrl"]+")"))
        data = await choose.run()
        if not data: return
        
        panel = ctx.bot.Panel(ctx, title=data['trackName'], subtitle=data['artistName'], description=data['primaryGenreName'], icon=data['artworkUrl100'])
        await panel.draw()
        await panel.send_as_attachment()
        panel.close()

    @command(['tr', 'trans'])
    @cooldown(5)
    @require_args(2)
    async def translate(self, ctx, *args):
        await ctx.trigger_typing()
        parser = ctx.bot.Parser(args)
        parser.parse()
        try:
            destination = parser["to"].lower() if parser["to"] else parser.other[0].lower()
            if (destination not in LANGUAGES) and (len(destination) != 2):
                _filter = list(filter(
                    lambda x: destination in x.lower(), [LANGUAGES[x] for x in list(LANGUAGES)]
                ))
                assert bool(_filter)
                destination = _filter[0]
                del _filter
            toTrans = ' '.join(parser.other) if parser["to"] else ' '.join(parser.other[1:])
            translation = self.translator.translate(toTrans[:1000], src=parser["from"] if parser["from"] else "auto", dest=destination)
            await ctx.embed(title=f"{LANGUAGES[translation.src]} to {LANGUAGES[translation.dest]}", description=translation.text[:1900])
            del translation, destination, toTrans, parser
        except:
            return await ctx.bot.cmds.invalid_args(ctx)

    @command(['wiki'])
    @cooldown(7, channel_wide=True)
    @require_args()
    @permissions(bot=['add_reactions'])
    async def wikipedia(self, ctx, *args):
        await ctx.trigger_typing()
        
        page = self.Wikipedia.page(' '.join(args))
        if not page.exists():
            raise ctx.error_message('That page does not exist!')
        
        paginator = ctx.bot.EmbedPaginator.from_long_string(ctx, page.summary, data={
            "title": page.title,
            "url": page.fullurl
        })
        
        if not paginator:
            await ctx.embed(title=page.title, url=page.fullurl, description=page.summary[:2000])
            del page, paginator
        del page
        return await paginator.execute()
    
    @command(['movie', 'film'])
    @cooldown(5)
    @require_args()
    async def imdb(self, ctx, *args):
        await ctx.trigger_typing()
        try:
            query = " ".join(args[1:])
            res = self.ia.search_movie(query)
            
            choose = ctx.bot.ChooseEmbed(ctx, res[:10], key=(lambda x: x["long imdb title"]))
            movie = await choose.run()
            
            if not movie:
                return
            await ctx.trigger_typing()
            
            data = self.ia.get_movie_main(movie.movieID)["data"]
            votes = (":star:" * round(data["rating"])) + f' ({data["rating"]}, {data["votes"]} votes)' if (data.get("votes") and data.get("rating")) else "<data not available>"

            await ctx.embed(title=movie["long imdb title"], url=self.ia.get_imdbURL(movie), image=movie["full-size cover url"], fields={
                "Plot": data["plot outline"].split(".")[0][:1000],
                "Movie Ratings": votes,
                "Directors": ", ".join([i["name"] for i in data["directors"] if i.get("name")]),
                "Producers": ", ".join([i["name"] for i in data["producers"] if i.get("name")]),
                "Writers": ", ".join([i["name"] for i in data["writers"] if i.get("name")])
            })
            del res, data, votes, choose, movie, query
        except Exception as e:
            raise ctx.error_message("The movie query does not exist.\n" + str(e))
        
def setup(client):
    client.add_cog(apps())
