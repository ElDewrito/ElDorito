/**
 * Error codes.
 *
 * @readonly
 * @enum {number}
 */
DewError = {
    OK: 0,
    UNKNOWN_ERROR: 1,
    JS_EXCEPTION: 2,
    BAD_QUERY: 3,
    UNSUPPORTED_METHOD: 4,
    INVALID_ARGUMENT: 5,
    NETWORK_ERROR: 6
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

    // Calls a native method.
    function callMethod(method, args, onSuccess, onFailure) {
        try {
            // Ensure default values
            onSuccess = onSuccess || defaultSuccess;
            onFailure = onFailure || defaultFailure;
            args = args || {};

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
                onSuccess: onSuccess,
                onFailure: function (code, message) {
                    onFailure({
                        method: method,
                        code: code,
                        message: message
                    });
                }
            });
        } catch (e) {
            onFailure(DewError.JS_EXCEPTION, e.message);
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
            if (data.event == name) {
                callback(data);
            }
        }, false);
    }

    // Posts a message to the UI.
    function postUiMessage(message) {
        if (window.parent) {
            window.parent.postMessage(message, "*");
        }
    }

    // Notify the UI once the screen loads.
    $(window).load(function () {
        postUiMessage({ loaded: true });
    });

    /**
	 * <p>Methods for interfacing with ElDewrito.</p>
	 * <p>Some methods run asynchronously and accept success and failure callbacks.
	 * For performance reasons, asynchronous methods that are run while the web overlay is closed will be delayed until it is open again.</p>
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
	 */

    /**
	 * (ASYNCHRONOUS) Hides the web overlay.
	 * While the web overlay is hidden, other dew methods will not run until the overlay is re-shown by the user.
	 *
	 * @name dew.hide
	 * @function
	 * @param {SuccessCallback} [onSuccess] - The success callback.
	 * @param {FailureCallback} [onFailure] - The failure callback.
	 */
    dew.hide = function (onSuccess, onFailure) {
        callMethod("hide", {}, onSuccess, onFailure);
    }

    /**
	 * (ASYNCHRONOUS) Runs a console command.
	 * This will always succeed unless an error occurs while calling the method itself.
	 *
	 * @name dew.console
	 * @function
	 * @param {string} command - The command and its arguments, separated by spaces.
	 * @param {object} [options] - Additional options that control how the command should run.
	 * @param {boolean} [options.internal=false] - If set to true, then internal commands can be executed.
	 * @param {SuccessCallback} [onSuccess] - The success callback. It will be passed the string returned by the command.
	 * @param {FailureCallback} [onFailure] - The failure callback.
	 */
    dew.command = function (command, options, onSuccess, onFailure) {
        callMethod("command", function () {
            options = options || {};
            return {
                command: command.toString(),
                internal: (typeof options.internal === "boolean") ? options.internal : false
            };
        }, onSuccess, onFailure);
    }

    /**
	 * (ASYNCHRONOUS) Pings a server.
	 * The screen will receive a [pong]{@link event:pong} event if the server responds.
	 *
	 * @name dew.ping
	 * @function
	 * @param {string} address - The IP address of the server to ping. Must not include a port number.
	 * @param {SuccessCallback} [onSuccess] - The success callback. This will be called after the ping is sent, not when a response is received.
	 * @param {FailureCallback} [onFailure] - The failure callback.
	 * @see event:pong
	 * @see dew.on
	 */
    dew.ping = function (address, onSuccess, onFailure) {
        callMethod("ping", function () {
            return {
                address: address.toString()
            };
        }, onSuccess, onFailure);
    }

    /**
	 * Registers a callback to be run when an event occurs.
	 *
	 * @name dew.on
	 * @function
	 * @param {string} event - The name of the event to register a callback for.
	 * @param {EventCallback} callback - The callback to register.
	 * @see event:hide
	 * @see event:show
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
	 * @param {string} event.data - Event-specific data.
	 */

    /**
	 * Fired when the current screen is made active and before it is shown.
	 * The data passed to this event is the data that was passed to {@link dew.requestScreen}.
	 *
	 * @event activate
	 * @type {object}
	 */

    /**
	 * Fired after the current screen is hidden, either because the web overlay was hidden or because another screen is being made active.
	 *
	 * @event hide
	 * @type {object}
	 */

    /**
	 * Fired after the current screen is shown, either because the web overlay was shown or because the screen was just made active.
	 *
	 * @event show
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
	 * Requests a different screen to be shown.
	 * This will hide the current screen and then show the new one.
	 *
	 * @name dew.requestScreen
	 * @function
	 * @param {string} id - The ID of the screen to request.
	 * @param {object} [data] - Data to pass to the screen's [activate]{@link event:activate} event.
	 */
    dew.requestScreen = function (id, data) {
        postUiMessage({ requestScreen: id, data: data });
    }

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
            if (DewError.hasOwnProperty(error) && DewError[error] == code) {
                return error;
            }
        }
        return code.toString();
    }
})();