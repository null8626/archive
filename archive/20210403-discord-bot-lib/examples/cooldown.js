const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?",
    onCooldown: async (ctx, retryAfter) => {
        return await ctx.send(`You are on cooldown. Try again in ${retryAfter} seconds.`);
    }
});

bot.command({
    name: "avatar",
    cooldown: 5,
    run: async (ctx) => {
        return await ctx.send(ctx.avatar());
    }
});

// this one is a sample, DON'T directly copy-and-paste it to your code.
bot.command({
    name: "daily",
    cooldown: 86400,
    onCooldown: async (ctx, retryAfter) => {
        // this is a bad format; don't tell the users in seconds.
        return await ctx.send(`I am not made out of money. You can get daily again in ${retryAfter} seconds.`);
    },
    run: async (ctx) => {
        // do stuff with db here...
        return await ctx.send(`You claimed your daily for ${reward} credits!`);
    }
});

bot.run();