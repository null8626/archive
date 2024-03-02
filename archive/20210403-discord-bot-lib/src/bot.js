const { Command, Context } = require('./command');
const util = require('./util');
const ws = require('ws');
const EventEmitter = require('events');
const { platform } = require('os');
const { execFileSync } = require('child_process');

module.exports = class Bot extends EventEmitter {
    /**
     * The main bot object. This is where it all began, folks.
     * @param {object}   [options] options for the bot.
     * @param {string}   [options.token] The bot token.
     * @param {string|string[]|function} [options.prefix] The bot prefix to used by the command handler.
     * @param {number|number[]|string|string[]} [options.intents] The intent values to be used by the bot.
     * @param {string}   [options.status] The bot status string to be activated upon connection.
     * @param {object[]} [options.activities] Array of activity object to be activated upon connection.
     * @param {string}   [options.activityName] The activity text to be activated upon connection.
     * @param {number}   [options.activityType] The activity type to be activated upon connection.
     * @param {Boolean}  [options.dmsOnly] Whether to ONLY accept messages from DMs.
     * @param {Boolean}  [options.includeDMs] Whether to accept both guild and DM messages.
     * @param {function} [options.messageCheck] The function to be called to check if a message is valid (does not include prefix checking)
     * @param {function} [options.onCooldown] The global function to be called whenever a command cooldown is hit.
     * @param {function} [options.onWebsocketMessage] The function to be called whenever the websocket receives a message from the gateway.
     * @param {function} [options.onWebsocketError] The function to be called whenever the websocket throws an error.
     * @param {function} [options.onWebsocketClose] The function to be called whenever the websocket closes.
     * @param {string}   [options.websocketAddress] The websocket URI to be used.
     * @param {object}   [options.websocketProperties] The data object to send to discord during the ready event.
     * @param {number}   [options.websocketVersion] The webscoket URI version (on the query params)
     * @returns {Bot} the bot object, duh.
     */
    constructor(options) {
        if (typeof options !== 'object') throw new TypeError(`Options should be an object, not ${typeof options}.`);
        super(options);

        this.opt = Object.assign({
            prefix: '!',
            intents: 'auto'
        }, options);
        this.http = new util.HTTPClient(this);
        this.cmds = new Array();
        this.socket = null;
        this.user = null;
        this.seqNum = 1;
        this.sessionID = null;
        this.waits = {};

        if (options.commands && options.commands.length) {
            for (const command of options.commands) this.command(command);
            delete this.opt['commands'];
        }
        
        if (Array.isArray(this.opt.prefix))
            this.opt.prefix = this.opt.prefix.map(x => x.toLowerCase());
    }

    /**
     * Explicitly assigns the global function to call whenever a command hits a cooldown.
     * @param {function} callback The function.
     * @returns {Bot} Bot object
     */
    onCooldown(callback) {
        this.opt.onCooldown = callback;
        return this;
    }

    /**
     * Explicitly assigns a custom function on a specific type of event.
     * @param {string} type The event type.
     * @param {function} callback The callback function.
     */
    event(type, callback) {
        if (typeof callback !== 'function') throw new TypeError(`callback must be a function.`);
        
        if (['raw', '*', 'any'].includes(type.toLowerCase()))
            this.opt.onWebsocketMessage = callback;
        else
            this.opt[`_event_listener_${type.toLowerCase().replace(/[ \_]/g, '')}`] = callback;
        return this;
    }

    /**
     * Explicitly assigns a custom function whenever the client receives a specific status code from the discord API.
     * @param {number} statusCode The status code.
     * @param {function} callback The callback function.
     * @returns {Bot} Bot object
     */
    onHTTPResponse(statusCode, callback) {
        if (!Number.isInteger(statusCode) || statusCode.toString().length !== 3) throw new TypeError(`Invalid status code.`);
        if (typeof callback !== 'function') throw new TypeError(`callback must be a function.`);

        this.opt[`onHTTPResponse${statusCode}`] = callback;
        return this;
    }
    
    /**
     * Explicitly assigns a custom function whenever the client receives an Error (globally)
     * @param {function} callback The callback function
     * @returns {Bot} Bot object
     */
    error(callback) {
        if (typeof callback !== 'function') throw new TypeError(`callback must be a function.`);
        this.opt.onError = callback;
        return this;
    }

    /**
     * @param {object} message The message object.
     * @returns {Boolean} Whether the check is successful or not.
     */
    _resolveWaits(message) {
        const wait = this.waits[`${message.channel_id}${message.author.id}`];
        if (!wait || !wait.check(message)) return false;
        wait.timeout._onTimeout(message);
        return true;
    }

    /**
     * @async
     * Parses and checks if the message is a command or not.
     * @param {object} message The message
     * @returns {any}
     */
    async _parseMessage(message) {
        if (this._resolveWaits(message) || (typeof this.opt.messageCheck === 'function' && !this.opt.messageCheck(message))) return;
        if (this.opt.dmsOnly && message.guild) return;

        const commandName = await util.getCommandName(this.opt.prefix, message);
        if (!commandName) return;
        const command = this.cmds.filter(cmd => cmd.name === commandName || cmd.aliases.includes(commandName))[0];
        if (!command) return;
        
        const cooldownTimeout = command.resolveCooldown(message);
        if (cooldownTimeout) {
            if (command.onCooldown) await command.onCooldown(cooldownTimeout, new Context(this, command, message));
            else if (this.opt.onCooldown) await this.opt.onCooldown(cooldownTimeout, new Context(this, command, message));
            return;
        }
        
        if (command.noArgs && util.isEmpty(message.content.split(' ').slice(1).join('')))
            return await command.noArgs(new Context(this, command, message));

        const ctx = new Context(this, command, message);
        try {
            return await command.run(ctx);
        } catch (err) {
            if (command.onError) return await command.onError(ctx, err);
            if (this.opt.onError) return await this.opt.onError(ctx, err);
            throw err;
        } finally {
            ctx.command.usageCount++;
        }
    }
    
    /**
     * Sets the status to discord.
     * @param {string} type The status type. This could be online, idle, dnd, or invisible.
     * @returns {void}
     */
    setStatus(type) {
        if (!type || typeof type !== 'string' || !['online', 'idle', 'dnd', 'invisible'].includes(type.toLowerCase())) throw new TypeError(`Invalid status.`);
        
        this.socket.send(JSON.stringify({
            op: 3,
            d: {
                status: type.toLowerCase(),
                afk: false,
                since: type.toLowerCase() === 'idle' ? Date.now() : null,
                game: null
            }
        }));
    }

    /**
     * Adds a command to the bot.
     * @param {Command|object} param a Command object or an object representing the command.
     * @returns {Bot} Bot object.
     */
    command(param) {
        this.cmds.push(param instanceof Command ? param : new Command(param));
        return this;
    }
    
    /**
     * Gets the presence object.
     * @returns {object} The presence object.
     */
    getPresenceObject() {
        const presenceObject = { status: ( this.opt.status && typeof this.opt.status === 'string' && ['online', 'idle', 'dnd', 'invisible'].includes(this.opt.status.toLowerCase()) ) ? this.opt.status.toLowerCase() : 'online' };
        if (this.opt.activities || this.opt.activityName || this.opt.activityType) {
            presenceObject.activities = this.opt.activities || [{
                name: this.opt.activityName || "Bot made using discord-bot-lib",
                activity: this.opt.activityType || 0
            }];
        }
        
        if (presenceObject.status === 'idle')
            presenceObject.since = Date.now();
        
        return presenceObject;
    }

    /**
     * Runs the bot and initiates the websocket.
     * @returns {void}
     */
    run() {
        if (!this.opt.token || typeof this.opt.token !== 'string') throw new TypeError('Missing a valid token in the opt attribute.');
        else if (this.socket) throw new TypeError('A websocket attribute is already running.');
        
        this.socket = new ws(this.opt.websocketAddress || `wss://gateway.discord.gg/?v=${this.opt.websocketVersion || 8}&encoding=json`);
        this.socket.once('open', () => {
            this.socket.send(JSON.stringify({
                op: 2,
                d: {
                    token: this.opt.token,
                    intents: util.handleIntents(this),
                    properties: this.opt.websocketProperties || {
                        $os: platform,
                        $browser: 'discord.js',
                        $device: 'discord.js'
                    },
                    presence: this.getPresenceObject()
                }
            }));
            
            process.on('SIGINT', () => this.close(true));
            process.on('SIGTERM', () => this.close(true));

            this.socket.on('error', this.opt.onWebsocketError || (() => undefined));
            this.socket.on('message', (msg, flag) => (this.opt.onWebsocketMessage || util.onWebsocketMessage)(this, msg, flag));
            this.socket.on('close', errCode => (this.opt.onWebsocketClose || util.onWebsocketClose)(this, errCode));
        });
    }

    /**
     * Closes the bot.
     * @param {Boolean} closeProcess close the process too or not. defaults to false.
     * @returns {void}
     */
    close(closeProcess) {
        this.socket.close();
        this.socket = null;
        
        if (closeProcess) process.exit();
    }
}