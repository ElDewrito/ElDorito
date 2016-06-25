/**
 * Creates a new DewError object which represents an ElDewrito communication error.
 *
 * @class
 * @param {string} message - The error message.
 * @param {DewErrorCode} [code] - The error code.
 * @param {string} [method] - The internal method name.
 */
function DewError(message, code, method) {
    Error.captureStackTrace(this, this.constructor);

    /**
     * @member {string}
     */
    this.name = this.constructor.name;

    /**
     * The error message.
     * @member {string}
     */
    this.message = message || "";

    /**
     * The error code.
     * @member {DewErrorCode}
     */
    this.code = (typeof code === "number") ? code : DewErrorCode.UNKNOWN_ERROR;

    /**
     * The internal method name.
     * @member {string}
     */
    this.method = method || "";
}

/**
 * Gets the name of the error's code.
 *
 * @returns {string} A string representing the error's code.
 */
DewError.prototype.getCodeName = function () {
    for (var name in DewErrorCode) {
        if (DewErrorCode.hasOwnProperty(name) && DewErrorCode[name] === this.code) {
            return name;
        }
    }
    return this.code.toString();
}

/**
 * Error codes.
 *
 * @readonly
 * @enum {number}
 */
DewErrorCode = {
    /**
     * The method ran successfully.
     */
    OK: 0,

    /**
     * An unknown error occurred while running the method.
     */
    UNKNOWN_ERROR: 1,

    /**
     * A JavaScript exception occurred while running the method.
     */
    JS_EXCEPTION: 2,

    /**
     * A game communication error occurred.
     */
    BAD_QUERY: 3,

    /**
     * The method is not supported by the game.
     */
    UNSUPPORTED_METHOD: 4,

    /**
     * An invalid argument was passed to the method.
     */
    INVALID_ARGUMENT: 5,

    /**
     * An error occurred while performing a network operation.
     */
    NETWORK_ERROR: 6,

    /**
     * The requested information is not available.
     */
    NOT_AVAILABLE: 7,

    /**
     * The console command failed to execute successfully.
     */
    COMMAND_FAILED: 8
};

/**
 * Multiplayer event audiences.
 *
 * @readonly
 * @enum {number}
 */
MPEventAudience = {
    CAUSE_PLAYER: 0,
    CAUSE_TEAM: 1,
    EFFECT_PLAYER: 2,
    EFFECT_TEAM: 3,
    ALL: 4
};

/**
 * Multiplayer event categories.
 *
 * @readonly
 * @enum {number}
 */
MPEventCategory = {
    GENERAL: 0,
    FLAVOR: 1,
    SLAYER: 2,
    CTF: 3,
    ODDBALL: 4,
    FORGE: 5,
    KOTH: 6,
    VIP: 7,
    JUGGERNAUT: 8,
    TERRITORIES: 9,
    ASSAULT: 10,
    INFECTION: 11,
    SURVIVAL: 12,
    WP: 13
};

/**
 * Game modes.
 *
 * @readonly
 * @enum {number}
 */
GameMode = {
    NONE: 0,
    CTF: 1,
    SLAYER: 2,
    ODDBALL: 3,
    KOTH: 4,
    FORGE: 5,
    VIP: 6,
    JUGGERNAUT: 7,
    TERRITORIES: 8,
    ASSAULT: 9,
    INFECTION: 10
};

/**
 * Console command and variable types.
 *
 * @readonly
 * @enum {number}
 */
CommandType = {
    /**
     * A command.
     */
    COMMAND: 0,

    /**
     * An integer variable.
     */
    INT: 1,

    /**
     * A 64-bit integer variable.
     */
    INT64: 2,

    /**
     * A floating-point variable.
     */
    FLOAT: 3,

    /**
     * A string variable.
     */
    STRING: 4
};

(function () {
    window.dew = window.dew || {};

    // Default 1:1 result mapping.
    function defaultResultMapping(result) {
        return result;
    }

    // JSON result mapping.
    function jsonResultMapping(result) {
        return JSON.parse(result);
    }

    // Calls a native method and returns a Promise for it.
    dew.callMethod = function (method, args, resultMapping) {
        resultMapping = resultMapping || defaultResultMapping;
        args = args || {};
        return new Promise(function (resolve, reject) {
            // Ensure that we have a function to send queries to ED
            if (!window.dewQuery) {
                reject(new DewError("Unsupported method: window.dewQuery() is not available", DewErrorCode.UNSUPPORTED_METHOD, method));
                return;
            }

            // If args is a function, run it to get the actual arguments
            // This is useful in order to ensure that JS exceptions cause the promise to fail.
            if (typeof args === "function") {
                args = args();
            }

            // Send a CEF query
            window.dewQuery({
                request: JSON.stringify({
                    method: method,
                    args: args
                }),
                persistent: false,
                onSuccess: function (resultStr) {
                    // Try to map the result string and resolve the promise with it
                    var value;
                    try {
                        value = resultMapping(resultStr);
                    } catch (e) {
                        reject(e);
                        return;
                    }
                    resolve(value);
                },
                onFailure: function (code, message) {
                    reject(new DewError(message, code, method));
                }
            });
        });
    }

    // Registers an event handler for a UI event.
    function registerEvent(name, callback) {
        window.addEventListener("message", function (event) {
            var origin = event.origin;
            if (!origin.startsWith("dew:")) {
                return; // Only recognize messages sent by the UI
            }
            var data = event.data;
            if (typeof data !== "object" || !("event" in data)) {
                return;
            }
            if (data.event === name) {
                callback(data);
            }
        }, false);
    }

    // Posts a message to the UI.
    function postUiMessage(message, data) {
        // If this is running in the UI layer, then post to the current window, otherwise post to the parent
        var targetWindow = (window.location.href.startsWith("dew://ui/")) ? window : window.parent;
        if (targetWindow) {
            targetWindow.postMessage({
                message: message,
                data: data
            }, "*");
        }
    }

    // Disable text selection by default since most screens won't want it
    window.addEventListener("load", function (event) {
        document.body.style["-webkit-user-select"] = "none";
    });

    /**
     * Methods for interfacing with ElDewrito.
     *
     * @namespace dew
     */

    /**
     * A promise made by an asynchronous ElDewrito method.
     *
     * If the promise is rejected for a reason related to ElDewrito, it will be rejected with a {@link DewError} object.
     * This object includes an [error code]{@link DewErrorCode} which can be used to easily figure out what went wrong.
     *
     * If the promise is rejected for any other reason, it may be rejected with a generic Error.
     * Therefore, you must use `instanceof DewError` to check that the error is actually an ElDewrito error before getting specific information.
     *
     * @typedef {Promise<*|Error>} DewPromise
     */

    /**
     * (ASYNCHRONOUS) Retrieves the current version of ElDewrito.
     *
     * @returns {DewPromise<string>} - A promise for the version string.
     */
    dew.getVersion = function () {
        return dew.callMethod("version");
    }

    /**
     * Requests to show a screen.
     *
     * If the requested screen is still loading, it will not be shown until it finishes.
     *
     * This will always send a [show]{@link event:show} event to the screen, even if it is already visible.
     * You can use this as a simple means of sending messages between screens.
     *
     * @param {string} [id] - The ID of the screen to show. If this is null or omitted, the current screen will be shown.
     * @param {object} [data] - Data to pass to the screen's [show]{@link event:show} event.
     */
    dew.show = function (id, data) {
        postUiMessage("show", {
            screen: id || null,
            data: data || {}
        });
    }

    /**
     * Requests to hide a screen.
     *
     * This will send a [hide]{@link event:hide} event to the screen if it is visible.
     *
     * @param {string} [id] - The ID of the screen to hide. If this is null or omitted, the current screen will be hidden.
     */
    dew.hide = function (id) {
        postUiMessage("hide", {
            screen: id || null
        });
    }

    /**
     * Requests to change this screen's input capture state.
     *
     * @param {boolean} capture - true to capture mouse and keyboard input, false to release.
     */
    dew.captureInput = function (capture) {
        postUiMessage("captureInput", {
            capture: !!capture
        });
    }

    /**
     * (ASYNCHRONOUS) Runs a console command.
     *
     * If the command does not run successfully, the promise will be rejected with a [DewErrorCode.COMMAND_FAILED]{@link DewErrorCode} error.
     * The error message will be the command output.
     *
     * @param {string} command - The command and its arguments, separated by spaces.
     * @param {object} [options] - Additional options that control how the command should run.
     * @param {boolean} [options.internal=false] - If set to true, then internal commands can be executed.
     * @returns {DewPromise<string>} A promise for the command output.
     */
    dew.command = function (command, options) {
        return dew.callMethod("command", function () {
            options = options || {};
            return {
                command: command.toString(),
                internal: (typeof options.internal === "boolean") ? options.internal : false
            };
        });
    }

    /**
     * (ASYNCHRONOUS) Pings a server.
     *
     * The screen will receive a [pong]{@link event:pong} event if the server responds.
     *
     * **This method is currently broken and will report ping times that are much higher than they should be.**
     *
     * @param {string} address - The IPv4 address of the server to ping. Must not include a port number.
     * @returns {DewPromise} A promise that will be resolved once the ping is sent.
     */
    dew.ping = function (address) {
        return dew.callMethod("ping", function () {
            return {
                address: address.toString()
            };
        });
    }

    /**
     * (ASYNCHRONOUS) Gets info about the current map variant.
     *
     * If map variant info is not available, the promise will be rejected with a [DewErrorCode.NOT_AVAILABLE]{@link DewErrorCode} error.
     *
     * @returns {DewPromise<MapVariantInfo>} A promise for the map variant info.
     */
    dew.getMapVariantInfo = function () {
        return dew.callMethod("mapVariantInfo", {}, jsonResultMapping);
    }

    /**
     * Contains information about a map variant.
     *
     * @typedef {object} MapVariantInfo
     * @property {string} name - The name. Can be empty.
     * @property {string} description - The description. Can be empty.
     * @property {string} author - The author. Can be empty.
     * @property {number} mapId - The map ID.
     * @see dew.getMapVariantInfo
     */

    /**
     * (ASYNCHRONOUS) Gets info about the current game variant.
     *
     * If game variant info is not available, the promise will be rejected with a [DewErrorCode.NOT_AVAILABLE]{@link DewErrorCode} error.
     *
     * @returns {DewPromise<GameVariantInfo>} A promise for the game variant info.
     */
    dew.getGameVariantInfo = function () {
        return dew.callMethod("gameVariantInfo", {}, jsonResultMapping);
    }

    /**
     * Contains information about a game variant.
     *
     * @typedef {object} GameVariantInfo
     * @property {GameMode} mode - The base game mode.
     * @property {string} name - The name. Can be empty.
     * @property {string} description - The description. Can be empty.
     * @property {string} author - The author. Can be empty.
     * @property {boolean} teams - `true` if teams are enabled.
     * @property {number} timeLimit - The time limit in minutes (0 if unlimited).
     * @property {number} rounds - The number of rounds.
     * @property {number} scoreToWin - The score-to-win (-1 if unlimited).
     * @see dew.getGameVariantInfo
     */

    /**
     * (ASYNCHRONOUS) Gets a list of available console commands.
     *
     * @returns {DewPromise<ConsoleCommand[]>} A promise for the list of available console commands.
     */
    dew.getCommands = function () {
        return dew.callMethod("commands", {}, jsonResultMapping);
    }

    /**
     * Contains information about a console command and its current state.
     *
     * @typedef {object} ConsoleCommand
     * @property {CommandType} type - The type of the command or variable.
     * @property {string} module - The module name.
     * @property {string} name - The name of the command or variable. This includes the module prefix.
     * @property {string} shortName - The short name of the command or variable.
     * @property {string} description - A description to display in a help listing.
     * @property {*} value - The current value of the variable. For commands, this will be `null`.
     * @property {*} defaultValue - The default value of the variable. For commands, this will be `null`.
     * @property {*} minValue - The minimum value of the variable. For string variables and commands, this will be `null`.
     * @property {*} maxValue - The maximum value of the variable. For string variables and commands, this will be `null`.
     * @property {boolean} replicated - `true` if the variable should be synchronized to clients.
     * @property {boolean} archived - `true` if the variable should be saved when the config file is written.
     * @property {boolean} hidden - `true` if the command or variable should be omitted from a help listing.
     * @property {boolean} hostOnly - `true` if the command or variable can only be used by the game host.
     * @property {boolean} hideValue - `true` if the variable's value should be omitted from a help listing.
     * @property {boolean} internal - `true` if the command or variable can only be set internally.
     * @property {string[]} arguments - A list of arguments for the command. Each string will contain a value name, a space, and then a description. For variables, this will be empty.
     * @see dew.getCommands
     */

    /**
     * (ASYNCHRONOUS) Sends a chat message.
     *
     * @param {string} message - The chat message to send.
     * @param {boolean} teamChat - If true the message is sent to team chat instead of global.
     * @returns {DewPromise<boolean>} A promise for the success of sending the message.
     */
    dew.sendChat = function (message, teamChat) {
        return dew.callMethod("sendChat", function () {
            return {
                message: message.toString(),
                teamChat: teamChat
            };
        });
     }

    /**
     * (ASYNCHRONOUS) Gets information about the current game multiplayer session.
     *
     * @param {string} message - The chat message to send.
     * @param {boolean} teamChat - If true the message is sent to team chat instead of global.
     * @returns {DewPromise<SessionInfo>} A promise for the game multiplayer session information.
     */
    dew.getSessionInfo = function () {
        return dew.callMethod("sessionInfo", {}, jsonResultMapping);
    }

    /**
     * Contains information about the current game multiplayer session
     *
     * @typedef {object} SessionInfo
     * @property {boolean} established - `true` if a session is established.
     * @property {boolean} hasTeams - `true` if the current session has teams.
     * @property {boolean} isHost - `true` if the player is the host of the session.
     * @property {string} mapName - Name of the currently loaded map.
     * @see dew.getSessionInfo
     */

     /**
      * (ASYNCHRONOUS) Gets information about the current game's scoreboard.
      *
      * @returns {DewPromise<ScoreboardInfo>} A promise for the scoreboard information.
      */
     dew.getScoreboard = function () {
         return dew.callMethod("scoreboard", {}, jsonResultMapping);
     }

     /**
      * Contains information about the scoreboard.
      *
      * @typedef {object} ScoreboardInfo
      * @property {boolean} hasTeams - `true` if the current session has teams.
      * @property {number[]} teamScores - The scores of all of the teams in the game.
      * @property {string} gameType - The gamemode type.
      * @property {ScoreboardPlayer[]} players - Players listed on the scoreboard.
      * @see dew.getScoreboard
      */

     /**
      * Contains information about a player on the scoreboard.
      *
      * @typedef {object} ScoreboardPlayer
      * @property {string} name - The player's name.
      * @property {number} team - The player's team index.
      * @property {string} color - The player's primary armor color.
      * @property {string} UID - The player's UID.
      * @property {number} kills - The number of kills.
      * @property {number} assists - The number of assists.
      * @property {number} deaths - The number of deaths.
      * @property {number} score - The player's score.
      * @see ScoreboardInfo
      */

    /**
     * (ASYNCHRONOUS) Gets information about a player's stats.
     *
     * @param {string} playerName - The name of the player.
     * @returns {DewPromise<StatsInfo>} A promise for the player's stats.
     */
     dew.getStats = function (playerName) {
         return dew.callMethod("stats", { playerName: playerName }, jsonResultMapping);
     }

    /**
     * Contains information about the player's stats
     *
     * @typedef {object} StatsInfo
     * @property {number[]} medals - The medals earned during this game.
     * @property {WeaponStats[]} weapons - Information about the weapons used during this game.
     * @see dew.getStats
     */

    /**
     * nformation about a weapon's use in the current game
     *
     * @typedef {object} WeaponStats
     * @property {number} BetrayalsWith - How many times the player used this weapon to betray their teammates.
     * @property {number} HeadshotsWith - How many headshots the player got with this weapon.
     * @property {number} KilledBy - How many times the player was killed by this weapon.
     * @property {number} Kills - How many kills the player got with this weapon.
     * @property {number} SuicidesWith - How many times the player killed themselves with this weapon.
     * @see StatsInfo
     */

    /**
     * Registers a callback to be run when an event occurs.
     *
     * @name dew.on
     * @function
     * @param {string} event - The name of the event to register a callback for (e.g. "show").
     * @param {EventCallback} callback - The callback to register.
     * @see event:show
     * @see event:hide
     * @see event:pong
     * @see event:console
     * @see event:mpevent
     * @see event:loadprogress
     */
    dew.on = function (event, callback) {
        registerEvent(event, callback);
    }

    /**
     * A callback function for responding to events.
     *
     * @callback EventCallback
     * @param {object} event - The event information.
     * @param {string} event.event - The name of the event.
     * @param {string} event.screen - The ID of the screen the event was sent to.
     * @param {string} event.data - Event-specific data.
     */

    /**
     * Fired after the current screen is shown.
     * The data passed to this event is the data that was passed to {@link dew.show}.
     *
     * @event show
     * @type {object}
     */

    /**
     * Fired after the current screen is hidden.
     *
     * @event hide
     * @type {object}
     */

    /**
     * Fired when a server replies to a [ping]{@link dew.ping}.
     * This can only be received while the screen is active.
     *
     * @event pong
     * @type {object}
     * @property {string} address - The IP address of the server.
     * @property {number} latency - The round-trip time of the ping in milliseconds.
     * @property {number} timestamp - A timestamp value representing when the ping was sent.
     */

    /**
     * Fired when a line is written to the in-game console.
     *
     * @event console
     * @type {object}
     * @property {string} line - The line that was written. **Make sure to escape this properly before displaying it.**
     */

    /**
     * Fired when a multiplayer event occurs that affects the local player.
     *
     * @event mpevent
     * @type {object}
     * @property {string} name - The internal name of the event.
     * @property {MPEventCategory} category - The event's category.
     * @property {MPEventAudience} audience - The audience that the event is intended for.
     */

    /**
     * Fired when the loading screen is active and the loading progress changes.
     *
     * This event is only sent to visible screens.
     *
     * @event loadprogress
     * @type {object}
     * @property {number} currentBytes - The current number of bytes that have been decompressed.
     * @property {number} totalBytes - The total number of bytes that need to be decompressed.
     */

    /**
     * Fired when a chat message is recieved.
     *
     * @event chat
     * @type {object}
     * @property {string} message - The chat message. **Make sure to escape this properly before displaying it.**
     * @property {string} sender - The username that sent this message. **Make sure to escape this properly before displaying it.**
     * @property {string} chatType - The type of message that was recieved. Can be "GLOBAL", "TEAM", "WHISPER", or "SERVER".
     * @property {number} teamIndex - The team index of the player. Not included when chatType is "SERVER".
     * @property {string} UID - The UID of the player. Not included when chatType is "SERVER".
     * @property {string} color - The hex color of the player's primary armor color. Not included when chatType is "SERVER".
     */

    /**
     * Fires when the scoreboard is updated
     *
     * @event scoreboard
     * @type {ScoreboardInfo}
     */

     /**
      * Fired when attempting to connect to a server and when the connection succeeds or fails.
      *
      * @event serverconnect
      * @type {object}
      * @property {boolean} connecting - True if the client is currently trying to connect to the server.
      * @property {boolean} success - True if the client successfully connected to the server. Will always be false if connecting is true.
      */
})();
