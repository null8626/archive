# IMPORT EVERY SINGLE MODULE FROM PYTHON FIRST
from aiohttp import ClientSession, ClientTimeout
from .xmltodict import parse as xmltodict
from discord.ext.commands import Context
from discord import __version__ as _ver
from configparser import ConfigParser
from os import getenv, name, listdir
from random import choice, shuffle
from gc import collect as _collect
from discord.ext import commands
from traceback import format_exc
from subprocess import run, PIPE
from urllib.parse import quote
from datetime import datetime
from inspect import getsource
from base64 import b64encode
from re import sub as _sub
from json import loads
from io import BytesIO
from discord import *
from time import time
from enum import Enum
from PIL import Image

class SpotifyAPI:
    """ Much better than spotipy pypi module. This one uses aiohttp instead of the stinky requests module. """
    
    MAX_CACHE_SIZE = 100
    API_VERSION = 1
    __slots__ = ('bot', '_session', '_credentials', '_token', '_token_expiry_date')

    def __init__(self, bot, credentials, token=None, token_expiry_date=None):
        self.bot = bot
        self._session = None
        self._credentials = b64encode(credentials.encode()).decode()
        
        self._token = token
        self._token_expiry_date = token_expiry_date
    
    async def get_token(self):
        if (not self._token) or (time() >= self._token_expiry_date):
            response = await self._session.post("https://accounts.spotify.com/api/token", data={'grant_type': 'client_credentials'}, headers={'Authorization': f'Basic {self._credentials}'})
            response = await response.json()
            
            self._token = response['access_token']
            self._token_expiry_date = time() + response['expires_in']
        return self._token
    
    async def request(self, path, **kwargs):
        if not self._session:
            self._session = self.bot.http._HTTPClient__session
            self.bot = None
        
        path = f'https://api.spotify.com/v{SpotifyAPI.API_VERSION}' + path
        
        if kwargs:
            path += ( '?' + '&'.join(f'{a}={quote(str(b))}' for a, b in kwargs.items()) )
        
        token = await self.get_token()
        response = await self._session.get(path, headers={'Authorization': f'Bearer {token}'})
        
        if response.status >= 400:
            return
        
        return await response.json()

class LengthFormats(Enum):
    KILOMETERS  = (("km", "kilometer", "kilometre", "kilometers", "kilometres"), (
        None, "*1000", "*100000", "*1000000", "/1.609", "*1094", "*3281", "*39370"
    ))
    METERS      = (("m", "meter", "metre", "meters", "metres"), (
        "/1000", None, "*100", "*1000", "/1609", "*1.094", "*3.281", "*39.37"
    ))
    CENTIMETERS = (("cm", "centimeter", "centimetre", "centimeters", "centimetres"), (
        "/100000", "/100", None, "*10", "/160934", "/91.44", "/30.48", "/2.54"
    ))
    MILLIMETERS = (("mm", "millimeter", "millimetre", "millimeters", "millimetres"), (
        "/1000000", "/1000", "/10", None, "/1609000", "/914", "/305", "/25.4"
    ))
    MILES       = (("mile", "mi.", "miles", "mi"), (
        "*1.609", "*1609", "*160934", "*1609000", None, "*1760", "*5280", "*63360"
    ))
    YARDS       = (("yard", "yards", "yd"), (
        "/1094", "/1.094", "*91.44", "*914", "/1760", None, "*3", "*36"
    ))
    FOOT        = (("feet", "foot", "ft", "ft."), (
        "/3281", "/3.281", "*30.48", "*305", "/5280", "/3", None, "*12"
    ))
    INCHES      = (("inch", "inches"), (
        "/39370", "/39.37", "*2.54", "*25.4", "/63360", "/36", "/12", None
    ))
    
    _ALL = ("KILOMETERS", "METERS", "CENTIMETERS", "MILLIMETERS", "MILES", "YARDS", "FOOT", "INCHES")

class error_message(Exception):
    def __init__(self, message: str, description: bool = False):
        self.use_description = description or (len(message) > 256)
        self.embed = Embed(title=None if self.use_description else message, description=message if self.use_description else None, color=Color.red())
        super().__init__(message)

class Util:

    def __init__(
        self,
        client,
        attribute_name: str = "util",
        config_file: str = "config.ini"
    ):
        """
        Bot Utilities. That's all.
        
        Upon initiation, this class will make a copy of itself to the discord.Client object.
        """
        self.bot = client
        self._alphabet = list('abcdefghijklmnopqrstuvwxyz')
        self._start = time()
        self.alex_client = ClientSession(headers={'Authorization': getenv("ALEXFLIPNOTE_TOKEN")}, timeout=ClientTimeout(total=10.0))
        self.github_client = ClientSession(headers={'Authorization': 'token ' + getenv('GITHUB_TOKEN')}, timeout=ClientTimeout(total=10.0))
        
        self._time = {
            31536000: "year",
            2592000: "month",
            86400: "day",
            3600: "hour",
            60: "minute"
        }
        
        self._config = ConfigParser()
        self._config.read(config_file)
        
        for key in dict(self._config["bot"]).keys():
            setattr(self, key, int(self._config["bot"][key]) if self._config["bot"][key].isnumeric() else self._config["bot"][key])
            
        self._8ball_template = (
            "As I see it, {}",
            "My reply is {}",
            "My sources say {}",
            "{}",
            "Of course {}",
            "Well, {}. Of course",
            "{}, definitely",
            "Signs point to {}",
            "{}. Without a doubt",
            "Hell {}",
            "Well... {}",
            "Why did you ask me for this. The answer is always {}",
            "The answer is always {}",
            "Shut up. The answer is {}",
            "Stop asking me that question. The answer is definitely {}",
            "Heck {}",
            "That question's answer is always {}",
            "{}. {}!!!",
            "Someone told me the answer is {}",
            "Sorry, but the answer is {}"
        )
        
        del self._config
        
        setattr(client, attribute_name, self)

    def mobile_indicator(self) -> None:
        """ Turns your bot to a bot with mobile status. Source from this gist: https://gist.github.com/norinorin/0ef021163d042b3be76b892726d76e52 """
        
        s = __import__("inspect").getsource(gateway.DiscordWebSocket.identify).split('\n')
        indent = len(s[0]) - len(s[0].lstrip())
        source_ = '\n'.join(i[indent:] for i in s)

        source_ = __import__("re").sub(r'([\'"]\$browser[\'"]:\s?[\'"]).+([\'"])', r'\1Discord Android\2', source_)  # hh this regex
        m = __import__("ast").parse(source_)
        
        loc = {}
        exec(compile(m, '<string>', 'exec'), gateway.__dict__, loc)
        
        gateway.DiscordWebSocket.identify = loc['identify']
        del m, loc, source_, s, indent
        _collect()

    def timestamp(self, input, time_data: str = None, include_time_past: bool = True) -> str:
        """ Formats a timestamp. """
        if isinstance(input, str):
            input = datetime.strptime(input.split(".")[0].rstrip("Z"), "%Y-%m-%dT%H:%M:%S" if input.count("T") else time_data)
        
        _past = f"({self.strfsecond(time() - input.timestamp())} ago)" if include_time_past else ""
        return f'{input.strftime("%A, %d %B %Y" if input.minute == input.hour else "%A, %d %B %Y at %H:%M:%S")} {_past}'

    def convert_length(self, string):
        """ Does a math like `10 meters to kilometers` """
        try:
            string = string.lower().replace(" ", "")
            formats = _sub(r"\d+", "", string)
            number = int(string.replace(formats, "").strip(",."))
            first_format, second_format = formats.split("to")
            assert first_format != second_format
            assert (-999999 < number < 999999999)
            
            for format in LengthFormats._ALL.value:
                if first_format in getattr(LengthFormats, format).value[0]:
                    first_format = getattr(LengthFormats, format).value[1]
                elif second_format in getattr(LengthFormats, format).value[0]:
                    second_format = format
            
            return f'{eval(f"{number}{first_format[LengthFormats._ALL.value.index(second_format)]}")} {second_format.lower()}'
        except:
            raise error_message("Unsupported or invalid calculation.")
    
    def has_nitro(self, guild, member) -> bool:
        return (((member.is_avatar_animated()) or (member in guild.premium_subscribers)) and (not member.bot))
    
    def join_position(self, guild, member) -> int:
        sorted_array = sorted([i.joined_at.timestamp() for i in guild.members])
        res = sorted_array.index(member.joined_at.timestamp())
        del sorted_array, guild, member
        return res + 1
   
    def eight_ball(self, ctx) -> str:
        """ Gets the eight ball answer. """
        code = hash(ctx.message.content.lower().replace(" ", "").replace("?", ""))
        if code < 0: code *= -1
        
        response = ((code + ctx.author.id) % 2 == 0)
        del code, ctx
        return choice(self._8ball_template).format("yes" if response else "no")
    
    async def handle_error(self, ctx, error, raw):
        """ Handles errors like a boss. """
        error = getattr(error, "original", error)
        if isinstance(error, commands.CommandNotFound) or isinstance(error, commands.CheckFailure): return
        elif isinstance(error, commands.CommandOnCooldown): return await ctx.reply("Calm down. Try again in {}.".format(self.strfsecond(round(error.retry_after))), delete_after=2)
        elif isinstance(error, error_message):
            await ctx.reply(embed=error.embed)
            del error
            return
        elif isinstance(error, errors.Forbidden): 
            try: return await ctx.reply("I don't have the permission required to use that command!")
            except: return
        await self.bot.get_user(self.owner_id).send(embed=Embed(
            title='Error', color=Color.red(), description=f'Content:\n```{ctx.message.content}```\n\nError:\n```{raw.split("The above exception was the direct cause of")[0]}```'
        ).set_footer(text='Bug made by user: {} (ID of {})'.format(str(ctx.author), ctx.author.id)))
        return await ctx.reply('Sorry, there was an error while executing this command.\nThis message has been reported to the developer of the bot.', delete_after=3)
    
    def load_cogs(self, cog_folder: str = None, exclude: list = []):
        """ Loads the cogs from a directory. """
        
        _cog_folder = getattr(self, "cogs_dirname", cog_folder)
        
        for i in listdir(_cog_folder):
            if not i.endswith(".py") or i in exclude: continue
            try:
                print("Loading cog", i)
                self.bot.load_extension(f'{_cog_folder}.{i[:-3]}')
            except Exception as e:
                print("Error while loading cog:", format_exc())

    def _crop_out_memegen(self, ctx, _bytes: bytes) -> BytesIO:
        """ idk if this is illegal but anyway """
        image = Image.open(BytesIO(_bytes))
        return ctx.bot.Image.save(image.crop((0, 12, image.width, image.height - 12)))

    async def request(self, url, json=False, xml=False, alexflipnote=False, github=False, **kwargs):
        """ Does a GET request to a specific URL with a query parameters."""

        query_param = "?" + "&".join([i + "=" + quote(str(kwargs[i])).replace("+", "%20") for i in kwargs.keys()]) if kwargs else ""
        
        try:
            session = self.alex_client if alexflipnote else (
                self.github_client if github else self.bot.http._HTTPClient__session
            )
            result = await session.get(url + query_param)
            assert result.status < 400, f"API returns a non-OK status code: {result.status}"
            if json:
                try:
                    return await result.json()
                except:
                    return loads(await result.read())
            elif xml:
                return xmltodict(await result.text())
            return await result.text()
        except Exception as e:
            raise error_message("Request Failed. Exception: " + str(e))

    def strfsecond(self, seconds: int):
        """ Converts a second to a string """
        seconds = int(seconds)
        
        if seconds < 60:
            return f"{seconds} second" + ("" if (seconds == 1) else "s")
        
        for key in self._time.keys():
            if seconds >= key:
                seconds //= key
                return f"{seconds} {self._time[key]}" + ("" if seconds == 1 else "s")
        
        seconds //= 31536000
        return f"{seconds} year" + ("" if seconds == 1 else "s")
    
    async def execute(self, command: str) -> str:
        """ Evaluates a terminal command and returns an output. """
        
        return run(command.split(), stdout=PIPE).stdout.decode('utf-8')
    
    def atbash(self, text: str):
        """ Encodes a text to atbash cipher. """
        
        result = ""
        for char in text:
            if char.isalpha():
                result += self._alphabet[::-1][self._alphabet.index(char.lower())]
            else:
                result += char
        return result
    
    def caesar(self, text: str, offset: int):
        """Encodes a text as caesar cipher."""
        
        if offset > 26:
            while offset > 26:
                offset -= 26
        elif offset < 0:
            while offset < 0:
                offset += 26
        
        result = ""
        for char in text:
            if char.isalpha():
                index = self._alphabet.index(char.lower()) + offset
                
                if index > 25:
                    result += self._alphabet[index - 26]
                elif index < 0:
                    result += self._alphabet[index + 26]
                else:
                    result += self._alphabet[index]
            else:
                result += char
        return result

    def shuffle(self, text: str) -> str:
        """ Shuffles a text. """
        l = list(text)
        shuffle(l)
        return "".join(choice((l, l[::-1])))