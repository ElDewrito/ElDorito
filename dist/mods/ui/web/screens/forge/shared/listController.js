function makeListWidgetController(name, settings, dataSource) {
	var selectedIndex = 0;
	var dataSource = dataSource;
	var settings = settings;
	var callbacks = {};
	var emitter = new Emitter();

	return {
		on: (type, callback) => emitter.on(type, callback),
		setSelectedIndex: (index) => {
			selectedIndex = index;
			emitter.emit('selectedIndexChanged', { index: selectedIndex, userInput: false });
		},
		selectedIndex: () => selectedIndex,
		navigate: navigate
	}

	function constrainSelection() {	
		var count = dataSource.count();
		if(selectedIndex >= count) {
			if(settings.wrapAround)
				selectedIndex = 0;
			else
				selectedIndex = count -1;
		}
		if(selectedIndex < 0) {
			if(settings.wrapAround)
				selectedIndex = count -1;
			else
				selectedIndex = 0;
		}
	}

	function navigate(dir) {
		var prevSelectedIndex = selectedIndex;
		selectedIndex += dir;
		constrainSelection();

		if(prevSelectedIndex != selectedIndex) {
			emitter.emit('selectedIndexChanged', { index: selectedIndex, userInput: true });
		}
	}
}