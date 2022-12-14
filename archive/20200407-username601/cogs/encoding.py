import discord
from discord.ext import commands
from decorators import *
from io import BytesIO
from urllib.parse import quote
from base64 import b64encode
from json import loads

class encoding(commands.Cog):
    def __init__(self):
        self.ciphers = loads(open("./assets/json/encode.json", "r").read())
        pass
    
    @command(["jumble"])
    @cooldown(2)
    @require_args()
    async def shuffle(self, ctx, *args):
        return await ctx.reply(ctx.bot.util.shuffle(" ".join(args)))
    
    @command(["morse-code"])
    @cooldown(5)
    @require_args()
    async def morse(self, ctx, *args):
        total = ""
        for char in " ".join(args).lower():
            total += " " + self.ciphers.get(char, { "morse": char })["morse"]
        return await ctx.reply(total[1:])
    
    @command(["blind"])
    @cooldown(5)
    @require_args()
    async def braille(self, ctx, *args):
        total = ""
        for char in " ".join(args).lower():
            total += self.ciphers.get(char, { "braille": char })["braille"]
        return await ctx.reply(total)
    
    @command(["curve", "curve-text"])
    @cooldown(5)
    @require_args()
    async def cursive(self, ctx, *args):
        total = ""
        for char in " ".join(args).lower():
            total += self.ciphers.get(char, { "cursive": char })["cursive"]
        return await ctx.reply(total)
    
    @command(["fancy-text"])
    @cooldown(5)
    @require_args()
    async def fancy(self, ctx, *args):
        total = ""
        for char in " ".join(args).lower():
            total += self.ciphers.get(char, { "fancy": char })["fancy"]
        return await ctx.reply(total)
    
    @command(["upside-down", "upsidedown", "flip-text", "textflip"])
    @cooldown(5)
    @require_args()
    async def fliptext(self, ctx, *args):
        total = ""
        for char in " ".join(args).lower():
            total += self.ciphers.get(char, { "upside-down": char })["upside-down"]
        return await ctx.reply(total)
    
    @command()
    @cooldown(4)
    @require_args()
    @permissions(bot=['attach_files'])
    async def ascii(self, ctx, *args):
        await ctx.trigger_typing()
        parser = ctx.bot.Parser(args)
        parser.parse(('hastebin',))
        
        if (not parser) or (not parser.has("image")):
            if not parser.other:
                return await ctx.bot.cmds.invalid_args(ctx)
            
            ascii = await ctx.bot.util.request(
                "http://artii.herokuapp.com/make",
                text=' '.join(parser.other)
            )
            if parser.has("hastebin"):
                try:
                    response = await ctx.bot.http._HTTPClient__session.post("https://paste.mod.gg/documents", data=ascii)
                    assert response.status < 400
                    json = await response.json()
                    await ctx.success_embed(description=f"[**Click here to see the asciified text.**](https://paste.mod.gg/{json['key']})")
                    del ascii, image, parser, json
                    return
                except AssertionError:
                    pass
            
            await ctx.reply(f'```{ascii[:2000]}```')
            del ascii, parser
            return
        
        parser.shift("image")
        image = await ctx.bot.Parser.parse_image(ctx, parser.other)
        string = await ctx.bot.Image.asciify(image)
        if hastebin:
            try:
                response = await ctx.bot.http._HTTPClient__session.post("https://paste.mod.gg/documents", data=string)
                assert response.status < 400
                json = await response.json()
                await ctx.success_embed(description=f"[**Click here to see the asciified image.**](https://paste.mod.gg/{json['key']})")
                del string, image, parser, hastebin, json
                return
            except AssertionError:
                pass
        
        await ctx.bot.http.send_files(ctx.channel.id, content="", files=[discord.File(BytesIO(bytes(string, 'utf-8')), "asciified.txt")])
        del string, image, parser, hastebin

    @command()
    @cooldown(2)
    @permissions(bot=['attach_files'])
    @require_args()
    async def barcode(self, ctx, *args):
        await ctx.trigger_typing()
        return await ctx.send_image('http://www.barcode-generator.org/zint/api.php?bc_number=20&bc_data=' + quote(' '.join(args))[:75])
    
    @command(['qrcode', 'qr-code'])
    @cooldown(2)
    @permissions(bot=['attach_files'])
    @require_args()
    async def qr(self, ctx, *args):
        await ctx.trigger_typing()
        return await ctx.send_image('https://api.qrserver.com/v1/create-qr-code/?size=150x150&data=' + quote(' '.join(args))[:75])

    @command()
    @cooldown(2)
    @require_args()
    async def binary(self, ctx, *args):
        return await ctx.reply('```'+''.join(map(lambda x: f"{ord(x):08b}", ' '.join(args)))[:2000]+'```')

    @command()
    @cooldown(2)
    @require_args(2)
    async def caesar(self, ctx, *args):
        offset = ctx.bot.Parser.get_numbers(args)
        if not offset:
            return await ctx.bot.cmds.invalid_args(ctx)
        return await ctx.reply(ctx.bot.util.caesar(str(' '.join(args).replace(str(offset[0]), '')), offset[0]))
    
    @command()
    @cooldown(2)
    @require_args()
    async def atbash(self, ctx, *args):
        return await ctx.reply(ctx.bot.util.atbash(' '.join(args)))

    @command()
    @cooldown(2)
    @require_args()
    async def reverse(self, ctx, *args):
        return await ctx.reply(' '.join(args)[::-1])
    
    @command(['b64'])
    @cooldown(2)
    @require_args()
    async def base64(self, ctx, *args):
        return await ctx.reply(b64encode(' '.join(args).encode('ascii')).decode('ascii'))
    
def setup(client):
    client.add_cog(encoding())