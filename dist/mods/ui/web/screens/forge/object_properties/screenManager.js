function makeScreenManager(screens) {
	var screenStack = [];
	var screens = screens;
	var callbacks = {};

	return {
		clear: () => { 
			while(screenStack.length)
				screenStack.pop();
		},
		currentScreen: () => screenStack[screenStack.length-1],
		pushedScreens: () => {
			return screenStack;
		},
		push: (name) => {
			var screen = screens[name];
			if(!screen) {
				console.error('screen not found.');
				return;
			}
			if(screenStack.length) {
				var current = screenStack[screenStack.length-1];
				current.deactivate();
			}

			screenStack.push(screen);
			screen.activate();

			emit('screenChange');
		},
		pop: () => {
			if(!screenStack.length)
				return;

			var old = screenStack.pop();
			old.deactivate();

			if(screenStack.length) {
				var current = screenStack[screenStack.length-1];
				current.activate();
			}
			else {
				dew.hide();
				return;
			}

			emit('screenChange');
		},
		on: (type, callback) => {
			if(!callbacks[type]) callbacks[type] = [];
				callbacks[type].push(callback);
		}
	}

	function emit(type, data) {
		var cbs = callbacks[type];
		if(!cbs)
			return;

		for(i = 0; i < cbs.length; i++) {
			cbs[i](data);
		}
	}
}