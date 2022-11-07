const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?"
});

// async function for delaying in a specific amount of milliseconds
function timeout(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

// editing message content
bot.command({
    name: "test-edit",
    run: async (ctx) => {
        // send the initial message
        const msg = await ctx.send('Message');
        
        // delay for 2 seconds
        await timeout(2000);
        
        // edit the message content
        await msg.edit('Edited message');
    }
});

// editing message embed
bot.command({
    name: "test-edit-embed",
    run: async (ctx) => {
        // send the initial embed
        const msg = await ctx.embed({
            title: "Embed 1",
            color: 0xFF0000
        });
        
        // delay for 2 seconds
        await timeout(2000);
        
        // edit the embed
        await msg.edit({
            embed: {
                title: "Embed 2",
                color: 0x00FF00
            }
        });
    }
});

bot.run();