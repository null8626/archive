const Bot = require('discord-bot-lib');
const bot = new Bot({
    token: "Discord Token",
    prefix: "?"
});

/* 
   This requires you to cache guilds
   which sadly, this lib doesn't do it for you
   so you have to do it on your own :^)
*/

// create a cache object
let guildsCache = {};

// add guild to cache
bot.event('guildCreate', guild => {
    guildsCache[guild.id] = guild;
});

// modify cache on guild update
bot.event('guildEdit', guild => {
    const g = guildsCache[guild.id];
    if (!g) return;
    guildsCache[guild.id] = Object.assign(guild, g);
});

// delete guild if the bot is kicked or a guild is deleted
bot.event('guildDelete', guild => {
    if (!guildsCache[guild.id]) return;
    delete guildsCache[guild.id];
});

// get guild from cache using a role ID
function getGuildFromRoleID(roleID) {
    for (const guild of Object.values(guildsCache))
        if (guild.roles.filter(x => x.id == roleID).length)
            return guild;
}

// add a role to cache (if exists)
bot.event('guildRoleCreate', role => {
    const guild = getGuildFromRoleID(role.id);
    if (!guild) return;
    guild.roles.push(role);
    guildsCache[guild.id] = guild;
});

// update role in cache (if exists)
bot.event('guildRoleUpdate', updatedRole => {
    const guild = getGuildFromRoleID(updatedRole.id);
    if (!guild) return;
    for (role of guild.roles) {
        if (role.id !== updatedRole.id) continue;
        guild.roles[guild.roles.indexOf(role)] = Object.assign(updatedRole, role);
    }
    guildsCache[guild.id] = guild;
});

// remove role from cache (if exists)
bot.event('guildRoleDelete', role => {
    const guild = getGuildFromRoleID(role.id);
    if (!guild) return;
    guild.roles = guild.roles.filter(x => x.id !== role.id);
    guildsCache[guild.id] = guild;
});

// add a member to cache (if exists)
bot.event('guildMemberAdd', member => {
    const guild = guildsCache[member.guild_id];
    if (!guild) return;
    guild.members.push(member);
    guild.member_count += 1;
    guildsCache[guild.id] = guild;
});

// update member in cache (if exists)
bot.event('guildMemberUpdate', updatedMember => {
    const guild = guildsCache[updatedMember.guild_id];
    if (!guild) return;
    for (member of guild.members) {
        if (member.id !== updatedMember.id) continue;
        guild.members[guild.members.indexOf(member)] = Object.assign(updatedMember, member);
    }
    guildsCache[guild.id] = guild;
});

// remove member from cache (if exists)
bot.event('guildMemberRemove', member => {
    const guild = guildsCache[member.guild_id];
    if (!guild) return;
    guild.members = guild.members.filter(x => x.id !== member.id);
    guildsCache[guild.id] = guild;
});

// update emoji in cache (if exists)
bot.event('guildEmojisUpdate', emojis => {
    const guild = guildsCache[emojis.guild_id];
    if (!guild) return;
    guild.emojis = emojis.emojis;
    guildsCache[emojis.guild_id] = guild;
});

// get a guild from the cache from a channel ID
function getGuildFromChannelID(channelID) {
    for (const guild of Object.values(guildsCache))
        if (guild.channels.filter(x => x.id === channelID).length)
            return guild;
}

bot.command({
    name: "serverinfo",
    run: async (ctx) => {
        const guild = getGuildFromChannelID(ctx.raw().channel_id);
        if (!guild) return await ctx.send('This guild is not stored in our cache. Please try again later.');
        
        return await ctx.embed({
            title: `Server info for ${guild.name}`,
            thumbnail: guild.icon ? `https://cdn.discordapp.com/icons/${guild.id}/${guild.icon}.${guild.icon.startsWith('a_') ? 'gif' : 'png'}` : null,
            fields: {
                "Member Count": `${guild.member_count}`,
                "Guild Roles": guild.roles.map(role => `<@&${role.id}>`).join(' ').slice(0, 1024) || 'No roles',
                "Guild Emojis": guild.emojis.map(emoji => `<${emoji.animated ? 'a' : ''}:${emoji.name}:${emoji.id}>`).join(' ').slice(0, 1024) || 'No emojis'
            },
            color: 0x66FFC8
        });
    }
});

bot.run();
