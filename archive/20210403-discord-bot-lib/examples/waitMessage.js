const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?"
});

// waiting message response
bot.command({
    name: "test-wait",
    run: async (ctx) => {
        await ctx.send(`Are you sure you want to exist? I'll be waiting for response in 5 seconds.`);
        
        // wait for a response, check function and a milliseconds timeout
        const response = await ctx.wait(message => [`y`, `n`, `yes`, `no`].includes(message.content.toLowerCase()), 5000);
        
        // response is null if the person does not response in 5 seconds (or timed out)
        if (!response) return await ctx.send(`You didn't send anything in 5 seconds. I am angry right now.`);
        
        // process the response
        return await ctx.send(response.content.toLowerCase().includes('y') ? 'What? no!' : 'Understandable, have a great day.');
    }
});

bot.run();