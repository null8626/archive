from discord.ext import commands
from discord import Embed, Color

def command(aliases: list = None):
    if not aliases:
        return commands.command(pass_context=True)
    return commands.command(pass_context=True, aliases=aliases)

def cooldown(amount: int, channel_wide: bool = False, server_wide: bool = False):
    return commands.cooldown(1, amount, getattr(commands.BucketType, "guild" if server_wide else ("channel" if channel_wide else "user")))

def require_args(count: int = 1, name: str = None):
    async def predicate(ctx):
        if len(ctx.message.content.split()[1:]) < count:
            await ctx.bot.cmds.invalid_args(ctx, name=name)
            return False
        return True
    return commands.check(predicate)

def owner_only():
    async def predicate(ctx):
        if ctx.author.id != ctx.bot.util.owner_id:
            await ctx.send(embed=Embed(title=f"This command is reserved only for the developer.", color=Color.red()))
            return False
        return True
    return commands.check(predicate)

def permissions(author: list = None, bot: list = None):
    async def predicate(ctx):
        if author:
            author_permissions = ctx.channel.permissions_for(ctx.author)
            for perms in author:
                if not getattr(author_permissions, perms):
                    await ctx.send(embed=Embed(description=f"You are missing the `{', '.join(author).replace('_', '')}` permissions. Which is required to run this command.", color=Color.red()))
                    return False
        if bot:
            bot_permissions = ctx.channel.permissions_for(ctx.me)
            for perms in bot:
                if not getattr(bot_permissions, perms):
                    await ctx.send(embed=Embed(description=f"This bot is missing the `{', '.join(bot).replace('_', '')}` permissions. Which is required to run this command.", color=Color.red()))
                    return False
        return True
    return commands.check(predicate)

def require_profile(set_profile=False):
    async def predicate(ctx):
        data = ctx.bot.db.get("economy", {"userid": ctx.author.id})
        if set_profile:
            setattr(ctx, "profile", data or {
                "userid": ctx.author.id,
                "lastDaily": None,
                "bal": 0,
                "bankbal": 0
            })
        
        if data:
            return True
        
        ctx.bot.db.add("economy", {
            "userid": ctx.author.id,
            "lastDaily": None,
            "bal": 0,
            "bankbal": 0
        })
        return True
    return commands.check(predicate)