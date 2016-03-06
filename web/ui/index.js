(function(){
	var screenJsonUrl = "dew://screens/screens.json";

	var activeScreen = null;
	var screenElements = {};

	if (typeof window.dew === "undefined") {
		window.dew = {};
	}

	// Creates a screen from a screen data object.
	dew.createScreen = function(screen) {
		if (screen.id in screenElements) {
			return;
		}
		screenElements[screen.id] =
			$("<iframe></iframe>", {
				"class": "screen-frame",
				src: screen.url
			})
			.hide()
			.appendTo($("#content"));
	}

	// Changes the currently-active screen.
	dew.activateScreen = function(id) {
		if (activeScreen) {
			activeScreen.hide();
		}
		activeScreen = (id !== null) ? screenElements[id] : null;
		if (activeScreen) {
			activeScreen.show();
		}
	}

	$(window).load(function() {
		// Request the list of screens as JSON data
		$.getJSON(screenJsonUrl, function(data) {
			// Create each screen
			if (data.screens) {
				$.each(data.screens, function(key, val) {
					dew.createScreen(val);
				});
			}

			// If an initial screen is set, activate it
			if (data.initialScreen) {
				dew.activateScreen(data.initialScreen);
			}
		});
	});
})();