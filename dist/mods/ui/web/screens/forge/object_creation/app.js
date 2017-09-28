(function($) {
    var categoryTreeElement = document.getElementById('category_tree');
    var objectListElement = document.getElementById('object_list');
    var searchResultListElement = document.querySelector('.search-results');
    var tabsElement = document.getElementById('main_tabs');
    var treeList = makeTreeList(categoryTreeElement, null);
    var budget = {};
    var doc = null;
    var objectListNode = null;
    var searchResults = null;
    var doSearch = null;
  
    loadItems(function(data) {
        var parser = new DOMParser();
        doc = parser.parseFromString(data, "application/xml");
        treeList.setSource(doc.firstChild);

        var itemsJson = buildItemsJson(doc.firstChild);

        var fuse = new Fuse(itemsJson,  {
            shouldSort: true,
            threshold: 0.6,
            location: 0,
            distance: 100,
            maxPatternLength: 32,
            minMatchCharLength: 1,
            keys: ["name"]
        });

        doSearch = function(query) {
            return fuse.search(query);
        }

        changeTab('props');
    });

    $(document).mouseup(function(e) {
        var container = $(".object-browser");
        if (!container.is(e.target) && container.has(e.target).length === 0) {
            quickClose();
        }
    });

    document.querySelector('.search-text').addEventListener('input', function(e) {
        e.preventDefault();
        e.stopPropagation();

        var query = e.target.value.trim();
        if(!query || !query.length)
            return;

        searchResults = doSearch(query);
        renderSearchResults(searchResults);
        searchResultListController.setSelectedIndex(0);
        updateHelpText();
    });

    document.querySelector('.search-text').addEventListener('keydown', function(e) {   
        if(e.keyCode == 38  || e.keyCode == 40) {
            e.preventDefault();
        }
    });
    
    var objectListController = makeListWidgetController('object_list', {wrapAround:true}, {
        count: () => objectListElement.querySelectorAll('li').length
    })
    var tabListController = makeListWidgetController('tab_list', {}, {
        count: () => 3
    });
    var searchResultListController = makeListWidgetController('search_results', {wrapAround:true}, {
        count: () => searchResultListElement.querySelectorAll('li').length
    })

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

    searchResultListController.on('selectedIndexChanged', function({index, userInput}) {
        var items = searchResultListElement.querySelectorAll('li');
        for(var i = 0; i < items.length; i++) {
            items[i].classList.remove('selected');
        }
        var item = searchResultListElement.querySelector(`li:nth-of-type(${index+1})`);
        if(item) {
            item.classList.add('selected');
            if(userInput)
                item.scrollIntoView(false);
        }
    });

    categoryTreeElement.addEventListener('click', function(e) {
        handleUiInput(0);
    }, true);

    objectListElement.addEventListener('click', function(e) {
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

    searchResultListElement.addEventListener('mouseover', function(e) {
        if(e.target.nodeName == 'LI') {
            var nodes = Array.prototype.slice.call(searchResultListElement.children);
            searchResultListController.setSelectedIndex(nodes.indexOf(e.target));
        }    
    }, true);

    objectListElement.addEventListener('click', function(e) {
        handleUiInput(0);
    }, true);

    searchResultListElement.addEventListener('click', function(e) {
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
            },
            onKeyDown: function(keyCode) {
                if(!(!(keyCode > 47 && keyCode < 58) && // 0-9
                    !(keyCode > 64 && keyCode < 91) && // A-Z
                    !(keyCode > 96 && keyCode < 123))) { // a-z
                    screenManager.push('search');
                }
            }
        },
        object_list: {
            name: 'object_list',
            activate: function() {
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
                        var index = $(selected).index();

                        if(selected) {
                            var selectedTagIndex = parseInt(selected.getAttribute('data-tagindex'));

                            var j = 0;
                            var node = null;
                            for(var x = objectListNode.firstChild; x; x = x.nextSibling) {
                                if(x.nodeName === 'item') {
                                    if(j++ == index) {
                                        node = x;
                                    }

                                }
                            }

                            if(!node)
                                return;

                            var props = {};
                            for(var x = node.firstChild; x; x = x.nextSibling) {
                                if(x.nodeName === 'setter') {
                                    var target = x.getAttribute('target');
                                    var value = x.getAttribute('value');
                                    var type = x.getAttribute('type');
                                    switch(type) {
                                        case 'int':
                                            props[target] = parseInt(value);
                                            break;
                                        case 'float':
                                            props[target] = parseFloat(value);
                                            break;
                                    }
                                    
                                }
                            }

                            dew.callMethod('forgeaction',  {
                                type: 2,
                                data: props
                            });

                            dew.command('forge.spawnitem ' + selected.getAttribute('data-tagindex')).then(function() {});
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
            },
            onKeyDown: function(keyCode) {
                if(!(!(keyCode > 47 && keyCode < 58) && // 0-9
                    !(keyCode > 64 && keyCode < 91) && // A-Z
                    !(keyCode > 96 && keyCode < 123))) { // a-z
                    screenManager.push('search');
                }    
            }
        },
        search: {
            name: 'search',
            activate: function() {
                $('#main_tabs').hide();
                var searchContainerElement = document.getElementById('search');
                searchContainerElement.classList.remove('hidden');
                searchContainerElement.classList.add('active');
                var searchTextInput = searchContainerElement.querySelector('.search-text');
                searchTextInput.value = '';
                searchTextInput.focus();
            },
            deactivate: function() {
                $('#main_tabs').show();

                var searchContainerElement = document.getElementById('search');
                searchContainerElement.classList.add('hidden');
                searchContainerElement.classList.remove('active');
            },
            handleInput: function(uiButtonCode) {
                switch(uiButtonCode) {
                    case 8:
                    case 9:
                        searchResultListController.navigate(uiButtonCode == 8 ? -1 : 1);
                        dew.command('Game.PlaySound 0xafe');
                        break;
                    case 0:
                        dew.command('Game.PlaySound 0xb00');
                        var selectedElem = searchResultListElement.querySelector('li.selected');
                        dew.command('forge.spawnitem ' + selectedElem.getAttribute('data-tagindex')).then(function() {});
                        dew.hide();
                        break;
                    case 1:
                        if(screenManager.currentScreen().name !== 'main')
                            screenManager.pop();
                        break;
                }
            }
        }
    })


    function buildItemsJson(node) {
        var items = [];
        for(var x = node.firstChild; x; x = x.nextSibling) {
           
            if(x.nodeName === 'category' || x.nodeName === 'root')
                items = items.concat(buildItemsJson(x));
            else if(x.nodeName === 'item') {
                items.push({
                    name: x.getAttribute('name'),
                    tagindex: x.getAttribute('tagindex'),
                    node: x
                })
            }
        }

        return items;
    }


    screenManager.push('main');

    function loadItems(cb) {
        $.get('items.xml', function(resp) {
            cb(resp);
        });
    }

    function updateHelpText() {
        var currentScreen = screenManager.currentScreen();

        var helpText = null;
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

    function renderSearchResults(results) {
        var searchContainerElement = document.getElementById('search');
        var searchResultListElement = searchContainerElement.querySelector('.search-results');

        var MAX_RESULTS = 7;
        var numResultsToDisplay = Math.min(MAX_RESULTS, results.length);

        var html = '';
        for(var i = 0; i < numResultsToDisplay; i++) {
            var result = results[i];

            var name = result.name;
            var tagindex = parseInt(result.tagindex);
            var itemBudget = budget[tagindex];
            var remainingItems = 255;

            var category = result.node.parentNode.getAttribute('name');

            if(itemBudget) {  
                remainingItems = itemBudget.max_allowed - itemBudget.count_on_map;
            }

            var help = getNodeHelp(result.node);

            html += `
            <li data-tagindex="${result.tagindex}" data-help="${help}">
                <span class="title">${name}<span class="text-mute"> [${category}]</span></span>
                <span class="quota">${remainingItems}</span>
            </li>`;
        }

        searchResultListElement.innerHTML = html;
    }

    function renderObjectList(node) {
        if(!node)
            return;

        objectListNode = node;

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
        switch(uiButtonCode) {
            case 2:
                quickClose();
            return;
        }
        var screen = screenManager.currentScreen();
        screen.handleInput(uiButtonCode);
    }

    document.addEventListener('keydown', function(e) {  
        var screen = screenManager.currentScreen();

        
        if(screen && screen.onKeyDown)
            screen.onKeyDown(e.keyCode);

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
    }, false);


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

    dew.on('show', function(e) {
        var currentScreen = screenManager.currentScreen();
        if(currentScreen) {
            currentScreen.activate();
        }
        budget = {};
        for(var i = 0; i < e.data.budget.length; i++) {
            var itemBudget = e.data.budget[i];
            budget[parseInt(itemBudget.tagindex)] = itemBudget;
        }

        var oldSelectedIndex = objectListController.selectedIndex();
        renderObjectList(treeList.selectedNode());
        objectListController.setSelectedIndex(oldSelectedIndex);

        if(searchResults && screenManager.currentScreen().name === 'search') {
            renderSearchResults(searchResults);
            searchResultListController.setSelectedIndex(0);
        }
    });

})(jQuery);

function quickClose() {
     dew.command('Game.PlaySound 0xb01');
     dew.hide();
}

function makeTreeList(_element, _data) {
    var _stack = [];
    var _paths = [];
    var _callbacks = {};
    var _selectedIndex = 0;
    var _lastSelectedIndex = -1;

    if(_data) {
        push(_data.firstChild);
        render();
    }

    _element.addEventListener('mouseover', function(e) {
        var nodes = Array.prototype.slice.call(_element.children);
        _selectedIndex = nodes.indexOf(e.target);
        renderSelection(false);
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

    function renderSelection(shouldScroll = true) {
        var items = _element.querySelectorAll('li');
        for(var i = 0; i < items.length; i++) {
            items[i].classList.remove('selected');
        }

        if(items[_selectedIndex]) {
            var itemElem = items[_selectedIndex];
            itemElem.classList.add('selected');
            if(shouldScroll)
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
