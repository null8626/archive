const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?"
});

bot.command({
    name: "embed",
    noArgs: async (ctx) => {
        return await ctx.send("Please add a text.");
    },
    run: async (ctx) => {
        return await ctx.embed({
            description: ctx.args.join(' '),
            color: 0xFF0000,
            thumbnail: ctx.avatar()
        });
    }
});

bot.run();