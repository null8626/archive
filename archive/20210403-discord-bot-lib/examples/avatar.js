const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?"
});

/*
Avatar URLs are by default .png (or .gif if the author has animated avatar) with the size of 4096 (maximum).

You can change the values to something else, example:
const avatarURL = ctx.avatar({ format: 'jpeg', size: 128 });
*/

bot.command({
    name: "avatar",
    run: async (ctx) => {
        return await ctx.send(ctx.avatar());
    }
});

bot.run();