const { Inflate } = require('pako');
const { request } = require('https');
const zlib = require('zlib');
const bufferSuffix = Buffer.from("\r\n------------------discordbotlib--");

class HTTPException extends Error {
    constructor(raw, response) {
        super(`ERROR ${raw.statusCode || '???'}: ${response || 'Unknown'}`);
        this.statusCode = raw ? raw.statusCode : null;
        this.headers = raw ? raw.headers : null;
        this.raw = raw;
        this.response = response;
    }
}

class SequentialBucket {
    constructor(limit, latencyRef) {
        this.limit = this.remaining = limit;
        this.reset = 0;
        this.processing = false;
        this.latencyRef = latencyRef || { latency: 0 };
        this._queue = new Array();
    }

    check(override) {
        if (!this._queue.length) {
            if (this.processing) {
                clearTimeout(this.processing);
                this.processing = false;
            }
            return;
        }

        if (this.processing && !override) return;
        const now = Date.now();
        const offset = this.latencyRef.latency + (this.latencyRef.offset || 0);

        if (!this.reset || this.reset < now - offset) {
            this.reset = now - offset;
            this.remaining = this.limit;
        }

        this.last = now;
        if (this.remaining <= 0) {
            this.processing = setTimeout(() => {
                this.processing = false;
                this.check(true);
            }, Math.max(0, (this.reset || 0) - now + offset) + 1);
        }
        this.remaining--;
        this.processing = true;
        this._queue.shift()(() => {
            if (this._queue.length)
                this.check(true);
            else
                this.processing = false;
        });
    }

    queue(func, short) {
        if (short)
            this._queue.unshift(func);
        else
            this._queue.push(func);
        this.check();
    }
}

class HTTPClient {
    constructor(bot) {
        this.bot = bot;
        this.baseURL = bot.opt.baseURL || `/api/v${bot.opt.APIVersion || 7}`;
        this.ratelimits = {};
        this.requestTimeout = bot.opt.requestTimeout || 15000;
        this.globalBlock = false;
        this.userAgent = bot.opt.userAgent || `DiscordBot (https://github.com/null8626/discord-bot-lib)`;
        this.latencyRef = {
            latency: 500,
            offset: bot.opt.ratelimiterOffset || 0,
            raw: new Array(10).fill(500),
            timeOffset: 0,
            timeOffsets: new Array(10).fill(0),
            lastTimeOffsetCheck: 0
        };
        this.readyQueue = new Array();
    }

    /**
     * A copy of https://github.com/abalabahaha/eris/blob/master/lib/util/MultipartData.js.
     * The entire file compressed to a single function lol.
     * @param {object[]} arr Array of data objects.
     * @returns {Buffer[]} An array of buffers or something.
     */
    resolveBuffer(arr) {
        let bufs = [];
        for (const data of arr.filter(x => x)) {
            const isBuf = ArrayBuffer.isView(data.buffer);
            if (!isBuf && typeof data.buffer === 'object')
                data.buffer = Buffer.from(JSON.stringify(data.buffer));
            else if (isBuf && !(data.buffer instanceof Uint8Array))
                data.buffer = new Uint8Array(data.buffer.buffer, data.buffer.byteOffset, data.buffer.byteLength);

            bufs.push(Buffer.from(`\r\n------------------discordbotlib\r\nContent-Disposition: form-data; name="${data.fieldName}"${data.fileName ? '; filename="' + data.fileName + '"' : ''}\r\nContent-Type: application/${isBuf ? 'octet-stream' : 'json'}\r\n\r\n`));
            bufs.push(data.buffer);
        }

        return [...bufs, bufferSuffix];
    }

    globalUnblock() {
        this.globalBlock = false;
        while (this.readyQueue.length > 0)
            this.readyQueue.shift()();
    }

    route(url, method) {
        let route = url.replace(/\/([a-z-]+)\/(?:[0-9]{17,19})/g, (match, p) => {
            return p === "channels" || p === "guilds" || p === "webhooks" ? match : `/${p}/:id`;
        }).replace(/\/reactions\/[^/]+/g, "/reactions/:id").replace(/^\/webhooks\/(\d+)\/[A-Za-z0-9-_]{64,}/, "/webhooks/$1/:token");
        
        if (method === "DELETE" && route.endsWith("/messages/:id"))
            route = method + route;
        return route;
    }

    /**
     * Does a HTTP request to the Discord API.
     * @param {string} method The request method.
     * @param {string} path The request path.
     * @param {object} body The request body (optional)
     * @param {string} _route The request route (also optional)
     * @param {Boolean} short tall
     * @returns {Promise<object|string>} the response
     */
    request(method, path, body, fileBuffer, fileName, _route, short) {
        const route = _route || this.route(path, method);

        return new Promise((res, rej) => {
            let tries = 0;
            let url = this.baseURL + path;

            const requestFunc = (callback) => {
                const headers = {
                    "User-Agent": this.userAgent,
                    "Accept-Encoding": "gzip,deflate",
                    "X-RateLimit-Precision": "millisecond",
                    Authorization: `Bot ${this.bot.opt.token}`
                };
                let data;

                if (fileBuffer && fileName) {
                    headers["Content-Type"] = "multipart/form-data; boundary=----------------discordbotlib";
                    data = this.resolveBuffer([{
                        fileName,
                        buffer: fileBuffer,
                        fieldName: `file`
                    }, body ? {
                        fieldName: `payload_json`,
                        buffer: body
                    } : null]);
                } else if (body) {
                    if (method === "GET" || method === "DELETE") {
                        let qs = "";
                        for (const entry of Object.entries(body).filter(x => x[1] != undefined)) {
                            if (Array.isArray(entry[1]))
                                for (const child of entry[1])
                                    qs += `&${encodeURIComponent(entry[0])}=${encodeURIComponent(child)}`;
                            else
                                qs += `&${encodeURIComponent(entry[0])}=${encodeURIComponent(entry[1])}`;
                        }
                        url += `?${qs.substring(1)}`;
                    } else {
                        data = JSON.stringify(body);
                        headers["Content-Type"] = "application/json";
                    }
                }

                const req = request({
                    method: method,
                    host: "discord.com",
                    path: url,
                    headers: headers,
                    agent: this.bot.opt.agent || null
                });

                let reqError;

                req.once("abort", () => {
                    callback();
                    reqError = reqError || new Error(`Request aborted by client on ${method} ${path}`);
                    reqError.req = req;
                    rej(reqError);
                }).once("error", err => {
                    reqError = err;
                    req.abort();
                });

                let latency = Date.now();

                req.once("response", resp => {
                    latency = Date.now() - latency;
                    this.latencyRef.raw.push(latency);
                    this.latencyRef.latency = this.latencyRef.latency - ~~(this.latencyRef.raw.shift() / 10) + ~~(latency / 10);

                    const headerNow = Date.parse(resp.headers["date"]);
                    if (this.latencyRef.lastTimeOffsetCheck < Date.now() - 5000) {
                        const timeOffset = headerNow + 500 - (this.latencyRef.lastTimeOffsetCheck = Date.now());
                        this.latencyRef.timeOffset = this.latencyRef.timeOffset - ~~(this.latencyRef.timeOffsets.shift() / 10) + ~~(timeOffset / 10);
                        this.latencyRef.timeOffsets.push(timeOffset);
                    }

                    resp.once("aborted", () => {
                        callback();
                        reqError = reqError || new Error(`Request aborted by server on ${method} ${path}`);
                        reqError.req = req;
                        rej(reqError);
                    });

                    let response = '';
                    let respStream = resp;

                    if (resp.headers['content-encoding']) {
                        if(resp.headers["content-encoding"].includes("gzip"))
                            respStream = resp.pipe(zlib.createGunzip());
                        else if (resp.headers["content-encoding"].includes("deflate"))
                            respStream = resp.pipe(zlib.createInflate());
                    }

                    respStream.on('data', d => response += d).on('error', err => {
                        reqError = err;
                        req.abort();
                    }).once('end', () => {
                        const now = Date.now();

                        if (resp.headers['x-ratelimit-limit'])
                            this.ratelimits[route].limit = +resp.headers["x-ratelimit-limit"];
                        
                        this.ratelimits[route].remaining = resp.headers["x-ratelimit-remaining"] === undefined ? 1 : +resp.headers["x-ratelimit-remaining"] || 0;
                        let retryAfter = parseInt(resp.headers["retry-after"]);

                        if (retryAfter && (typeof resp.headers.via !== "string" || !resp.headers.via.includes("1.1 google")))
                            retryAfter *= 1000;
                        
                        if (retryAfter >= 0) {
                            if (resp.headers["x-ratelimit-global"]) {
                                this.globalBlock = true;
                                setTimeout(() => this.globalUnblock(), retryAfter || 1);
                            } else {
                                this.ratelimits[route].reset = (retryAfter || 1) + now;
                            }
                        } else if (resp.headers["x-ratelimit-reset"])
                            this.ratelimits[route].reset = ((~route.lastIndexOf("/reactions/:id")) && (+resp.headers["x-ratelimit-reset"] * 1000 - headerNow) === 1000) ? (now + 250) : (Math.max(+resp.headers["x-ratelimit-reset"] * 1000 - this.latencyRef.timeOffset, now));
                        else
                            this.ratelimits[route].reset = now;
                        
                        if (this.bot.opt[`onHTTPResponse${resp.statusCode}`])
                            this.bot.opt[`onHTTPResponse${resp.statusCode}`](resp);
                        
                        if (resp.statusCode >= 300) {
                            if (resp.statusCode === 429) {
                                callback();
                                if (retryAfter) {
                                    setTimeout(() => {
                                        this.request(method, url, body, fileBuffer, fileName, route, true).then(res).catch(rej);
                                    }, retryAfter);
                                } else
                                    this.request(method, url, body, fileBuffer, fileName, route, true).then(res).catch(rej);
                                return;
                            } else if (resp.statusCode === 502 && ++tries < 4) {
                                setTimeout(() => {
                                    this.request(method, url, body, fileBuffer, fileName, true).then(res).catch(rej);
                                }, Math.floor(Math.random() * 1900 + 100));
                                return callback();
                            }
                            callback();
                            rej(new HTTPException(resp, response));
                        }

                        if (response.length && resp.headers["content-type"] === "application/json") {
                            try { response = JSON.parse(response); } catch (err) {
                                callback();
                                rej(err);
                                return;
                            }
                        }

                        callback();
                        res(response);
                    });
                });

                req.setTimeout(this.requestTimeout, () => {
                    reqError = new Error(`Request timed out (${this.requestTimeout}ms) on ${method} ${path}`);
                    req.abort();
                });

                if (Array.isArray(data)) {
                    for (const chunk of data)
                        req.write(chunk);
                    req.end();
                } else req.end(data);
            };

            if (this.globalBlock) {
                this.readyQueue.push(() => {
                    if (!this.ratelimits[route])
                        this.ratelimits[route] = new SequentialBucket(1, this.latencyRef);
                    this.ratelimits[route].queue(requestFunc, short);
                });
            } else {
                if (!this.ratelimits[route])
                    this.ratelimits[route] = new SequentialBucket(1, this.latencyRef);
                this.ratelimits[route].queue(requestFunc, short);
            }
        });
    }
}

module.exports = {
    /**
     * Converts the object to readable-by-discord embed object.
     * @param {object} obj The object to parse from.
     * @returns {object} the object readable by the discord API (mostly contains snake_case)
     */
    parseEmbedObject: (obj) => {
        let total = {
            type: typeof obj.type === 'string' ? obj.type : "rich",
            title: (obj.title || "").slice(0, 256),
            description: (obj.description || "").slice(0, 2048),
            fields: [],
            url: obj.url
        };

        if (obj.fields) {
            if (typeof obj.fields === 'object')
                for (const field in obj.fields)
                    total.fields.push({ name: field.slice(0, 256), value: obj.fields[field].slice(0, 1024) });
            else if (Array.isArray(obj.fields) && obj.fields.every(x => typeof x === 'object' && x.name && x.value))
                total.fields = obj.fields.map(x => { return { name: x.name, value: x.value, inline: x.inline || false } });
        } else
            delete total['fields'];
        
        if (obj.color && typeof obj.color === 'number')
            total.color = obj.color;

        if (obj.footer) {
            if (typeof obj.footer === 'string') total.footer = { text: obj.footer };
            else if (typeof obj.footer === 'object') total.footer = { text: obj.footer.text, icon_url: obj.footer.iconURL };
            else throw new Error(`Invalid type for a footer: ${typeof obj.footer}`);
            total.footer.text = total.footer.text.slice(0, 2048);
        }

        if (obj.author) {
            if (typeof obj.author === 'object') {
                let author = {};
                if (obj.author.name) author.name = obj.author.name.slice(0, 256);
                if (obj.author.url) author.url = obj.author.url;
                if (obj.author.iconURL) author.icon_url = obj.author.iconURL;
                if (author) total.author = author;
            } else if (typeof obj.author === 'string')
                total.author = { name: obj.author };
        }

        if (obj.thumbnail && typeof obj.thumbnail === 'string') total.thumbnail = { url: obj.thumbnail };
        if (obj.image && typeof obj.image === 'string') total.image = { url: obj.image };
        if (obj.includeTimestamp) total.timestamp = new Date().toISOString();
        if (total.fields && obj.inlineFields !== undefined)
            for (const field of total.fields)
                field.inline = obj.inlineFields || false;

        for (const key in total)
            if (!total[key] || (Array.isArray(total[key]) && !total[key].length))
                delete total[key];
        
        return total;
    },
    
    /**
     * Adjusts intent from a gateway event name/intent string.
     * Context: https://discord.com/developers/docs/topics/gateway#gateway-intents
     * @param {string} event The event name/intent string, lowercased, no underscores.
     * @param {Object} options Bot options.
     * @param {Number[]} list The current intents list.
     * @return {Number} the intent number.
     */
    parseIntentValue: (event, options, list) => {
        switch (event) {
            case 'guilds':
            case 'guildcreate':
            case 'guildupdate':
            case 'guilddelete':
            case 'guildrolecreate':
            case 'guildroleupdate':
            case 'guildroledelete':
            case 'channelcreate':
            case 'channelupdate':
            case 'channeldelete':
            case 'channelpinsupdate':
                return 1 << 0;
            case 'guildmembers':
            case 'guildmemberadd':
            case 'guildmemberupdate':
            case 'guildmemberremove':
                return 1 << 1;
            case 'guildbans':
            case 'guildbanadd':
            case 'guildbanremove':
                return 1 << 2;
            case 'guildemojis':
            case 'guildemojisupdate':
                return 1 << 3;
            case 'guildintegrations':
            case 'guildintegrationsupdate':
            case 'integrationcreate':
            case 'integrationupdate':
            case 'integrationdelete':
                return 1 << 4;
            case 'guildwebhooks':
            case 'webhooksupdate':
                return 1 << 5;
            case 'guildinvites':
            case 'invitecreate':
            case 'invitedelete':
                return 1 << 6;
            case 'guildvoicestates':
            case 'voicestateupdate':
                return 1 << 7;
            case 'guildpresences':
            case 'presenceupdate':
                return 1 << 8;
            case 'guildmessages':
                return 1 << 9;
            case 'messagecreate':
            case 'messageupdate':
            case 'messagedelete':
            case 'messagedeletebulk':
                if (options.dmsOnly)
                    return 1 << 12;
                else if (options.includeDMs) {
                    if (list.includes(1 << 9) && !list.includes(1 << 12)) return 1 << 12;
                    else if (!list.includes(1 << 9) && list.includes(1 << 12)) return 1 << 9;
                }
                return 1 << 9;
            case 'messagereactionadd':
            case 'messagereactionremove':
            case 'messagereactionremoveall':
            case 'messagereactionremoveemoji':
                if (options.dmsOnly)
                    return 1 << 13;
                else if (options.includeDMs) {
                    if (list.includes(1 << 10) && !list.includes(1 << 13)) return 1 << 13;
                    else if (!list.includes(1 << 10) && list.includes(1 << 13)) return 1 << 10;
                }
                return 1 << 10;
            case 'typingstart':
                if (options.dmsOnly)
                    return 1 << 14;
                else if (options.includeDMs) {
                    if (list.includes(1 << 11) && !list.includes(1 << 14)) return 1 << 14;
                    else if (!list.includes(1 << 11) && list.includes(1 << 14)) return 1 << 11;
                }
                return 1 << 11;
            case 'directmessages':
                return 1 << 12;
            case 'directmessagereactions':
                return 1 << 13;
            case 'directmessagetyping':
                return 1 << 14;
            default:
                return 0;
        }
    },

    /**
     * Auto-adjusts intents from the gateway events the bot listens to.
     * @param {Bot} bot The bot object.
     * @returns {Number} The intents number.
     */
    autoAdjustIntents: (bot) => {
        const intentList = [];
        
        for (const event of Object.keys(bot.opt).filter(x => x.startsWith('_event_listener_')).map(y => y.slice(16))) {
            const intent = module.exports.parseIntentValue(event, bot.opt, intentList);
            
            if (!intentList.includes(intent))
                intentList.push(intent);
        }
        
        if (bot.cmds.length) {
            if (bot.opt.dmsOnly && !intentList.includes(1 << 12)) {
                const index = intentList.indexOf(1 << 9);
                if (index > -1) intentList.splice(index, 1);
                intentList.push(1 << 12);
            } else if (bot.opt.includeDMs) {
                if (intentList.includes(1 << 9) && !intentList.includes(1 << 12)) intentList.push(1 << 12);
                else if (!intentList.includes(1 << 9) && intentList.includes(1 << 12)) intentList.push(1 << 9);
            } else if (!intentList.includes(1 << 9))
                intentList.push(1 << 9);
        }
        
        return intentList.reduce((a, b) => a | b, 0);
    },

    /**
     * Parses the intent parameter.
     * @param {Bot} bot The bot object.
     * @returns {number} the intent bitfield.
     */
    handleIntents: (bot) => {
        const params = bot.opt.intents || 1 << 9;
        
        if (typeof params === 'string') {
            if (params.toLowerCase() === 'auto') return module.exports.autoAdjustIntents(bot);
            params = [params];
        } else if (Number.isInteger(params)) return params;
        else if (!Array.isArray(params)) return 1 << 9;
        else if (params.every(n => Number.isInteger(n))) return Object.values(params).reduce((a, b) => a | b, 0);
        
        let bits = 0;
        for (const param of params)
            bits |= module.exports.parseIntentValue(param.replace(/[\_ ]/g, '').toLowerCase());
        return bits;
    },

    /**
     * No description.
     */
    evaluate: (msg, flag) => {
        if (!flag || typeof flag !== 'object') flag = {};
        if (!flag.binary) return JSON.parse(msg);
        const inflator = new Inflate();
        inflator.push(msg);
        if (inflator.err) throw new Error('An error occurred while decompressing data');
        return JSON.parse(inflator.toString());
    },

    /**
     * @async
     * Function that is fired by default whenever the websocket gets a response.
     */
    onWebsocketMessage: async (bot, data, flag) => {
        const msg = module.exports.evaluate(data, flag);

        if (msg.t) {
            const customListener = bot.opt[`_event_listener_${msg.t.toLowerCase().replace(/_/g, '')}`];
            if (customListener && (msg.t !== 'READY' || msg.t !== 'MESSAGE_CREATE')) await customListener(msg.d);
            else {
                if (customListener) await customListener(msg.d);

                switch (msg.t) {
                    case 'READY':
                        if (!msg.d.user.bot) throw new Error(`Please do NOT use this lib for userbotting/selfbotting. That's gross.`);
                        bot.sessionID = msg.d.session_id;
                        bot.user = msg.d.user;
                        break;
                    case 'MESSAGE_CREATE':
                        await bot._parseMessage(msg.d);
                }
            }
        }

        switch (msg.op) {
            case 7: // reconnect
                return bot.socket.send(JSON.stringify({
                    op: 6,
                    d: {
                        session_id: bot.sessionID,
                        token: bot.opt.token,
                        seq: bot.seqNum
                    }
                }));
            case 10: // heartbeat
                if (bot.hb) clearInterval(bot.hb);
                bot.hb = setInterval(() => {
                    return bot.socket.send(JSON.stringify({
                        op: 1,
                        d: bot.seqNum
                    }));
                }, msg.d.heartbeat_interval);
        }
    },

    /**
     * Function that is fired by default whenever the websocket closes.
     */
    onWebsocketClose: (bot, errCode) => {
        if (bot.hb) clearInterval(bot.hb);
        if (errCode === 4004) throw new TypeError('Invalid client token.');
        bot.close();
        bot.run();
    },
    
    /**
     * Check if the text is empty.
     * @param {string} text The text.
     * @returns {Boolean} if it is empty or not.
     */
    isEmpty: (text) => {
        if (!text.length) return true;
        return !text.replace(/[ \u0020\u00A0\u2000\u2001\u2002\u2003\u2004\u2005\u2006\u2007\u2008\u2009\u200A\u2028\u205F\u3000]/g, '').length;
    },
    
    /**
     * @async
     * Gets the command name based on the bot's prefix.
     * @param {string|string[]|function} prefixAttribute The prefix attribute from the client options.
     * @param {object} message The raw message object from discord.
     * @returns {string} The command name.
     */
    getCommandName: async (prefixAttribute, message) => {
        const content = message.content.split(' ')[0].toLowerCase();
        
        switch (typeof prefixAttribute) {
            case 'string':
                return content.startsWith(prefixAttribute) ? content.slice(prefixAttribute.length) : null;
            case 'object':
                for (const prefix of prefixAttribute)
                    if (content.startsWith(prefix))
                        return content.slice(prefix.length);
                break;
            case 'function':
                const prefix = await prefixAttribute(message);
                if (typeof prefix !== 'string') throw new Error('The function does not return a valid prefix.');
                return content.slice(prefix.length);
        }
    },

    /**
     * Custom-made exception for issues whenever the bot does a request to the discord API.
     */
    HTTPException,

    /**
     * HTTP handler. (copied from eris i know)
     */
    HTTPClient
};