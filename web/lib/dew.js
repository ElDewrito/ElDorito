/**
 * Error codes.
 *
 * @readonly
 * @enum {number}
 */
DewError = {
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

    // Default success handler.
    function defaultSuccess(result) {
    }

    // Default failure handler.
    function defaultFailure(err) {
        console.error("Dew query \"" + err.method + "\" failed with code " + err.code + " (" + dew.getErrorName(err.code) + "): " + err.message);
    }

    // Default 1:1 result mapping.
    function defaultResultMapping(result) {
        return result;
    }

    // JSON result mapping.
    function jsonResultMapping(result) {
        return JSON.parse(result);
    }

    // Calls a native method.
    dew.callMethod = function (method, args, onSuccess, onFailure, resultMapping) {
        try {
            // Ensure default values
            onSuccess = onSuccess || defaultSuccess;
            onFailure = onFailure || defaultFailure;
            resultMapping = resultMapping || defaultResultMapping;
            args = args || {};

            // Ensure that we have a function to send queries to ED
            if (!window.dewQuery) {
                onFailure({
                    method: method,
                    code: DewError.UNSUPPORTED_METHOD,
                    message: "Unsupported method: window.dewQuery() is not available"
                });
                return;
            }

            // If args is a function, run it to get the actual arguments
            // This is useful in order to ensure that JS exceptions trigger onFailure
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
                    // Map the result string using the resultMapping function,
                    // calling onFailure if an error occurs in the process
                    var result;
                    try {
                        result = resultMapping(resultStr);
                    } catch (e) {
                        onFailure({
                            method: method,
                            code: DewError.JS_EXCEPTION,
                            message: e.message
                        });
                        return;
                    }
                    onSuccess(result);
                },
                onFailure: function (code, message) {
                    onFailure({
                        method: method,
                        code: code,
                        message: message
                    });
                }
            });
        } catch (e) {
            onFailure({
                method: method,
                code: DewError.JS_EXCEPTION,
                message: e.message
            });
        }
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
     * Some methods run asynchronously and accept success and failure callbacks.
     *
     * @namespace dew
     */

    /**
     * The onSuccess callback is called when an asynchronous method succeeds.
     *
     * @callback SuccessCallback
     * @param {*} result - The result value. The type of this depends on the method that was called.
     */

    /**
     * The onFailure callback is called when an asynchronous method fails to execute.
     *
     * @callback FailureCallback
     * @param {object} err - Information about the error that occurred.
     * @param {DewError} err.code - The internal error code.
     * @param {string} err.message - The error message.
     * @param {string} err.method - The name of the internal method that was called.
     */

    /**
     * (ASYNCHRONOUS) Retrieves the current version of ElDewrito.
     * 
     * @name dew.getVersion
     * @function
     * @param {SuccessCallback} [onSuccess] - The success callback. It will be passed the version string.
     * @param {FailureCallback} [onFailure] - The failure callback.
     */
    dew.getVersion = function (onSuccess, onFailure) {
        dew.callMethod("version", {}, onSuccess, onFailure);
    }

    /**
     * Requests to show a screen.
     * 
     * If the requested screen is still loading, it will not be shown until it finishes.
     * 
     * This will always send a [show]{@link event:show} event to the screen, even if it is already visible.
     * You can use this as a simple means of sending messages between screens.
     *
     * @name dew.show
     * @function
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
     * @name dew.hide
     * @function
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
     * @name dew.captureInput
     * @function
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
     * If the command does not run successfully, onFailure will be called with [DewError.COMMAND_FAILED]{@link DewError}.
     * The error message will be the command output.
     *
     * @name dew.command
     * @function
     * @param {string} command - The command and its arguments, separated by spaces.
     * @param {object} [options] - Additional options that control how the command should run.
     * @param {boolean} [options.internal=false] - If set to true, then internal commands can be executed.
     * @param {SuccessCallback} [onSuccess] - The success callback. It will be passed the string returned by the command.
     * @param {FailureCallback} [onFailure] - The failure callback. If the command fails with [DewError.COMMAND_FAILED]{@link DewError}, this will be called with the error message set to the command output.
     */
    dew.command = function (command, options, onSuccess, onFailure) {
        dew.callMethod("command", function () {
            options = options || {};
            return {
                command: command.toString(),
                internal: (typeof options.internal === "boolean") ? options.internal : false
            };
        }, onSuccess, onFailure);
    }

    /**
     * (ASYNCHRONOUS) Pings a server.
     * 
     * The screen will receive a [pong]{@link event:pong} event if the server responds.
     *
     * **This method is currently broken and will report ping times that are much higher than they should be.**
     *
     * @name dew.ping
     * @function
     * @param {string} address - The IPv4 address of the server to ping. Must not include a port number.
     * @param {SuccessCallback} [onSuccess] - The success callback. This will be called after the ping is sent, not when a response is received.
     * @param {FailureCallback} [onFailure] - The failure callback.
     * @see event:pong
     * @see dew.on
     */
    dew.ping = function (address, onSuccess, onFailure) {
        dew.callMethod("ping", function () {
            return {
                address: address.toString()
            };
        }, onSuccess, onFailure);
    }

    /**
     * (ASYNCHRONOUS) Gets info about the current map variant.
     * 
     * If map variant info is not available, onFailure will be called with [DewError.NOT_AVAILABLE]{@link DewError}.
     * 
     * @name dew.getMapVariantInfo
     * @function
     * @param {MapVariantInfoSuccessCallback} onSuccess - The success callback. It will be passed information about the map variant.
     * @param {FailureCallback} onFailure - The failure callback.
     * @see MapVariantInfoSuccessCallback
     */
    dew.getMapVariantInfo = function (onSuccess, onFailure) {
        dew.callMethod("mapVariantInfo", {}, onSuccess, onFailure, jsonResultMapping);
    }

    /**
     * Called when {@link dew.getMapVariantInfo} succeeds.
     *
     * @callback MapVariantInfoSuccessCallback
     * @param {object} info - The map variant info.
     * @param {string} info.name - The name. Can be empty.
     * @param {string} info.description - The description. Can be empty.
     * @param {string} info.author - The author. Can be empty.
     * @param {number} info.mapId - The map ID.
     * @see dew.getMapVariantInfo
     */

    /**
     * (ASYNCHRONOUS) Gets info about the current game variant.
     * 
     * If game variant info is not available, onFailure will be called with [DewError.NOT_AVAILABLE]{@link DewError}.
     * 
     * @name dew.getGameVariantInfo
     * @function
     * @param {GameVariantInfoSuccessCallback} onSuccess - The success callback. It will be passed information about the game variant.
     * @param {FailureCallback} onFailure - The failure callback.
     * @see GameVariantInfoSuccessCallback
     */
    dew.getGameVariantInfo = function (onSuccess, onFailure) {
        dew.callMethod("gameVariantInfo", {}, onSuccess, onFailure, jsonResultMapping);
    }

    /**
     * Called when {@link dew.getGameVariantInfo} succeeds.
     *
     * @callback GameVariantInfoSuccessCallback
     * @param {object} info - The game variant info.
     * @param {GameMode} info.mode - The base game mode.
     * @param {string} info.name - The name. Can be empty.
     * @param {string} info.description - The description. Can be empty.
     * @param {string} info.author - The author. Can be empty.
     * @param {boolean} info.teams - `true` if teams are enabled.
     * @param {number} info.timeLimit - The time limit in minutes (0 if unlimited).
     * @param {number} info.rounds - The number of rounds.
     * @param {number} info.scoreToWin - The score-to-win (-1 if unlimited).
     * @see dew.getGameVariantInfo
     */

    /**
     * (ASYNCHRONOUS) Gets a list of available console commands.
     * 
     * @param {CommandListSuccessCallback} onSuccess - The success callback. It will be passed an array of objects containing command info.
     * @param {FailureCallback} onFailure - The failure callback.
     * @see CommandListSuccessCallback
     */
    dew.getCommands = function (onSuccess, onFailure) {
        dew.callMethod("commands", {}, onSuccess, onFailure, jsonResultMapping);
    }

    /**
     * Called when {@link dew.getCommands} succeeds.
     *
     * @callback CommandListSuccessCallback
     * @param {object[]} commands - An array of available commands.
     * @param {CommandType} commands[].type - The type of the command or variable.
     * @param {string} commands[].module - The module name.
     * @param {string} commands[].name - The name of the command or variable. This includes the module prefix.
     * @param {string} commands[].shortName - The short name of the command or variable.
     * @param {string} commands[].description - A description to display in a help listing.
     * @param {*} commands[].value - The current value of the variable. For commands, this will be `null`.
     * @param {*} commands[].defaultValue - The default value of the variable. For commands, this will be `null`.
     * @param {*} commands[].minValue - The minimum value of the variable. For string variables and commands, this will be `null`.
     * @param {*} commands[].maxValue - The maximum value of the variable. For string variables and commands, this will be `null`.
     * @param {boolean} commands[].replicated - `true` if the variable should be synchronized to clients.
     * @param {boolean} commands[].archived - `true` if the variable should be saved when the config file is written.
     * @param {boolean} commands[].hidden - `true` if the command or variable should be omitted from a help listing.
     * @param {boolean} commands[].hostOnly - `true` if the command or variable can only be used by the game host.
     * @param {boolean} commands[].hideValue - `true` if the variable's value should be omitted from a help listing.
     * @param {boolean} commands[].internal - `true` if the command or variable can only be set internally.
     * @param {string[]} commands[].arguments - A list of arguments for the command. Each string will contain a value name, a space, and then a description. For variables, this will be empty.
     * @see dew.getCommands
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
     * Gets the name of the DewError value corresponding to an error code.
     *
     * @name dew.getErrorName
     * @function
     * @param {DewError} code - The error code.
     * @returns {string} The error code's name.
     */
    dew.getErrorName = function (code) {
        for (var error in DewError) {
            if (DewError.hasOwnProperty(error) && DewError[error] === code) {
                return error;
            }
        }
        return code.toString();
    }
})();