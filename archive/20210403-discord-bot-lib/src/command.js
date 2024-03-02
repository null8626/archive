const { parseEmbedObject } = require('./util');

class Command {
    /**
     * A basic bot command.
     * @param {Object} [params] The parameters.
     * @param {string} [params.name] The command name.
     * @param {string[]} [params.aliases] The command aliases. 
     * @param {function} [params.run] The command run function.
     * @param {function} [params.onCooldown] The function to be called whenever the command hits a cooldown.
     * @param {function} [params.noArgs] Function to call whenever the user doesn't add arguments (at all).
     * @param {number} [params.cooldown] The command cooldown in seconds.
     */
    constructor(params) {
        if (!params.name || typeof params.name !== 'string') throw new TypeError('A command MUST have a name.');

        this.name = params.name.toLowerCase();
        this.aliases = (params.aliases && params.aliases.length && params.aliases.every(s => typeof s === 'string')) ? params.aliases.map(s => s.toLowerCase()) : [];
        this.run = params.run || (async (x) => undefined);
        this.noArgs = typeof params.noArgs === 'function' ? params.noArgs : null;
        this.onCooldown = typeof params.onCooldown === 'function' ? params.onCooldown : null;
        this.onError = typeof params.onError === 'function' ? params.onError : null;
        this.cooldown = params.cooldown || 0;
        this.usageCount = 0;
        if (params.cooldown) this.cooldownCache = {};
    }

    resolveCooldown(message) {
        if (!this.cooldown) return;
        
        const currentTime = Math.floor((new Date()).getTime() / 1000);
        const bucket = `${message.channel_id}${message.author.id}`;
        if (this.cooldownCache[bucket]) {
            if (this.cooldownCache[bucket] > currentTime) return this.cooldownCache[bucket] - currentTime;
            delete this.cooldownCache[bucket];
            return;
        }

        this.cooldownCache[bucket] = currentTime + this.cooldown;
    }
}

class Context {
    constructor(client, command, message) {
        this.content = message.content;
        this.command = command;
        let _deleted = false;
        let _sentMessage = false;
        
        /**
         * Retrieves the message author's name.
         * @param {Boolean} withDiscrim Boolean to set if the function returns the author's discrim or not.
         * @returns {string} The author's name.
         */
        this.name = (withDiscrim) => `${message.author.name}${withDiscrim ? '#' + message.author.discriminator : ''}`;

        /**
         * @async
         * Sends a message.
         * @param {string} content The message content to reply with.
         * @returns {Promise<Context>} The message context.
         */
        this.send = (content) => {
            return new Promise((ctx, err) => {
                client.http.request('POST', `/channels/${message.channel_id}/messages`, {
                    content: (content || '_ _').slice(0, 2000),
                    allowed_mentions: {
                        replied_user: false,
                        parse: []
                    },
                    message_reference: _sentMessage ? {} : {
                        message_id: message.id
                    }
                }).then((res, rej) => {
                    if (rej) return err(rej);
                    if (!_sentMessage) _sentMessage = true;
                    return ctx(new Context(client, this.command, res));
                });
            });
        }

        /**
         * @async
         * Sends a file attachment to the chat.
         * @param {Buffer} buffer The file buffer.
         * @param {string} fileName The file name.
         * @param {object} [param] Extra optional parameters.
         * @param {string} [param.content] The message content.
         * @param {object} [param.embed] The message embed object.
         * @param {object} [param.rawJSON] The raw discord message embed object.
         * @returns {Promise<Context>} The context.
         */
        this.file = async (buffer, fileName, { content, embed, rawJSON }) => {
            if (!Buffer.isBuffer(buffer)) throw new TypeError(`The first parameter must be a buffer.`);
            else if (!fileName || !(typeof fileName === 'string') || !/\./g.test(fileName)) throw new TypeError('Invalid filename.');

            return new Promise((ctx, err) => {
                client.http.request('POST', `/channels/${message.channel_id}/messages`, {
                    content: (content || '').slice(0, 2000),
                    embed: (rawJSON || embed) ? (rawJSON || parseEmbedObject(embed)) : {},
                    allowed_mentions: {
                        replied_user: false,
                        parse: []
                    },
                    message_reference: _sentMessage ? {} : {
                        message_id: message.id
                    }
                }, buffer, fileName).then((res, rej) => {
                    if (rej) return err(rej);
                    if (!_sentMessage) _sentMessage = true;
                    return ctx(new Context(client, this.command, res));
                });
            });
        };

        /**
         * @async
         * Sends a message in a form of an embed.
         * @param {object} [data] The embed object to send to.
         * @param {string} [data.title] The embed title.
         * @param {string} [data.description] The embed description.
         * @param {string} [data.type] Sets the embed type.
         * @param {string} [data.url] The embed URL.
         * @param {number} [data.color] The embed color.
         * @param {object[]} [data.fields] Embed fields.
         * @param {Boolean} [data.inlineFields] Sets all the fields as inline.
         * @param {string} [data.image] The embed image URL.
         * @param {string} [data.thumbnail] The embed thumbnail URL.
         * @param {object|string} [data.author] The embed author object.
         * @param {string|object} [data.footer] The embed footer object or text.
         * @returns {Promise<Context>} The message.
         */
        this.embed = (data) => {
            if (!typeof data === 'object') throw new TypeError(`Input must be an object.`);

            return new Promise((ctx, err) => {
                client.http.request('POST', `/channels/${message.channel_id}/messages`, {
                    content: (data.content || "").slice(0, 2000),
                    embed: data.rawJSON || parseEmbedObject(data),
                    allowed_mentions: {
                        replied_user: false,
                        parse: []
                    },
                    message_reference: _sentMessage ? {} : {
                        message_id: message.id
                    }
                }).then((res, rej) => {
                    if (rej) return err(rej);
                    if (!_sentMessage) _sentMessage = true;
                    return ctx(new Context(client, this.command, res));
                });
            });
        }

        /**
         * @async
         * Edits the message.
         * @param {string|object} [data] The new data. Can be either a string (representing a new text content) or an object to edit text content and/or embed at the same time.
         * @param {string} [data.content] The new message content.
         * @param {object} [data.embed] The new message embed.
         * @returns {void}
         */
        this.edit = async (data) => {
            // you can't edit messages from other users
            if (message.author.id !== client.user.id || _deleted) return;

            const json = {};
            if (typeof data === 'string' || data.content) json.content = (typeof data === 'string' ? data : data.content).slice(0, 2000);
            if (data.embed) json.embed = parseEmbedObject(data.embed);
            if (!json) throw new Error(`No data provided.`);
            await client.http.request('PATCH', `/channels/${message.channel_id}/messages/${message.id}`, json);
        };

        /**
         * @async
         * Deletes the message.
         * @returns {void}
         */
        this.delete = async () => {
            if (_deleted) return;
            await client.http.request('DELETE', `/channels/${message.channel_id}/messages/${message.id}`);
            _deleted = true;
        }
        
        /**
         * Gets the author avatar URL.
         * @param {object} [param] The parameters
         * @param {string} [param.format] The format, could be either gif, png, jpg, jpeg, or webp.
         * @param {number} [param.size] The avatar size.
         * @returns {string} The author avatar URL.
         */
        this.avatar = ({ format, size }) => {
            if (message.author.avatar) {
                const avatarAnimated = message.author.avatar.startsWith('a_');
                if (format) format = format.toLowerCase().replace(/\./g, '');
                if (format && !['gif', 'png', 'jpeg', 'jpg', 'webp'].includes(format)) throw new TypeError(`Invalid format: ${format}`);
                if (format === 'gif' && !avatarAnimated) format = 'png';
    
                size = Number(size) || 4096;
                if (size & (size - 1) || (size < 16 || size > 4096)) throw new TypeError(`The size must be a power of two between 16 and 4096.`);
                return `https://cdn.discordapp.com/avatars/${message.author.id}/${message.author.avatar}.${format || (avatarAnimated ? 'gif' : 'png')}?size=${size}`;
            }
            return `https://cdn.discordapp.com/embed/avatars/${Number(message.author.discriminator) % 5}.png`;
        }
        
        /**
         * Gets the raw message object.
         * @returns {object} The message object.
         */
        this.raw = () => message;
        
        /**
         * @async
         * Waits for a message response.
         * @param {function} check The function to check whether a message is valid.
         * @param {number} time The wait timeout in milliseconds.
         * @returns {Promise<Context | null>} A context object, or null if timed out.
         */
        this.wait = (check, time) => {
            if (isNaN(time) || !Number.isInteger(time)) throw new TypeError('Number must be an integer.');
            else if (!typeof check === 'function') throw new TypeError('Function must be provided.');
            else if (client.waits[`${message.channel_id}${message.author.id}`]) throw new Error(`A timeout is already running.`);

            return new Promise(resolve => {
                const bucket = `${message.channel_id}${message.author.id}`;
                const timeout = setTimeout(data => {
                    clearTimeout(timeout);
                    if (client.waits[bucket]) delete client.waits[bucket];
                    return resolve(data || null);
                }, time);
                
                client.waits[bucket] = { timeout, check };
            });
        }
    }

    /**
     * The arguments passed onto the command.
     */
    get args() {
        return this.content.split(' ').slice(1);
    }
}

module.exports = { Command, Context };