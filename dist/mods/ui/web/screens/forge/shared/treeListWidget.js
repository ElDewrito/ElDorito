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
        },
        clearSelection: function() {
            _selectedIndex =0;
            renderSelection();
        },
        gotoRoot: function() {
            _selectedIndex = 0;
            while(_stack.length > 1) {
                pop();
            }
            render();
        },
        gotoNode: gotoNode
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

    function gotoNode(node) {
        var path = [];

        for(var x = node; x; x = x.parentNode) {
            
            if(x.nodeName === 'category') {
                path.push(x);
            }
        }
        while(path.length) {
            var node = path.pop();
            push(node);
            if(path.length === 0) {
                var categoryIndex = -1;
                for(var x = node.parentNode.firstChild; x.nextSibling; x = x.nextSibling) {
                    if(x.nodeName === 'category')
                        categoryIndex++;
                    if(x === node) {
                        _selectedIndex = categoryIndex;
                        renderSelection();
                    }
                }
            }
           
        }
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