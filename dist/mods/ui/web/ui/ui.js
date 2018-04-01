(function () {
    var screenJsonUrl = "dew://screens/screens.json";
    var jsonQuery = null;
    var screens = {};

    // Screen states.
    var ScreenState = {
        HIDDEN: 0,  // The screen is hidden.
        WAITING: 1, // The screen is loading and should be shown once it is finished.
        VISIBLE: 2  // The screen is visible.
    };

    window.ui = window.ui || {};

    // Shows the web overlay.
    function showOverlay() {
        return dew.callMethod("show");
    }

    // Hides the web overlay.
    function hideOverlay() {
        return dew.callMethod("hide");
    }

    // Controls whether the web overlay captures input.
    function captureInput(capture, capturePointer=false) {
        return dew.callMethod("captureInput", { 
			capture: !!capture, 
			capturePointer: !!capturePointer 
		});
    }

    // Updates the overlay after a screen has been shown, hidden, or changed.
    function updateOverlay() {
        // Count the number of visible screens,
        // determine whether input should be captured,
        // and find the topmost screen that captures input
        var visible = 0;
        var capture = false;
		var capturePointer = false;
        var topmostZ = 0;
        var topmostScreen = null;
        $.each(screens, function (id, screen) {
            if (screen.state === ScreenState.VISIBLE) {
                visible++;
				if(screen.capturePointer)
					capturePointer = true;
                if (screen.captureInput)
					capture = true;
				if(screen.captureInput || screen.capturePointer) {
                    if (!topmostScreen || screen.zIndex > topmostZ) {
                        topmostScreen = screen;
                        topmostZ = screen.zIndex;
                    }
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
        captureInput(capture, capturePointer);
        if (topmostScreen) {
            topmostScreen.selector.focus();
        }
    }

    // Finds a screen by ID.
    function findScreen(id) {
        if (typeof id !== "undefined" && id !== null && screens.hasOwnProperty(id)) {
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
        if(screen.constructor === String)
            screen = findScreen(screen);

        if (screen === null || screen.selector === null || !screen.loaded) {
            return;
        }
        screen.selector[0].contentWindow.postMessage({
            event: event,
            screen: screen.id,
            data: data || {}
        }, "*");
    }

    // Creates a frame for a screen.
    function createFrame(screen) {
        // Create an iframe
        var selector = $("<iframe></iframe>", {
                "class": "screen-frame"
            })
		    .hide()
            .css("z-index", screen.zIndex)
            .css("pointer-events", screen.captureInput ? "auto" : "none")
		    .appendTo($("#content"));

        // Once the screen loads, show it if it should be visible
        selector.on("load", function (event) {
            screen.loaded = true;
            if (screen.state === ScreenState.WAITING) {
                showScreen(screen, true);
            }
        });

        return selector;
    }

    // Reloads a screen.
    function reloadScreen(screen) {
        screen.loaded = false;
        screen.loadStartTime = Date.now();

        // If the screen is visible, hide it and put it in the waiting state
        if (screen.state === ScreenState.WAITING || screen.state === ScreenState.VISIBLE) {
            showScreen(screen, false);
            screen.state = ScreenState.WAITING;
        }

        // Send a HEAD request to the screen's URL to make sure that it can be accessed
        $.ajax(screen.url, {
            method: "HEAD",
            crossDomain: false, // dew://ui/ is whitelisted to bypass the same-origin restriction for HTTP
            cache: false
        }).done(function (data, textStatus, jqXHR) {
            // URL is accessible - navigate to it
            screen.selector[0].src = screen.url;
        }).fail(function (jqXHR, textStatus, errorThrown) {
            // URL is not accessible - try the fallback URL
            console.error("Failed to access " + screen.url + " - trying fallback");
            if (screen.fallbackUrl) {
                screen.selector[0].src = screen.fallbackUrl;
            }
        });
    }

    // Shows or hides a screen.
    function showScreen(screen, show) {
        if (screen.selector === null) {
            return;
        }
        if (show) {
            screen.state = ScreenState.VISIBLE;
            screen.selector.show();
            notifyScreen(screen, "show", screen.data || {});
        } else if (screen.state === ScreenState.WAITING || screen.state === ScreenState.VISIBLE) {
            screen.state = ScreenState.HIDDEN;
            screen.selector.hide();
            notifyScreen(screen, "hide", {});
        }
        screen.data = {};
        updateOverlay();
    }

    // Changes a screen's input capture state.
    function setCaptureState(screen, capture) {
        screen.captureInput = capture;
		screen.capturePointer = false; //no matter what we will remove pointer capture
        if (screen.selector !== null) {
            screen.selector.css("pointer-events", capture ? "auto" : "none");
        }
        updateOverlay();
    }
	
	//Change a screen's pointer capture only
	function setPointerCapture(screen, capture) {
		screen.capturePointer = capture;
		if (screen.selector !== null) {
            screen.selector.css("pointer-events", capture ? "auto" : "none");
        }
        updateOverlay();
	}

    // Sends an event notification.
    ui.notify = function (event, data, broadcast, fromDew) {
        $.each(screens, function (id, screen) {
            if (broadcast || screen.state === ScreenState.VISIBLE) {
                notifyScreen(screen, event, data);
            }
        });
    }

    ui.notifyScreen = notifyScreen;

    // Creates a screen from a screen data object.
    ui.createScreen = function (data) {
        if (data.id in screens) {
            return;
        }

        // Register the screen
        var screen = {
            id: data.id,
            url: null,
            fallbackUrl: data.fallbackUrl || null,
            captureInput: data.captureInput || false,
			capturePointer: data.capturePointer || false,
            zIndex: data.zIndex || 0,
            state: ScreenState.HIDDEN,
            loaded: false,
            loadStartTime: Date.now(),
            data: {},
            selector: null
        };
        screen.selector = createFrame(screen);
        screens[screen.id] = screen;

        // Load its frame
        if ("url" in data) {
            // Just use the URL in the data
            screen.url = data.url;
            reloadScreen(screen);
        } else if ("var" in data) {
            // Get the screen URL from a variable
            dew.command(data.var, { internal: true }).then(function (value) {
                if (value !== "") {
                    screen.url = value;
                    reloadScreen(screen);
                }
            });
        }
    }

    // Requests to show a screen.
    ui.requestScreen = function (id, data) {
        if (jsonQuery) {
            // The screen will be requested once the screen JSON loads
            jsonQuery = jsonQuery.done(function () {
                ui.requestScreen(id, data);
            });
            return;
        }
        var screen = findScreen(id);
        if (screen === null) {
            console.error("Requested to show screen \"" + id + "\", but it isn't defined!");
            return;
        }
        screen.data = data || {};
        if (screen.loaded) {
            // Show the screen immediately
            showScreen(screen, true);
        } else {
            // The screen will be shown once it finishes loading
            screen.state = ScreenState.WAITING;
            // If the screen has taken more than 5 seconds to load then reload the screen
            // This fixes a rare bug where screens get stuck loading indefinitely
            if((Date.now() - screen.loadStartTime) > 5000) {
                console.error("Screen \"" + id + "\" has taken longer than 5 seconds to load. Reloading!");
                reloadScreen(screen);
            }
        }
    }

    // Requests to hide a screen.
    ui.hideScreen = function (id) {
        var screen = findScreen(id);
        if (screen === null) {
            console.error("Requested to hide screen \"" + id + "\", but it isn't defined!");
            return;
        }
        showScreen(screen, false);
    }

    // Reloads all screens.
    ui.reload = function () {
        $.each(screens, function (id, screen) {
            reloadScreen(screen);
        });
    }

    ui.sendControllerInput = function (input) {
        var topmostScreen;
        $.each(screens, function (id, screen) {
            if (screen.state === ScreenState.VISIBLE && screen.captureInput)
                if (!topmostScreen || screen.zIndex > topmostScreen.zIndex)
                    topmostScreen = screen;
        });

        if(topmostScreen)
            notifyScreen(topmostScreen, "controllerinput", input);
    }

    ui.showToast = function(toast) {
        let testContainer = document.getElementById('toast_container');

        let toastElement = document.createElement('div');
        toastElement.classList.add('toast');
        toastElement.innerHTML = `
            <div class="toast-content">${toast.body}</div>
        `
        testContainer.prepend(toastElement);
        requestAnimationFrame(function () {
            toastElement.classList.add('toast-show');
        });

        setTimeout(function () {
            toastElement.classList.remove('toast-show');
            setTimeout(function () {
                toastElement.remove();
            }, 3000);        
        }, 3000);
    }

    // Register the message handler.
    window.addEventListener("message", function (event) {
        // Get the screen that sent the event
        // If the event was sent by the UI layer, then a null screen is possible
        var screen = null;
        if (event.source !== window) {
            // Event was sent from a screen
            screen = findScreenFromWindow(event.source);
            if (screen === null) {
                return;
            }
        }

        // Make sure the event data is valid
        var eventData = event.data;
        if (eventData === null || typeof eventData !== "object" || typeof eventData.message !== "string" || typeof eventData.data !== "object") {
            return;
        }

        // Process the message
        var data = eventData.data;
        switch (eventData.message) {
            case "show":
                // Requested to show a screen
                var showId = (typeof data.screen === "string") ? data.screen : (screen ? screen.id : null);
                if (showId) {
                    ui.requestScreen(showId, data.data);
                }
                break;
            case "hide":
                // Requested to hide a screen
                var hideId = (typeof data.screen === "string") ? data.screen : (screen ? screen.id : null);
                if (hideId) {
                    ui.hideScreen(hideId);
                }
                break;
            case "captureInput":
                // Requested to change capture state
                if (screen && "capture" in data) {
                    setCaptureState(screen, !!data.capture);
                }
                break;
			case "capturePointer":
				//Change pointer capture only
				if (screen && "capture" in data) {
                    setPointerCapture(screen, !!data.capture);
                }
                break;
            case "toast":
                ui.showToast(data);
                break;
			default:
				var message = (typeof eventData.message === "string") ? eventData.message : (screen ? screen.id : null);
				if(message){
					ui.notify(message, data.data, true, false);
				}
				break;
        }
    }, false);

    $(window).load(function () {
        // Request the list of screens as JSON data and create each one
        jsonQuery = $.getJSON(screenJsonUrl, function (data) {
            if (data.screens) {
                $.each(data.screens, function (index, screen) {
                    ui.createScreen(screen);
                });
            }
            jsonQuery = null;
        });
    });
})();