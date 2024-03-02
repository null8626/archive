const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?"
});

/*
 This one uses a user object. (which stores a SMALL portion of the author's info)
 if you want to store member info, you MUST cache members first.
 a good example of this is found within the serverinfo.js.
 */
bot.command({
    name: "userinfo",
    description: "Sends a partial user info.",
    run: async (ctx) => {
        const rawAuthor = ctx.raw().author;
        const avatarURL = ctx.avatar();

        return await ctx.embed({
            title: `User info for ${ctx.name()}`,
            fields: {
                discriminator: rawAuthor.discriminator,
                Bot: rawAuthor.bot ? `Yes` : `No`,
                "Avatar URL": `[Click Here](${avatarURL})`
            },
            thumbnail: avatarURL
        });
    }
});

bot.run();