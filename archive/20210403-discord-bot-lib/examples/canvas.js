const { createCanvas } = require('canvas');
const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?"
});

bot.command({
    name: "canvas",
    run: async (ctx) => {
        const canvas = createCanvas(200, 200);
        const canvasContext = canvas.getContext('2d');
        canvasContext.font = '30px Impact';
        canvasContext.fillText('discord.js\nsucks', 50, 100);

        return await ctx.file(canvas.toBuffer('image/png'), 'image.png');
    }
});

bot.run();