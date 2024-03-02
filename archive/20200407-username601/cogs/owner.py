import discord
import sys
import os
import gc
from re import match
from time import time
from traceback import format_exc
from random import randint
from decorators import *
from discord.ext import commands
from datetime import datetime as t
from subprocess import run, PIPE
from inspect import isawaitable, getsource
from asyncio import sleep
totallyrealtoken = 'Ng5NDU4MjY5NTI2Mjk0MTc1.AkxrpC.MyB2BEHJLXuZ8h0wY0Qro6Pwi8'

class owner(commands.Cog):
    def __init__(self, client):
        self.db = client.db
    
    @command()
    @owner_only()
    async def leave(self, ctx, *args):
        server_id = int(''.join(args))
        if (server := ctx.bot.get_guild(server_id)):
            await ctx.embed(title=f"Successfully left a server. ({len(ctx.bot.guilds):,} => {(len(ctx.bot.guilds) - 1):,})", fields={
                "Server Name": server.name,
                "Server Members": f"{server.member_count:,}",
            }, color=(255, 0, 0))
            return await server.leave()
        raise ctx.error_message('That server does not exist, nerd.')
    
    def resolve_variable(self, variable):
        if hasattr(variable, "__iter__"):
            var_length = len(list(variable))
            if (var_length > 100) and (not isinstance(variable, str)):
                return f"<a {type(variable).__name__} iterable with more than 100 values ({var_length})>"
            elif (not var_length):
                return f"<an empty {type(variable).__name__} iterable>"
        
        if (not variable) and (not isinstance(variable, bool)):
            return f"<an empty {type(variable).__name__} object>"
        return (variable if (len(f"{variable}") <= 1000) else f"<a long {type(variable).__name__} object with the length of {len(f'{variable}'):,}>")
    
    def prepare(self, string):
        string = string.strip("```").replace("py", "").replace("thon", "").strip("\n")
        if "\n" not in string:
            return f'\n\t{"" if string.strip(" ").startswith("return") else "return"} {string}'
        l = string.split("\n")
        if not l[-1].strip(" ").startswith("return"):
            l[-1] = "return " + l[-1]
        return "\n\t" + "\n\t".join(l)
    
    @command(["eval"])
    @cooldown(1)
    async def code(self, ctx, *args):
        code = self.prepare(ctx.message.content[(len(ctx.prefix) + 5):])
        
        if ctx.author.id != ctx.bot.util.owner_id:
            code = ctx.message.content[6:].replace('```py', '').strip('`').replace('"', '').replace("'", "")
            if code.startswith("return "): code = code[7:]
            if code.startswith("await "): code = code[6:]
            
            if ("token" in code.lower()) or ("secret" in code.lower()):
                return await ctx.reply(f"```py\n{totallyrealtoken}````str | {randint(0, 10)}ms`")
            
            text = match(r'(.*?)[ \.\+\-\(\)\:\/\*\%]', code)
            if text:
                text = text.group()[:-1].strip(' ')
            elif code:
                text = code.strip('(').strip(')').strip('[').strip(']').strip('+').strip('-').strip('*').strip(':').strip('/').strip('%').strip(' ')
            
            if (not code) or (not text):
                raise ctx.error_message('Please add something to evaluate.')
            return await ctx.reply("```py\nTraceback (most recent call last):\n  File \"/bot/owner.py\", line 69, in eval_command\n    exec(f\"async def func():{code}\", args)\n  File \"<eval>\", line 2\n        return result\n               ^\nNameError: '" + text.replace("'", '"') + "' is not defined```")
        
        args = {
            "discord": discord,
            "sauce": getsource,
            "sys": sys,
            "os": os,
            "imp": __import__,
            "this": self,
            "ctx": ctx
        }
        
        try:
            exec(f"async def func():{code}", args)
            a = time()
            response = await eval("func()", args)
            if (response is None) or isinstance(response, discord.Message):
                del args, code
                return gc.collect()
            
            await ctx.reply(f"```py\n{self.resolve_variable(response)}````{type(response).__name__} | {(time() - a) / 1000} ms`")
        except:
            await ctx.reply(f":x: Error occurred:```py\n{format_exc()}```")
        
        del args, code
        gc.collect()
    
    @command()
    @owner_only()
    async def selfpurge(self, ctx):
        messages = list(filter(lambda x: x.author == ctx.me and x.guild == ctx.guild, ctx.bot.cached_messages))
        for message in messages[:10]:
            await message.delete()
            await sleep(2)
        del messages
    
    @command(['ann', 'announcement'])
    @cooldown(2)
    @owner_only()
    async def announce(self, ctx, *args):
        self.db.modify("config", self.db.types.APPEND, {"h": True}, {"changelog": "`["+str(t.now())[:-7]+" UTC]` "+" ".join(args)})
        await ctx.reply("OK, added to db!")

    @command(['src'])
    @owner_only()
    async def source(self, ctx, *args):
        try:
            source = eval("getsource({})".format(' '.join(args)))
            return await ctx.reply('```py\n'+str(source)[:1900]+'```')
        except Exception as e:
            raise ctx.error_message(str(e))

    @command()
    @owner_only()
    async def rp(self, ctx, *args):
        try:
            user_to_send = ctx.bot.get_user(int(args[0]))
            em = discord.Embed(title="Hi, "+user_to_send.name+"! the bot owner sent a response for your feedback.", description=' '.join(args[1:]), colour=ctx.me.color)
            await user_to_send.send(embed=em)
            await ctx.message.add_reaction('✅')
        except Exception as e:
            raise ctx.error_message(f'Error: `{str(e)}`')

    @command()
    @owner_only()
    async def fban(self, ctx, *args):
        self.db.modify("config", self.db.types.APPEND, {"h": True}, {"bans": args[0]+"|"+" ".join(args[1:])})
        await ctx.reply('banum\'d')
    
    @command()
    @owner_only()
    async def funban(self, ctx, *args):
        data = self.db.get("config", {"h": True})["bans"]
        self.db.modify("config", self.db.types.CHANGE, {"h": True}, {"bans": [i for i in data if int(args[0]) != int(i.split("|")[0])]})
        await ctx.reply("unbannum'd")

    @command()
    async def token(self, ctx):
        await ctx.reply(totallyrealtoken)

    @command(['sh'])
    async def bash(self, ctx, *args):
        command = " ".join(args)[:100] if args else "echo hello world"
        if ctx.author.id == ctx.bot.util.owner_id:
            try:
                await ctx.trigger_typing()
                data = await ctx.bot.util.execute(command)
                return await ctx.reply(embed=discord.Embed(title='Bash Terminal', description='Input:```sh\n'+command+'```**Output:**```sh\n'+str(data)[:2000]+'```', color=discord.Color.green()))
            except Exception as e:
                return await ctx.reply(embed=discord.Embed(title='Error on execution', description='Input:```sh\n'+command+'```**Error:**```py\n'+str(e)+'```', color=discord.Color.red()))
        await ctx.reply(embed=discord.Embed(title='Error on execution', description='Input:```sh\n'+command+'```**Error:**```py\nDenied by sudo.sh```', color=discord.Color.red()).set_footer(text='It is because it is owner only you dumbass'))

def setup(client):
    client.add_cog(owner(client))