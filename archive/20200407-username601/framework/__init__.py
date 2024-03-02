# This framework folder is the Discord Framework my bot use.
# You can use the code in this directory for your bot.
# I am not really planning on uploading it to PyPI though...

from inspect import getmembers, getsource, signature
from discord.ext import commands as _commands
from requests import post as _post_message
from functools import wraps as _wraps
from gc import collect as _collect
from traceback import format_exc
from datetime import datetime
from aiohttp import FormData
from io import BytesIO
from json import dumps
from os import getenv
import discord as dpy
import signal as sg
import asyncio

from .oreo import Oreo
from .parser import Parser
from .panel import CustomPanel
from .colorthief import Smart_ColorThief
from .message import embed, Paginator, ChooseEmbed, WaitForMessage
from .util import Util, error_message, SpotifyAPI
from .games import GuessTheFlag, Slot, TicTacToe, RockPaperScissors, GeographyQuiz, MathQuiz, GuessAvatar, Trivia, GuessMyNumber, Hangman
from .canvas import ServerCard, UserCard, ProfileCard, GDLevel, Blur, ImageClient
from .database import Database
from .lego import legofy

VALID_FILENAMES = ('png', 'jpg', 'jpeg', 'gif', 'svg')
UNUSED_MESSAGE_HANDLERS = (
    "pinned",
    "flags",
    "application",
    "activity",
    "mention_everyone",
    "tts",
    "type",
    "attachments",
    "embeds",
    "nonce",
    "mention_roles",
    "call"
)

def get_prefix(config_file: str = "config.ini"):
    from configparser import ConfigParser # i don't recommend calling this function more than once
    _config = ConfigParser()
    _config.read(config_file)
    return _config["bot"].get("prefix", "1")

def modify_discord_py_functions():
    """
    Modify the discord.py functions and variables that are unused by the bot.
    This also changes how the bot caches stuff.
    
    Things changed such as:
      - How the bot starts.
      - How the bot shuts down.
      - How the bot handles messages.
      - Added several custom functions here and there.
      - Several functions now uses the lower-level functions of discord.py.
      - The bot now only caches messages made by the bot itself.
      - The bot now caches guild emojis as string.
      - The bot doesn't cache other bots.
      - The discord.py's Message object doesn't retrieve the stickers, embeds, nonce, role_mentions, etc.
      - The discord.py's Attachment object is not used. <message>.attachments will return an array of attachment URLs instead.
    """
    
    ### BEGIN MADNESS ###
    for name, value in getmembers(dpy.User):
        if name.startswith("__") or value.__class__.__name__ != "function" or "can only be used by non-bot accounts" not in getsource(value):
            continue
        delattr(dpy.User, name)

    for handler in UNUSED_MESSAGE_HANDLERS:
        setattr(dpy.Message, f"_handle_{handler}", (lambda s, *a: None))
    
    ### BEGIN MADNESS ###
    def _hooked_wrapped_callback(command, ctx, coro):
        @_wraps(coro)
        async def wrapped():
            try:
                extra_args = ctx.message.content[:500].split()[1:]
                await coro(command.cog, ctx, *(extra_args if extra_args and len(signature(coro).parameters) > 2 else tuple()))
            except asyncio.CancelledError:
                ctx.command_failed = True
                return
            except Exception as exc:
                ctx.command_failed = True
                raise _commands.CommandInvokeError(exc) from exc
            finally:
                await command.call_after_hooks(ctx)
            return None
        return wrapped
    
    def _embed_add_useless_stuff(self, ctx, disable_color: bool = False):
        self._footer = {
            "text": "Command executed by "+str(ctx.author),
            "icon_url": str(ctx.author.avatar_url)
        }
        self.timestamp = datetime.now()
        
        if not disable_color:
            self.colour = ctx.me.colour
        return self
    
    async def _send_image(self, url, alexflipnote: bool = False, content: str = None, file_format="png"):
        try:
            if isinstance(url, BytesIO):
                return await self.bot.http.send_files(self.channel.id, content=content, files=[dpy.File(url, f"file.{file_format}")], reference=self.message)
        
            session = self.bot.util.alex_client if alexflipnote else self.bot.http._HTTPClient__session
            
            async with session.get(url) as data:
                _bytes = await data.read()
                assert data.status < 400, "API returns a bad status code"
                assert data.headers['Content-Type'].startswith("image/"), "Content does not have an image."
                extension = "." + data.headers['Content-Type'][6:].lower()
                buffer = self.bot.util._crop_out_memegen(self, _bytes) if url.startswith("https://api.memegen.link/") else BytesIO(_bytes)
                await self.bot.http.send_files(self.channel.id, content=content, files=[dpy.File(buffer, f"file{extension}")], reference=self.message)
                del extension, _bytes, data, buffer
                _collect()
        except Exception as e:
            raise self.error_message(f"Image not found.\n{str(e)}")
    
    async def _success_embed(self, message=None, description=None, delete_after=None):
        response = await self._state.http.send_message(self.channel.id, content="", embed={
            "title": message,
            "description": description,
            "color": 3066993
        })
        
        if delete_after:
            await asyncio.sleep(delete_after)
            return await self._state.http.delete_message(channel_id=self.channel.id, message_id=response["id"])
    
    async def _send_embed(self, *args, **kwargs):
        _builtin_embed = self.bot.Embed(self, *args, **kwargs)
        await _builtin_embed.send()
        del _builtin_embed
    
    def _parse_message_create(self, data):
        if (not hasattr(self, "_is_ready")) or data["type"] or (not data.get("guild_id")) or data.get("webhook_id"):
            return
        elif data["author"].get('bot') and (not data["author"]["id"] == str(self.user.id)):
            return
        
        channel, _ = self._get_guild_channel(data)
        if not data["author"].get("bot"): self.dispatch('message', dpy.Message(channel=channel, data=data, state=self))
        else: self._messages.append(dpy.Message(channel=channel, data=data, state=self))
        del channel, data
    
    async def _run_command(self, message):
        if not message.content.startswith(self.command_prefix):
            return
        
        ctx = _commands.Context(prefix=self.command_prefix, bot=self, message=message, args=message.content.split(" ")[1:])
        if not ctx.channel.permissions_for(ctx.me).send_messages:
            return # what's the point of running a command if the bot doesn't have the perms to send messages kekw
        
        try:
            command_name = message.content[len(self.command_prefix):].split()[0]
            ctx.command = self.all_commands[command_name.lower()]
            if not await ctx.command.can_run(ctx):
                return
            
            if ctx.command._max_concurrency:
                await ctx.command._max_concurrency.acquire(ctx)
            
            try:
                ctx.command._prepare_cooldowns(ctx)
            except:
                if ctx.command._max_concurrency:
                    await ctx.command._max_concurrency.release(ctx)
                raise
            
            await _hooked_wrapped_callback(ctx.command, ctx, ctx.command.callback)()
        except (KeyError, IndexError):
            return
        except Exception as exc:
            self.dispatch("command_error", ctx, exc, format_exc())
        else:
            self.command_uses += 1
        del ctx, command_name
    
    def _remove_guild(self, guild):
        self._guilds.pop(guild.id, None)
        for emoji in map(lambda x: int(x.split(':')[2].split('>')[0]), guild.emojis):
            self._emojis.pop(emoji, None)
        del guild
        _collect()
    
    def _event_on_close(self):
        print("Closing bot...")
        if hasattr(self, "_is_closed") and self._is_closed:
            print("Closing process closed because the bot is already closed.")
            return
        
        total_uptime = self.util.strfsecond(datetime.now().timestamp() - self.util._start)
        _post_message(f"{dpy.http.Route.BASE}/channels/{self.util.status_channel}/messages", headers={ "Authorization": f"Bot {self.http.token}", "Content-Type": "application/json" }, data=dumps({
            "embed": { "title": "Bot is down :(", "description": "The bot is down.", "color": 16711680, "fields": [
                {"name": "Commands run throughout run-time",
                "value": str(self.command_uses), "inline": True}, { "name": "Total Uptime",
                "value": total_uptime, "inline": True}
            ], "footer": { "text": "Please note that not all down-times are due to hosting problems. This could be a bug-fix or a development update." } }
        }))
        data = self.db.get("config", {"h": True})
        
        current_time = datetime.now()
        if len(data["down_times"]) > 20:
            data["down_times"].pop(0)
        
        data["down_times"].append(f'{current_time.strftime("%Y-%m-%dT%H:%M:%SZ")}|{self.util.strfsecond(current_time.timestamp() - self.util._start)}')
        self.db.modify("config", self.db.types.CHANGE, {"h": True}, {
            "commands_run": data["commands_run"] + self.command_uses,
            "down_times": data["down_times"],
            "online": False,
            "spotify_credentials": {
                "token": self.spotify._token,
                "expiry_date": self.spotify._token_expiry_date
            }
        })
        self._is_closed = True
        del data, current_time, total_uptime
        self.loop.stop()
    
    def _store_user(self, data):
        user_id = int(data["id"])
        try:
            return self._users[user_id]
        except:
            user = dpy.User(state=self, data=data)
            if (data["discriminator"] != "0000") and (not data.get("bot")):
                self._users[user_id] = user
            return user
    
    def _store_emoji(self, guild, data):
        guild_data = (guild,) if guild else ()
        self._emojis[int(data["id"])] = (data["name"], int(data["id"]), data.get("animated", False), dpy.utils.snowflake_time(int(data["id"])), bool(guild), *guild_data)
        return f"<{'a' if data.get('animated') else ''}:{data['name']}:{data['id']}>"
    
    def _run_bot(self, *args, **kwargs):
        loop = self.loop
        try:
            loop.add_signal_handler(sg.SIGINT,  self.event_on_close)
            loop.add_signal_handler(sg.SIGTERM, self.event_on_close)
        except:
            pass
    
        async def runner():
            try:
                await self.start(*args, **kwargs)
            finally:
                if not self.is_closed():
                    await self.close()
                self.event_on_close()
    
        def stop_loop_on_completion(f):
            loop.stop()
    
        future = asyncio.ensure_future(runner(), loop=loop)
        future.add_done_callback(stop_loop_on_completion)
        try:
            loop.run_forever()
        finally:
            future.remove_done_callback(stop_loop_on_completion)
            self.event_on_close()
        try:
            try:
                task_retriever = asyncio.Task.all_tasks
            except:
                task_retriever = asyncio.all_tasks
            tasks = {t for t in task_retriever(loop=loop) if not t.done()}
            if not tasks:
                return
            for task in tasks:
                task.cancel()
            loop.run_until_complete(asyncio.gather(*tasks, return_exceptions=True))
            for task in tasks:
                if task.cancelled():
                    continue
                if task.exception() is not None:
                    loop.call_exception_handler({
                        'message': 'Unhandled exception during Client.run shutdown.',
                        'exception': task.exception(),
                        'task': task
                    })
            loop.run_until_complete(loop.shutdown_asyncgens())
            loop.run_until_complete(self.http._HTTPClient__session.close())
        finally:
            loop.close()
    
        if not future.cancelled():
            try:
                return future.result()
            except KeyboardInterrupt:
                return
    
    def _parse_guild_emojis_update(self, data):
        guild = self._get_guild(int(data['guild_id']))
        if not guild:
            return
        
        before_emojis = guild.emojis
        for emoji in map(lambda x: int(x.split(':')[2].split('>')[0]), before_emojis):
            self._emojis.pop(emoji, None)
        guild.emojis = tuple(map(lambda d: self.store_emoji(guild, d), data['emojis'])) # don't dispatch since we're not using it
    
    def _message_init(self, *, state, channel, data):
        self._state = state
        self.id = int(data['id'])
        self.attachments = [a['url'] for a in data['attachments'] if a.get('url') and (a['filename'].split('.')[-1].lower() in VALID_FILENAMES)] if data['attachments'] else []
        self.channel = channel
        self.type = dpy.MessageType.default
        self._edited_timestamp = dpy.utils.parse_time(data['edited_timestamp'])
        self.content = data['content']
        self.reactions = []
        
        for h in ('author', 'member', 'mentions'):
            try:    getattr(self, f'_handle_{h}')(data[h])
            except: continue
    
    async def _message_edit(self, **fields):
        kwargs = { "content": fields.get("content", "") }
        if (embed := fields.get("embed")):
            kwargs['embed'] = embed.to_dict()
        return await self._state.http.edit_message(self.channel.id, self.id, **kwargs)
        
    async def _raw_send_message(self, channel_id, **kwargs):
        r = dpy.http.Route('POST', '/channels/{channel_id}/messages', channel_id=channel_id)
        kwargs['allowed_mentions'] = {'replied_user': False, 'parse': []}
        
        if (reference := kwargs.pop('reference', None)):
            kwargs['message_reference'] = { 'message_id': reference.id, 'guild_id': reference.guild.id }
        
        return await self.request(r, json=kwargs)

    async def _raw_send_files(self, channel_id, files, reference=None, **kwargs):
        r = dpy.http.Route('POST', '/channels/{channel_id}/messages', channel_id=channel_id)
        
        json = { 'content': kwargs.pop('content', ''), 'allowed_mentions': {'replied_user': False, 'parse': []} }
        if reference:
            json['message_reference'] = { 'message_id': reference.id, 'guild_id': reference.guild.id }
        
        form = FormData()
        form.add_field('payload_json', dpy.utils.to_json(json))
        form.add_field('file', files[0].fp, filename=files[0].filename, content_type='application/octet-stream')
        return await self.request(r, data=form, files=files)
    
    async def _send_message(self, content = "", embed = None, file = None, delete_after = None, reference = None):
        channel = await self._get_channel()
        
        if embed and (not isinstance(embed, dict)):
            embed = embed.to_dict()
        
        if file:
            try:     data = await self._state.http.send_files(channel.id, files=[file], content=content, embed=embed, reference=reference)
            finally: file.close()
        else:
            data = await self._state.http.send_message(channel.id, content=content, embed=embed, reference=reference)
        
        msg = dpy.Message(state=self._state, channel=channel, data=data)
        if delete_after:
            await msg.delete(delay=delete_after)
        return msg

    ### END MADNESS ###
    
    delattr(dpy.Client, "fetch_user_profile") # this one is made for selfbots only
    delattr(dpy.Embed, "__len__")
    setattr(dpy.Message, "__init__", _message_init)
    setattr(dpy.Message, "edit", _message_edit)
    setattr(dpy.Message, "__repr__", lambda s: f"<Message id={s.id} channel={s.channel!r} author={s.author!r}>")
    setattr(dpy.abc.Messageable, "send", _send_message)
    setattr(dpy.Client, "run", _run_bot)
    setattr(dpy.Client, "event_on_close", _event_on_close)
    setattr(_commands.bot.BotBase, "run_command", _run_command)
    setattr(_commands.Context, "error_message", error_message)
    setattr(_commands.Context, "embed", _send_embed)
    setattr(_commands.Context, "send_image", _send_image)
    setattr(_commands.Context, "success_embed", _success_embed)
    setattr(dpy.state.ConnectionState, "parse_message_create", _parse_message_create)
    setattr(dpy.state.ConnectionState, "store_emoji", _store_emoji)
    setattr(dpy.state.ConnectionState, "store_user", _store_user)
    setattr(dpy.state.ConnectionState, "_remove_guild", _remove_guild)
    setattr(dpy.state.ConnectionState, "parse_guild_emojis_update", _parse_guild_emojis_update)
    setattr(dpy.http.HTTPClient, "send_message", _raw_send_message)
    setattr(dpy.http.HTTPClient, "send_files", _raw_send_files)
    setattr(dpy.Embed, "add_useless_stuff", _embed_add_useless_stuff)
    
    del _send_message, _raw_send_message, _raw_send_files, _message_edit, _message_init, _run_bot, _event_on_close, _run_command, _send_embed, _send_image, _success_embed, _parse_message_create, _store_emoji, _store_user, _remove_guild, _parse_guild_emojis_update, _embed_add_useless_stuff
    _collect()

def initiate(client, db_name: str = "username601"): # no stop calling me yanderedev 2.0 (because i am)
    client.slot = Slot
    client.oreo = Oreo
    client.Blur = Blur
    client.lego = legofy
    client.Embed = embed
    client.Parser = Parser
    client.Trivia = Trivia
    client.GDLevel = GDLevel
    client.Hangman = Hangman
    client.Panel = CustomPanel
    client.MathQuiz = MathQuiz
    client.UserCard = UserCard
    client.TicTacToe = TicTacToe
    client.ServerCard = ServerCard
    client.rps = RockPaperScissors
    client.GeoQuiz = GeographyQuiz
    client.ProfileCard = ProfileCard
    client.GuessAvatar = GuessAvatar
    client.ChooseEmbed = ChooseEmbed
    client.EmbedPaginator = Paginator
    client.Image = ImageClient(client)
    client.GuessTheFlag = GuessTheFlag
    client.GuessMyNumber = GuessMyNumber
    client.ColorThief = Smart_ColorThief
    client.WaitForMessage = WaitForMessage
    client.db = Database(getenv("DB_LINK"), db_name)
    
    credentials = client.db.get('config', {'h': True}).get('spotify_credentials')
    client.spotify = SpotifyAPI(client, getenv("SPOTIFY_CREDENTIALS"), token=credentials['token'] if credentials else None, token_expiry_date=credentials['expiry_date'] if credentials else None)
    Util(client)