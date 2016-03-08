(function () {
    var screenJsonUrl = "dew://screens/screens.json";
    var screens = {};

    window.ui = window.ui || {};

    // Shows the web overlay.
    function showOverlay() {
        dew.callMethod("show", {});
    }

    // Hides the web overlay.
    function hideOverlay() {
        dew.callMethod("hide", {});
    }

    // Controls whether the web overlay captures input.
    function captureInput(capture) {
        dew.callMethod("captureInput", { capture: !!capture });
    }

    // Updates the overlay after a screen has been shown, hidden, or changed.
    function updateOverlay() {
        // Count the number of visible screens and determine whether input should be captured
        var visible = 0;
        var capture = false;
        $.each(screens, function (id, screen) {
            if (screen.visible && screen.loaded) {
                visible++;
                if (screen.captureInput) {
                    capture = true;
                }
            }
        });

        // If there's only one screen, show the overlay, and if there are none, hide it
        if (visible > 0) {
            showOverlay();
        } else if (visible === 0) {
            hideOverlay();
        }

        // Capture input accordingly
        captureInput(capture);
    }

    // Finds a screen by ID.
    function findScreen(id) {
        if (id !== null && screens.hasOwnProperty(id)) {
            return screens[id];
        }
        return null;
    }

    // Finds a screen from a window handle.
    function findScreenFromWindow(window) {
        for (var id in screens) {
            if (screens.hasOwnProperty(id)) {
                var screen = screens[id];
                if (screen.selector !== null && screen.selector[0].contentWindow === window) {
                    return screen;
                }
            }
        }
        return null;
    }

    // Sends an event notification to a screen.
    function notifyScreen(screen, event, data) {
        if (screen === null || screen.selector === null || !screen.loaded) {
            return;
        }
        screen.selector[0].contentWindow.postMessage({
            event: event,
            data: data || {}
        }, "*");
    }

    // Creates the frame for a screen once its URL has been determined.
    function createFrame(screen) {
        if (screen.selector !== null) {
            return;
        }
        screen.selector = $("<iframe></iframe>", {
                "class": "screen-frame",
                src: screen.url
            })
		    .hide()
            .css("z-index", screen.zIndex)
            .css("pointer-events", screen.captureInput ? "auto" : "none")
		    .appendTo($("#content"));
    }

    // Shows or hides a screen.
    function showScreen(screen, show) {
        if (screen.selector === null) {
            return;
        }
        if (show) {
            screen.visible = true;
            screen.selector.show();
            notifyScreen(screen, "show", screen.data);
        } else if (screen.visible) {
            screen.visible = false;
            screen.selector.hide();
            notifyScreen(screen, "hide", {});
        }
        updateOverlay();
    }

    // Changes a screen's input capture state.
    function setCaptureState(screen, capture) {
        screen.captureInput = capture;
        if (screen.selector !== null) {
            screen.selector.css("pointer-events", capture ? "auto" : "none");
        }
        updateOverlay();
    }

    // Sends an event notification.
    ui.notify = function (event, data, broadcast, fromDew) {
        $.each(screens, function (id, screen) {
            if (broadcast || (screen.visible && screen.loaded)) {
                notifyScreen(screen, event, data);
            }
        });
    }

    // Creates a screen from a screen data object.
    ui.createScreen = function (data) {
        if (data.id in screens) {
            return;
        }

        // Register the screen
        var screen = {
            id: data.id,
            url: null,
            captureInput: data.captureInput || false,
            zIndex: data.zIndex || 0,
            visible: false,
            loaded: false,
            data: {},
            selector: null
        };
        screens[screen.id] = screen;

        // Load its frame
        if ("url" in data) {
            // Just use the URL in the data
            screen.url = data.url;
            createFrame(screen);
        } else if ("var" in data) {
            // Get the screen URL from a variable
            dew.command(data.var, { internal: true }, function (value) {
                if (value !== "") {
                    screen.url = value;
                    createFrame(screen);
                }
            });
        }
    }

    // Requests to show a screen.
    ui.requestScreen = function (id, data) {
        if (typeof id === "undefined" || id === null || !screens.hasOwnProperty(id)) {
            return;
        }
        var screen = screens[id];
        screen.data = data || {};
        if (screen.loaded) {
            // Show the screen immediately
            showScreen(screen, true);
        } else {
            // The screen will be shown once it finishes loading
            screen.visible = true;
        }
    }

    // Register the message handler.
    window.addEventListener("message", function (event) {
        var screen = findScreenFromWindow(event.source);
        if (screen === null) {
            return; // Only accept messages from screens
        }
        var eventData = event.data;
        if (eventData === null || typeof eventData !== "object" || typeof eventData.message !== "string" || typeof eventData.data !== "object") {
            return;
        }
        var data = eventData.data;
        switch (eventData.message) {
            case "show":
                // Requested to show a screen
                var showId = (typeof data.screen === "string") ? data.screen : screen.id;
                ui.requestScreen(showId, data.data);
                break;
            case "hide":
                // Requested to hide a screen
                var hideScreen = (typeof data.screen === "string") ? findScreen(data.screen) : screen;
                if (hideScreen !== null) {
                    showScreen(hideScreen, false);
                }
                break;
            case "loaded":
                // A screen loaded
                if (!screen.loaded) {
                    screen.loaded = true;

                    // If the screen should be visible, then show it
                    if (screen.visible) {
                        showScreen(screen, true);
                    }
                }
                break;
            case "captureInput":
                // Requested to change capture state
                if ("capture" in data) {
                    setCaptureState(screen, !!data.capture);
                }
                break;
        }
    }, false);

    $(window).load(function () {
        // Request the list of screens as JSON data and create each one
        $.getJSON(screenJsonUrl, function (data) {
            if (data.screens) {
                $.each(data.screens, function (index, screen) {
                    ui.createScreen(screen);
                });
            }
        });
    });
})();