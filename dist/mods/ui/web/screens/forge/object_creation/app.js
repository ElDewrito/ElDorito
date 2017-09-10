
(function() {
	var categoryTreeElement = document.getElementById('category_tree');
	var objectListElement = document.getElementById('object_list');
  var tabsElement = document.getElementById('main_tabs');
	var budget = {};

  	var parser = new DOMParser();
  	var doc = parser.parseFromString(document.getElementById('item_data').innerHTML, "application/xml");
  	var treeList = makeTreeList(categoryTreeElement, doc);

  	var objectListController = makeListWidgetController('object_list', {wrapAround:true}, {
  		count: () => objectListElement.querySelectorAll('li').length
  	})
  	var tabListController = makeListWidgetController('tab_list', {}, {
  		count: () => 3
  	});

  	objectListController.on('selectedIndexChanged', function({index, userInput}) {
  		var items = objectListElement.querySelectorAll('li');
  		for(var i = 0; i < items.length; i++) {
  			items[i].classList.remove('selected');
  		}
  		var item = objectListElement.querySelector(`li:nth-of-type(${index+1})`);
  		if(item) {
  			item.classList.add('selected');
        if(userInput)
  			   item.scrollIntoView(false);
  		}
  		updateHelpText();
  	});

    categoryTreeElement.addEventListener('click', function(e) {
        handleUiInput(0);
    }, true);

    tabsElement.addEventListener('click', function(e) {
          if(e.target.className.indexOf('tab-header') == -1)
            return;
          if(screenManager.currentScreen().name !== 'main')
           screenManager.pop();
          dew.command('Game.PlaySound 0xb00');
          changeTab(e.target.getAttribute('data-target'));
    }, true);

    objectListElement.addEventListener('mouseover', function(e) {
        if(e.target.nodeName == 'LI') {
          var nodes = Array.prototype.slice.call(objectListElement.children);
            objectListController.setSelectedIndex(nodes.indexOf(e.target));
        }    
    }, true);

    objectListElement.addEventListener('click', function(e) {
          handleUiInput(0);
    }, true);

  	var screenManager = makeScreenManager({
  		main: {
  			name: 'main',
  			activate: function() {
  				categoryTreeElement.classList.remove('hidden');
  				updateHelpText();
  				
  			},
  			deactivate: function() {
  				categoryTreeElement.classList.add('hidden');
  			},
  			handleInput: function(uiButtonCode) {

  				switch(uiButtonCode) {
  					case 1:
  						dew.command('Game.PlaySound 0xb01');
	  					if(treeList.atRoot()) {
	  						dew.hide();
	  						return;
	  					}
  					break;
  					case 0:
  						dew.command('Game.PlaySound 0xb00');
  					break;
  					case 8:
  					case 9:
  						dew.command('Game.PlaySound 0xafe');
  					break;
  					case 4:
  					case 5:
  						tabListController.navigate(uiButtonCode == 5 ? -1 : 1);
  						dew.command('Game.PlaySound 0xaff');
  					break;
  					case 10:
  					case 11:
  						tabListController.navigate(uiButtonCode == 10 ? -1 : 1);
  						dew.command('Game.PlaySound 0xaff');
  					break;
  				}
  				
  				treeList.handleInput(uiButtonCode);
  			}
  		},
  		object_list: {
  			name: 'object_list',
  			activate: function() {
  				objectListController.setSelectedIndex(0);
  				objectListElement.classList.remove('hidden');
  				objectListElement.classList.add('active');
  				updateHelpText();
  			},
  			deactivate: function() {
		  		var items = objectListElement.querySelectorAll('li');
		  		for(var i = 0; i < items.length; i++) {
		  			items[i].classList.remove('selected');
		  		}
		  		objectListElement.classList.add('hidden');
  			},
  			handleInput: function(uiButtonCode) {
  				switch(uiButtonCode) {
  					case 1:
  						screenManager.pop();
  						dew.command('Game.PlaySound 0xb01');
  						break;
  					case 0:
  						dew.command('Game.PlaySound 0xb00');
  						var selected = objectListElement.querySelector('li.selected');
  						if(selected) {
	  						var tagIndex = selected.getAttribute('data-tagindex');
	  						dew.command('forge.spawnitem ' + tagIndex).then(function() {});
	  						dew.hide();
  						}
  					break;
  					case 10:
  					case 11:
                if(screenManager.currentScreen().name !== 'main')
                  screenManager.pop();
              tabListController.navigate(uiButtonCode == 10 ? -1 : 1);
              dew.command('Game.PlaySound 0xaff');
            break;
  					case 4:
  					case 5:
              if(screenManager.currentScreen().name !== 'main')
  						    screenManager.pop();
  						tabListController.navigate(uiButtonCode == 5 ? -1 : 1);
  						dew.command('Game.PlaySound 0xaff');
  					break;
  					case 8:
  					case 9:
  						objectListController.navigate(uiButtonCode == 9 ? 1 : -1);
  						dew.command('Game.PlaySound 0xafe');
  					break;
  				}
  			}
  		}
  	})

  	screenManager.push('main');

  	function updateHelpText() {
  		var currentScreen = screenManager.currentScreen();

  		var helpText = undefined;
  		switch(currentScreen.name) {
  			case 'main':
  				var selected = helpText = categoryTreeElement.querySelector('li.selected');
  				if(selected) {
  					helpText = selected.getAttribute('data-help');
  				}
  			break;
  			case 'object_list':
  				var selected = helpText = objectListElement.querySelector('li.selected');
  				if(selected) {
  					helpText = selected.getAttribute('data-help');
  				}			
  			break;
  		}	

  		var helpBoxElem = document.getElementById('help_text');
  		helpBoxElem.innerHTML = '';
  		if(helpText != 'undefined') {
  			helpBoxElem.innerHTML = helpText;
  		}
  	}

  	function getNodeHelp(node) {
  		for(var n = node.firstChild; n; n = n.nextSibling) {
  			if(n.nodeName === 'help') {
  				return n.textContent;
  			}
  		}
  	}

  	function renderObjectList(node) {
  		var html = '';
  		for(var x = node.firstChild; x; x = x.nextSibling) {
  			if(x.nodeName === 'item') {
  				var name = x.getAttribute('name');
  				var tagindex = parseInt(x.getAttribute('tagindex'));

  				var itemBudget = budget[tagindex];
          var remainingItems = 255;

          if(itemBudget) {
            remainingItems = itemBudget.max_allowed - itemBudget.count_on_map;
          }
  				
  				var help = getNodeHelp(x);

  				html += `
	  			<li data-tagindex="${x.getAttribute('tagindex')}" data-help="${help}">
	  				<span class="title">${name}</span>
	  				<span class="quota">${remainingItems}</span>
	  			</li>`;
  			}
  		}
  		objectListElement.innerHTML = html;
  	}

  	function changeTab(id) {
  		for(var node = doc.firstChild.firstChild; node; node = node.nextSibling) {
  			if(node.nodeName === 'category' && node.getAttribute('id') === id) {
  				treeList.setSource(node);
  				break;
  			}
  		}

  		var tabs = document.querySelectorAll('.tabs .tab-header');
  		for(var i = 0; i < tabs.length; i++) {
  			tabs[i].classList.remove('active');
  		}

  		document.querySelector('.tabs .tab-header[data-target='+id+']').classList.add('active');
  		updateHelpText();
  	}

  	treeList.on('change', function(node) {
  		updateHelpText();
  	});

  	treeList.on('back', function() {
  		objectListElement.innerHTML = '';
  		updateHelpText();
  	});
  	treeList.on('select', function(node) {
  		if(node.getElementsByTagName('category').length)
  			return;

  		screenManager.push('object_list');
	
  		renderObjectList(node);
  		objectListController.setSelectedIndex(0);
  	});

  	tabListController.on('selectedIndexChanged', function({index}) {
  		var j =0;
  		for(var node = doc.firstChild.firstChild; node; node = node.nextSibling) {
  			if(node.nodeName === 'category') {
  				if(j++ == index) {
  					changeTab(node.getAttribute('id'));
  					return;
  				}
  			}
  		}
  	});

  	function handleUiInput(uiButtonCode) {
  		var screen = screenManager.currentScreen();
  		screen.handleInput(uiButtonCode);
  	}

  	document.addEventListener('keydown', function(e) {	
  		switch(e.keyCode) {
			case 27:
  				handleUiInput(1);
  			break
  			case 38:
  			case 40:
  				handleUiInput(e.keyCode == 38 ? 8 : 9);
  			break;
  			case 37:
			case 39:
				handleUiInput(e.keyCode == 37 ? 10 : 11);
			break;
  			case 32:
  			case 13:
  				handleUiInput(0);
  			break;
		}
  	});


	(function() {
		/* todo: merge this with object properties */
	  	var stickTicks = { left: 0, right: 0, up: 0, down: 0 }
		var axisThreshold = .5;
		var lastHeldUpdate = 0;

		dew.on('controllerinput', function(e){ 
		    if(e.data.AxisLeftX > axisThreshold)
		        stickTicks.right++;
		    else
		    	stickTicks.right = 0;

		    if(e.data.AxisLeftX < -axisThreshold)
		        stickTicks.left++;
		     else
		        stickTicks.left = 0;


		    if(e.data.AxisLeftY > axisThreshold)
		        stickTicks.up++;
		    else
		    	stickTicks.up = 0;
		    if(e.data.AxisLeftY < -axisThreshold)
		        stickTicks.down++;
		    else
		    	stickTicks.down = 0;

		    var holding =
		    	stickTicks.up > 0 && (stickTicks.up * e.data.secondsPerTick) > .5 ||
		    	stickTicks.down > 0 && (stickTicks.down * e.data.secondsPerTick) > .5 ||
		    	stickTicks.left > 0 && (stickTicks.left * e.data.secondsPerTick) > .5 ||
		    	stickTicks.right > 0 && (stickTicks.right * e.data.secondsPerTick) > .5;
		   	

		    if(stickTicks.up == 1 || (stickTicks.up > 1 && holding && (e.data.gameTicks - lastHeldUpdate) * e.data.secondsPerTick > 0.1)) {
		    	lastHeldUpdate = e.data.gameTicks;
		    	handleUiInput(8);
		    }
		    else if(stickTicks.down == 1 || (stickTicks.down > 1 && holding && (e.data.gameTicks - lastHeldUpdate) * e.data.secondsPerTick > 0.1)) {
		    	lastHeldUpdate = e.data.gameTicks;
		    	handleUiInput(9);
		    }
		    else if(stickTicks.left == 1 || (stickTicks.left > 1 && holding && (e.data.gameTicks - lastHeldUpdate) * e.data.secondsPerTick > 0.1)) {
		    	lastHeldUpdate = e.data.gameTicks;
		    	handleUiInput(10);
		    }
		    else if(stickTicks.right == 1 || (stickTicks.right > 1 && holding && (e.data.gameTicks - lastHeldUpdate) * e.data.secondsPerTick > 0.1)) {
		    	lastHeldUpdate = e.data.gameTicks;
		    	handleUiInput(11);
		    }

		    if(e.data.A == 1) handleUiInput(0);
		    else if(e.data.B == 1) handleUiInput(1);
		    else if(e.data.X == 1) handleUiInput(2);
		    else if(e.data.Y == 1) handleUiInput(3);
		    else if(e.data.RightBumper == 1) handleUiInput(4);
		    else if(e.data.LeftBumper == 1) handleUiInput(5);
        else if(e.data.Up == 1) handleUiInput(8);
        else if(e.data.Down == 1) handleUiInput(9);
        else if(e.data.Left == 1) handleUiInput(10);
        else if(e.data.Right == 1) handleUiInput(11);
			});
	})();

  	dew.on('show', function(e) {
  		for(var i = 0; i < e.data.budget.length; i++) {
  			var itemBudget = e.data.budget[i];
  			budget[parseInt(itemBudget.tagindex)] = itemBudget;
  		}
  		var oldSelectedIndex = objectListController.selectedIndex();
  		renderObjectList(treeList.selectedNode());
  		objectListController.setSelectedIndex(oldSelectedIndex);
	});

	changeTab('props');
 
})();


function makeTreeList(_element, _data) {
	var _stack = [];
	var _paths = [];
	var _callbacks = {};
	var _selectedIndex = 0;
	var _lastSelectedIndex = -1;

	push(_data.firstChild);
	render();

  _element.addEventListener('mouseover', function(e) {
    var nodes = Array.prototype.slice.call(_element.children);
    _selectedIndex = nodes.indexOf(e.target);
    renderSelection();
  }, true);

	return  {
		on: function(eventType, callback) {
			if(!_callbacks[eventType])
				_callbacks[eventType] = [];
			_callbacks[eventType].push(callback);
		},
		handleInput : function(uiButtonCode) {
			switch(uiButtonCode) {
				case 0:
					var selected = getSelectedPath();
					if(selected) {
						push(selected);

						emit('select', selected);
					}
				break;
				case 1:
					pop();
				break;
				case 8:
				case 9:
					navigate(uiButtonCode == 9 ? 1 : -1);
				break;
			}
		},
		atRoot: function() {
			return _stack.length == 1;
		},
		selectedNode: function() {
			return getSelectedPath();
		},
		setSource: function(node) {
			_lastSelectedIndex = -1;
			_selectedIndex = 0;
			_stack = [];
			_paths = [];
			_stack.push(node);
			render();
		}
	}


  function getNodeHelp(node) {
  	for(var n = node.firstChild; n; n = n.nextSibling) {
  		if(n.nodeName === 'help') {
  			return n.textContent;
  		}
  	}
  }

	function getSelectedPath() {
		return _paths[_selectedIndex];
	}

	function navigate(dir) {
		var oldIndex = _selectedIndex;

		if(dir > 0) {
			_selectedIndex++;
		} else {
			_selectedIndex--;
		}

		var count = _paths.length;

		if(_selectedIndex >= count)
			_selectedIndex = 0;
		if(_selectedIndex < 0)
			_selectedIndex = count-1;
				
		renderSelection();

		if(oldIndex != _selectedIndex)
			emit('change', getSelectedPath());

	}

	function emit(eventType, data) {
		var cbs = _callbacks[eventType] || [];
		for(var i = 0; i < cbs.length; i++)
			cbs[i](data);
	}

	function pop() {
		if(_stack.length < 2)
			return;
		_stack.pop();
		render();
		_selectedIndex = _lastSelectedIndex == -1 ? 0 : _lastSelectedIndex;
		_lastSelectedIndex = -1;
		renderSelection();

		emit('back');
	}

	function push(node) {
		if(!node.getElementsByTagName('category').length) {
			return;
		}

		_lastSelectedIndex = _selectedIndex;
		_stack.push(node);
		render();
		_selectedIndex = 0;
		renderSelection();
	}

	function renderSelection() {
		var items = _element.querySelectorAll('li');
		for(var i = 0; i < items.length; i++) {
			items[i].classList.remove('selected');
		}

		if(items[_selectedIndex]) {
			var itemElem = items[_selectedIndex];
			itemElem.classList.add('selected');
			itemElem.scrollIntoView(false);
		}
	}

	function renderNode(node) {
		var html = '';
		_paths = [];
		for(var n = node.firstChild; n; n = n.nextSibling) {
			if(n.nodeName === 'category') {
				var name = n.getAttribute('name');
				var help = getNodeHelp(n);
				_paths.push(n);
				html += `<li data-help="${help}">${name}</li>`
			}
		}
		return html;
	}

	function render() {
		var currentNode = _stack[_stack.length-1];
		_element.innerHTML = renderNode(currentNode);
		renderSelection();
	}
}
