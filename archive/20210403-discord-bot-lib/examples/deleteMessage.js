const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?"
});

// async function for delaying in a specific amount of milliseconds
function timeout(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

// deleting author's message
bot.command({
    name: "hidden-say",
    noArgs: async (ctx) => {
        return await ctx.send("Please add a text.");
    },
    run: async (ctx) => {
        // this requires manage message perms
        await ctx.delete();
        
        // send a message
        await ctx.send(ctx.args.join(' '));
    }
});

// deleting a sent message
bot.command({
    name: "countdown",
    run: async (ctx) => {
        // store the sent message to 'msg'
        const msg = await ctx.send('This message will disappear in 3 seconds.');
        
        // wait for 3 seconds
        await timeout(3000);
        
        // delete the sent message
        await msg.delete();
    }
});

bot.run();