# discord-bot-lib
A basic discord bot lib for node.js but the library doesn't cache discord objects.<br>
[Examples](https://github.com/null8626/discord-bot-lib/tree/main/examples)<br>

### Key points
- Simple command handler.
- This lib does NOT cache external discord objects.
- This lib does NOT support voice (sorry music bots)
- This lib gives you full control on how you cache discord data.
- This lib gives you full control on how you handle discord gateway events.
- The codebase is only made up of three `js` files.
- Really customizable.

### Installation
```sh
$ npm install discord-bot-lib
```

### Simple Bot Example
```js
// require the lib
const Client = require('discord-bot-lib');

// create the client
const bot = new Client({
    token: "Discord Token",
    prefix: "/"
});

// this is a tradition among discord bots. and you MUST also do it.
bot.event('ready', data => {
    console.log(`The bot is ready! Serving ${data.guilds.length} guilds.`);
});

// simple error handling
bot.error(async (ctx, err) => {
    console.log(`Oh no i got an error: "${err.message}" while running ${ctx.command.name} command`);
});

// simple command handler example
bot.command({
    name: "ping",
    aliases: ["pong"],
    run: async (ctx) => {
        return await ctx.send('Pong');
    }
});

// run the bot
bot.run();
```
