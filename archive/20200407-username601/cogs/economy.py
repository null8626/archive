import discord
from random import randint, choice
from discord.ext import commands
from decorators import *
from json import loads
from time import time
from asyncio import sleep
from random import choice, randint
from gc import collect

class economy(commands.Cog):
    def __init__(self, client):
        self.blacklisted_words = (
            "nigga",
            "nigger",
            "discord.gg",
            "http://",
            "https://",
            "discordapp.com",
            "discord.com",
            "fuck",
            "shit",
            "bitch",
            "dick",
            "pussy",
            "top.gg/bot",
            "top.gg/server"
        )
        
        self.fish_json = loads(open(client.util.json_dir+'/fish.json', 'r').read())
        self.works = loads(open(client.util.json_dir+'/work.json', 'r').read())['works']
        self.db = client.db
        
        # { friendly name: tuple[ID, cooldown (in seconds)], cooldown string}
        self.cooldown_ids = {
            'Bet':      ('bt', 300,  '5 minutes'),
            'Beg':      ('bg', 600,  '10 minutes'),
            'Fish':     ('fs', 600,  '10 minutes'),
            'Work':     ('wr', 1800, '30 minutes'),
            'Rob':      ('rb', 900,  '15 minutes'),
            'Transfer': ('tf', 300,  '5 minutes')
        }
    
    def getfish(self):
        found, ctx = False, None
        for i in self.fish_json['results']['overall']:
            if randint(1, i['chance']) == 1:
                found, ctx = True, i
                break
        if not found: ctx = choice(self.fish_json['results']['else'])
        return {
            "catched": found,
            "ctx": ctx
        }
    
    def new_profile(self, ctx):
        data = {
            "userid": ctx.author.id,
            "lastDaily": None,
            "bal": 0,
            "bankbal": 0
        }
    
        ctx.bot.db.add("economy", data)
        return data
    
    async def economy_cooldown(self, ctx, profile = None):
        _ctx = self.cooldown_ids[ctx.command.name[0].upper() + ctx.command.name[1:].lower()]
        data = profile or self.db.get('economy', { 'userid': ctx.author.id })
        
        if (cooldown := data.get(f'cd_{_ctx[0]}')):
            can_be_used_in = cooldown - time()
        else:
            self.db.modify('economy', self.db.types.CHANGE, { 'userid' : ctx.author.id }, { f'cd_{_ctx[0]}': time() + _ctx[1] })
            return data
        
        cd = (0 if can_be_used_in < 0 else round(can_be_used_in))
        if not cd:
            self.db.modify('economy', self.db.types.CHANGE, { 'userid' : ctx.author.id }, { f'cd_{_ctx[0]}': time() + _ctx[1] })
            return data
        raise ctx.error_message(f'This command is on an economy cooldown. **You can run `{ctx.prefix}{ctx.command.name}` again in {ctx.bot.util.strfsecond(cd)}.**', description=True)
    
    @command(['cooldown', 'cd', 'cds'])
    @cooldown(2)
    @require_profile(True)
    async def cooldowns(self, ctx):
        await ctx.trigger_typing()
        data = dict([(k, v - time()) for k, v in ctx.profile.items() if k.startswith('cd_')])
        
        daily_cooldown = (not ((time() - ctx.profile.get("lastDaily", 0)) > 43200))
        fields = {
            "Daily": f'{":x:" if daily_cooldown else ":white_check_mark:"} {ctx.bot.util.strfsecond((ctx.profile["lastDaily"] + 43200) - time()) if daily_cooldown else "0 seconds"}{" (12 hours by default)" if daily_cooldown else ""}'
        }
        color = (255, 0, 0)
        
        for k, v in self.cooldown_ids.items():
            cd = data[f'cd_{v[0]}'] if data.get(f'cd_{v[0]}', -1) > 0 else 0
            if (not cd) and (color == (255, 0, 0)):
                color = (0, 255, 0)
            
            fields[k] = f"{':x:' if cd else ':white_check_mark:'} {ctx.bot.util.strfsecond(cd)}{f' ({v[2]} by default)' if cd else ''}"
    
        return await ctx.embed(
            title=f"{ctx.author.display_name}'s economy cooldowns",
            thumbnail=ctx.author.avatar_url,
            fields=fields,
            color=color
        )
    
    @command()
    @cooldown(2)
    @require_args()
    @require_profile(True)
    async def bet(self, ctx, *args):
        await ctx.trigger_typing()
        await self.economy_cooldown(ctx, ctx.profile)
    
        lucky = choice([False, True])
        try:
            amount = ctx.bot.Parser.get_numbers(args)
            assert amount[0] in range(0, 69421)
        except:
            return await ctx.bot.cmds.invalid_args(ctx)
        
        await ctx.reply(embed=discord.Embed(title=(f"Congratulations! {ctx.author.display_name} just won {amount[0]:,} bobux!" if lucky else f"Yikes! {ctx.author.display_name} just lost {amount[0]:,} bobux..."), color=getattr(discord.Color, "green" if lucky else "red")()))
        self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bal": (amount[0] if lucky else -amount[0])})
        
    @command()
    @cooldown(2)
    @require_profile(True)
    async def beg(self, ctx):
        await self.economy_cooldown(ctx, ctx.profile)
        award = randint(100, 800)
        self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bal": award})
        return await ctx.success_embed(f'You begged and got {award:,} bobux!')
  
    @command(['fishing'])
    @cooldown(2)
    @require_profile(True)
    async def fish(self, ctx):
        await ctx.trigger_typing()
        await self.economy_cooldown(ctx, ctx.profile)
        await sleep(randint(3, 8))
        res = self.getfish()
        
        if res['catched']:
            award = randint(res['ctx']['worth']['min'], res['ctx']['worth']['max'])
            self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bal": award})
            return await ctx.success_embed(f"{res['ctx']['emote']} | Congratulations! You caught a {res['ctx']['name']} and sell it worth for {award:,} bobux!")
        raise ctx.error_message(f"Yikes! You only caught {res['ctx']}... Try again later!")

    @command(['delete', 'deletedata', 'deldata', 'del-data', 'delete-data'])
    @cooldown(3600)
    @require_profile()
    async def reset(self, ctx):
        embed = await ctx.reply(embed=discord.Embed(title="Are you sure? This action is irreversible!", description="Reply with `yes` or `no`. Not replying will default to `no`.", color=discord.Color.red()))
        wait = ctx.bot.WaitForMessage(ctx, check=(lambda x: x.channel == ctx.channel and x.author == ctx.author and x.content.lower() in ["yes", "y", "no", "n"]))
        message = await wait.get_message()
        del wait

        if (not message) or (message.content.lower() in ['n', 'no']):
            return await ctx.success_embed("OK. No it is then.")
        self.db.delete("economy", {"userid": ctx.author.id})
        return await embed.edit(embed=discord.Embed(title="Alright. Your profile is gone. Reduced to atoms.", color=discord.Color.orange()))

    @command()
    @cooldown(2)
    @require_profile(True)
    async def work(self, ctx):
        await ctx.trigger_typing()
        await self.economy_cooldown(ctx, ctx.profile)
        reward = randint(100, 500)
        job = choice(self.works)
        self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bal": reward})
        return await ctx.success_embed(f"{ctx.author.display_name} worked {job} and earned {reward:,} bobux!")
    
    @command()
    @cooldown(7)
    @require_profile(True)
    async def daily(self, ctx, *args):
        await ctx.trigger_typing()
        
        if (not ctx.profile["lastDaily"]) or ((time() - ctx.profile["lastDaily"]) > 43200): # 43200 is 12 hours.
            
            if ctx.profile.get("streak") and ctx.profile["lastDaily"]: # idk how streak works pls help my brain is fried
                streak = ctx.profile["streak"]
                if (time() - ctx.profile["lastDaily"] - 43200) < 43200:
                    streak += 1
                else:
                    streak = 1 # reset the streak
            else:
                streak = 1
            
            reward = 250 * streak
            new_data = {
                "bal": ctx.profile["bal"] + reward,
                "lastDaily": time(),
                "streak": streak
            }
            
            await ctx.success_embed(f"You earned your Daily for {reward:,} bobux!" + "\n" + f"Your streak: {streak:,} (250 x {streak:,} bobux)")
            self.db.modify("economy", self.db.types.CHANGE, {"userid": ctx.author.id}, new_data)
        else:
            raise ctx.error_message(f"You can earn your daily in {ctx.bot.util.strfsecond((ctx.profile['lastDaily'] + 43200) - time())}!")

    @command()
    @cooldown(2)
    @require_args()
    @require_profile(True)
    async def transfer(self, ctx, *args):
        await ctx.trigger_typing()
        await self.economy_cooldown(ctx, ctx.profile)
        try:
            member = ctx.message.mentions[0]
            amount = ctx.bot.Parser.get_numbers(args)
            assert amount[0] in range(0, 100000), "The limit is 100.000 bobux!"
            assert self.db.exist("economy", {"userid": member.id}), f"{member.display_name} does not have a profile!"
            await ctx.success_embed(f"Successfully transferred {amount[0]:,} bobux to {member.display_name}!")
        
            self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bal": -amount[0]})
            self.db.modify("economy", self.db.types.INCREMENT, {"userid": member.id}, {"bal": amount[0]})
        except Exception as e:
            raise ctx.error_message(str(e))
    
    @command(['steal', 'crime', 'stole', 'robs'])
    @cooldown(2)
    @require_args(2)
    @require_profile(True)
    async def rob(self, ctx, *args):
        await ctx.trigger_typing()
        await self.economy_cooldown(ctx, ctx.profile)
    
        try:
            number = ctx.bot.Parser.get_numbers(args)[0]
            assert ctx.message.mentions, "Please mention someone to rob!"
            member = ctx.message.mentions[0]
            assert not member.bot, f"{member.display_name} is a botum."
            assert member != ctx.author, f"Error! `RecursionError: maximum recursion depth exceeded`"
            
            victim_data = self.db.get("economy", {"userid": member.id})
            assert victim_data, f"{member.display_name} has no profile!"
            assert victim_data["bal"], f"{member.display_name} only has 0 bobux! He is too poor to be robbed!"
            assert member.status != discord.Status.offline, f"{member.display_name} is currently offline!"
            
            if not number:
                number = randint(0, data["bal"])
            assert number <= ctx.profile["bal"], f"Number must be below the opponent's bobux. ({victim_data['bal']})"
            assert ctx.profile["bal"] > 750, f"You need at least 750 bobux ({750 - data['bal']} more) to rob someone."
            
            await ctx.success_embed(f"Successfully robbed {member.display_name} for {number:,} bobux.")
            self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bal": number})
            self.db.modify("economy", self.db.types.INCREMENT, {"userid": member.id})
        except AssertionError as e:
            raise ctx.error_message(str(e))
    
    @command(['dep'])
    @cooldown(10)
    @require_args()
    @require_profile(True)
    async def deposit(self, ctx, *args):
        
        if args[0].lower() == 'all':
            await ctx.success_embed('OK. Deposited all of your bobux to the bobux bank.')
            self.db.modify("economy", self.db.types.CHANGE, {"userid": ctx.author.id}, {"bankbal": ctx.profile['bal']})
            self.db.modify("economy", self.db.types.CHANGE, {"userid": ctx.author.id}, {"bal": 0})
            return

        try:
            num = ctx.bot.util.Parser.get_numbers(args)
            assert num < ctx.profile['bal'], 'Your input has more bobux than the one in your balance!'
            await ctx.success_embed(f'OK. Deposited {num:,} bobux to your bank.')

            self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bankbal": num})
            self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bal": -num})
        except AssertionError as e:
            raise ctx.error_message(str(e))
        except Exception as e:
            return await ctx.bot.cmds.invalid_args(ctx)
    
    @command()
    @cooldown(5)
    @require_args()
    @require_profile(True)
    async def withdraw(self, ctx, *args):
        if args[0].lower() == 'all':
            await ctx.success_embed('OK. Withdrew all of your bobux from the bobux bank.')
            self.db.modify("economy", self.db.types.CHANGE, {"userid": ctx.author.id}, {"bankbal": 0})
            self.db.modify("economy", self.db.types.CHANGE, {"userid": ctx.author.id}, {"bal": ctx.profile['bankbal']})
            return

        try:
            num = ctx.bot.util.Parser.get_numbers(args)
            assert num < ctx.profile['bankbal'], 'Your input has more bobux than the one in the bank!'
            await ctx.success_embed(f'OK. Withdrew {num:,} bobux from the bank.')

            self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bal": num})
            self.db.modify("economy", self.db.types.INCREMENT, {"userid": ctx.author.id}, {"bankbal": -num})
        except AssertionError as e:
            raise ctx.error_message(str(e))
        except Exception:
            return await ctx.bot.cmds.invalid_args(ctx)

    @command(['lb', 'leader', 'leaders', 'rich', 'richest', 'top'])
    @cooldown(10)
    async def leaderboard(self, ctx, *args):
        # PLEASE SOMEONE HELP ME FIND A BETTER WAY TO DO THIS
        # THIS ALGORITHM IS SHIT
    
        await ctx.trigger_typing()
        if args:
            if args[0].lower() == "global":
                data = list(self.db.get_all("economy"))
                sorted_bal = sorted(map(lambda x: x["bal"], data))[::-1][:20]
                ids = []
                description = ""
                
                for i, bal in enumerate(sorted_bal):
                    _data = list(filter(lambda x: x["bal"] == bal and x["userid"] not in ids, data))[0]
                    ids.append(_data["userid"])
                    user = ctx.bot.get_user(_data["userid"])
                    description += f"{i + 1}. **{user.name if user else '`???`'}** {_data['bal']:,} :money_with_wings:" + "\n"
                await ctx.embed(title=f"{ctx.me.display_name} world-wide leaderboard", description=description)
                del ids, description, sorted_bal, data
                collect()
                return
            elif args[0].lower() in ("local", "server", "server-wide", "serverwide"):
                pass
            else:
                return await ctx.bot.cmds.invalid_args(ctx)
        
        member_ids = list(map(lambda x: x.id, ctx.guild.members))
        data = list(filter(lambda x: x["userid"] in member_ids, list(self.db.get_all("economy"))))
        limit = len(list(data)) if len(list(data)) < 10 else 10
        if limit < 3:
            del data, limit, member_ids
            collect()
            raise ctx.error_message("This server has less than 3 members with a profile, thus a leaderboard cannot happen!")
        
        sorted_bal = sorted(list(map(lambda x: x["bal"], data)))[::-1][:limit]
        ids = []
        description = ""
        
        for i, bal in enumerate(sorted_bal):
            _data = list(filter(lambda x: x["bal"] == bal and x["userid"] not in ids, data))[0]
            ids.append(_data["userid"])
            user = ctx.bot.get_user(_data["userid"])
            description += f"{i + 1}. **{user.name if user else '<???>'}** {_data['bal']:,} :money_with_wings:" + "\n"
        await ctx.embed(title=f"{ctx.me.display_name} server-wide leaderboard", description=description, thumbnail=ctx.guild.icon_url)
        del ids, description, sorted_bal, data, member_ids, limit
        collect()

    @command(['balance', 'profile', 'economy'])
    @cooldown(5)
    @permissions(bot=['attach_files'])
    async def bal(self, ctx, *args):
        await ctx.trigger_typing()
    
        if args:
            parser = ctx.bot.Parser(args)
            parser.parse(("desc", "setdesc", "description", "bio", "color", "set-color", "col", "card", "image"))
            
            if parser.has_multiple("desc", "setdesc", "description", "bio"):
                await ctx.trigger_typing()
                try:
                    text = " ".join(parser.other)
                    assert text != "", "Please add a text for your new bio."
                    for blacklisted_word in self.blacklisted_words:
                        assert blacklisted_word not in text, "Please do not include any links or bad words in your bio!"
                    data = self.db.get("economy", {"userid": ctx.author.id}) or self.new_profile(ctx)
                    
                    assert data["bal"] > 500, f"You need at least 500 bobux to change bio ({(500 - data['bal']):,} more bobux required)"

                    await ctx.success_embed(f"Successfully changed your bio.")
                    self.db.modify("economy", self.db.types.CHANGE, {"userid": ctx.author.id}, {"desc": text[:50]})
                    del parser, data, text
                    return
                except Exception as e:
                    raise ctx.error_message(str(e))
            elif parser.has_multiple("color", "set-color", "col"):
                try:
                    color = ctx.bot.Parser.parse_color(parser.other)
                    assert bool(color)
                    data = self.db.get("economy", {"userid": ctx.author.id}) or self.new_profile(ctx)
                    assert data["bal"] > 1000, f"You need at least 1,000 bobux to change bio ({(1000 - data['bal']):,} more bobux required)"
                except ValueError:
                    return await ctx.bot.cmds.invalid_args(ctx)
                except AssertionError as e:
                    raise ctx.error_message(str(e))
                
                await ctx.reply(embed=discord.Embed(title="Changed the color for your profile to `"+ ('#%02x%02x%02x' % color).upper() +"`.", color=discord.Color.from_rgb(*color)))
                self.db.modify("economy", self.db.types.CHANGE, {"userid": ctx.author.id}, {"color": str(color).replace(" ", "")[1:-1]})
                del parser
                return
            elif parser.has_multiple("card", "image"):
                member = ctx.bot.Parser.parse_user(ctx, parser.other)                
                data = self.db.get("economy", {"userid": member.id})

                if not data:
                    raise ctx.error_message(f"{member.display_name} does not have any profile.")
                card = ctx.bot.ProfileCard(ctx, member, profile=data, font_path=ctx.bot.util.fonts_dir + "/NotoSansDisplay-Bold.otf")
                byte = await card.draw()
                await ctx.send_image(byte)
                await card.close()
                del card, byte, data, member, parser
                return

        member = ctx.bot.Parser.parse_user(ctx, args)
        data = self.db.get("economy", {"userid": member.id})
        if not data:
            if member != ctx.author:
                raise ctx.error_message(f"{member.display_name} does not have any profile!")
            
            data = self.db.add("economy", {
                "userid": ctx.author.id,
                "lastDaily": None,
                "bal": 0,
                "bankbal": 0
            })
        
        streak = data.get("streak", 1)
        
        embed = ctx.bot.Embed(ctx, title=f"{member.display_name}'s profile", thumbnail=member.avatar_url, fields={
            "Balance": f"{data['bal']:,} bobux" + "\n" f"{data['bankbal']:,} bobux (bank)",
            "Description": data["desc"] if data.get("desc") else "**<this profile is without description>**",
            "Daily": (f"**[:white_check_mark: can be claimed using `{ctx.prefix}daily`]**" if ((not data["lastDaily"]) or ((time() - data["lastDaily"]) > 43200)) else f"Can be claimed in {ctx.bot.util.strfsecond((data['lastDaily'] + 43200) - time())}") + "\n" + f"Streak: {streak} (Next daily reward: {(250 * (streak + 1)):,} bobux)"
        }, footer="Daily streaks will be reset back to 1 if daily is not claimed after 24 hours.")
        
        if data.get("color"):
            rgb = tuple([int(i) for i in data["color"].split(",")])
            embed.color(rgb)
            embed.add_field("Color", f"**Hex: **{('#%02x%02x%02x' % rgb).upper()}"+"\n"+f"**RGB: **{rgb[0]}, {rgb[1]}, {rgb[2]}")
            del rgb
        
        await embed.send()
        del embed, data

def setup(client):
    client.add_cog(economy(client))