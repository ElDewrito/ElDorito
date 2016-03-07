(function () {
    var screenJsonUrl = "dew://screens/screens.json";
    var screenJson = {};

    var activeScreen = null;
    var screenElements = {};
    var isVisible = false;

    window.ui = window.ui || {};

    function notifyScreen(screen, event, data) {
        screen[0].contentWindow.postMessage({
            event: event,
            data: data
        }, "*");
    }

    function findScreen(window) {
        for (var id in screenElements) {
            if (screenElements.hasOwnProperty(id) && screenElements[id][0].contentWindow === window) {
                return id;
            }
        }
        return null;
    }

    function createScreenFrame(url) {
        return $("<iframe></iframe>", {
            "class": "screen-frame",
            src: url
        })
		.hide()
		.appendTo($("#content"));
    }

    // Sends an event notification.
    ui.notify = function (event, data, broadcast, fromDew) {
        switch (event) {
            case "show":
                if (fromDew) {
                    isVisible = true;
                } else if (!isVisible) {
                    return; // Don't send show events if the overlay isn't visible
                }
                break;
            case "hide":
                if (fromDew) {
                    isVisible = false;
                }
                break;
        }
        if (broadcast) {
            // Send to all screens
            for (var id in screenElements) {
                if (screenElements.hasOwnProperty(id)) {
                    notifyScreen(screenElements[id], event, data);
                }
            }
        } else if (activeScreen) {
            // Only send to the active screen
            notifyScreen(activeScreen, event, data);
        }
    }

    // Creates a screen from a screen data object.
    ui.createScreen = function (screen) {
        if (screen.id in screenElements) {
            return;
        }
        if ("url" in screen) {
            screenElements[screen.id] = createScreenFrame(screen.url);
        } else if ("var" in screen) {
            // Get the screen URL from a variable
            dew.command(screen.var, { internal: true }, function (value) {
                if (value != "") {
                    screenElements[screen.id] = createScreenFrame(value);
                }
            });
        }
    }

    // Changes the currently-active screen.
    ui.activateScreen = function (id, data) {
        if (!screenElements.hasOwnProperty(id)) {
            return false;
        }
        if (screenElements[id] === activeScreen) {
            return true;
        }
        if (activeScreen) {
            ui.notify("hide", {}, false, false);
            activeScreen.hide();
        }
        if (id === null) {
            activeScreen = null;
            return true;
        }
        activeScreen = screenElements[id];
        if (!activeScreen) {
            return false;
        }
        ui.notify("activate", data || {}, false, false);
        activeScreen.show();
        ui.notify("show", {}, false, false);
        return true;
    }

    // Register the message handler
    window.addEventListener("message", function (event) {
        var data = event.data;
        if (typeof data !== "object") {
            return;
        }
        if (typeof data.requestScreen === "string") {
            // A new screen was requested
            ui.activateScreen(data.requestScreen, data.data);
        } else if (data.loaded) {
            // A screen loaded - find which one it is, and if it's the initial screen, activate it
            var screenId = findScreen(event.source);
            if (screenId !== null && screenJson && typeof screenJson.initialScreen === "object" && screenJson.initialScreen.id === screenId) {
                ui.activateScreen(screenId, screenJson.initialScreen.data);
            }
        }
    }, false);

    $(window).load(function () {
        // Request the list of screens as JSON data
        $.getJSON(screenJsonUrl, function (data) {
            screenJson = data;

            // Create each screen
            // dew.js will send us a notification once a screen loads,
            // and this is used to ensure that the initial screen is only activated once it's ready
            if (data.screens) {
                $.each(data.screens, function (key, val) {
                    ui.createScreen(val);
                });
            }
        });
    });
})();