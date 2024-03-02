const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?",
    onError: async (ctx, error) => {
        // this type of error handling is bad. please DON'T copy this.
        if (!(error instanceof Bot.HTTPException)) return console.log(error.message);

        console.log('An API error has occurred.');
        console.log(error.statusCode, '\n', error.response);
    }
});

// this is NOT recommended. This will OVERWRITE how the lib handles ratelimits.
bot.onHTTPResponse(429, data => {
    console.log(`The bot is ratelimited. Response: ${data}`)
});

bot.command({
    name: "ping",
    run: async (ctx) => {
        return await ctx.send('Pong.');
    }
});

bot.run();