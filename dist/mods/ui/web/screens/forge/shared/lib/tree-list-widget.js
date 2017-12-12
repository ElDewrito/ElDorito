dew.makeTreeList = function (_element, _data) {
    var _stack = [];
    var _paths = [];
    var _callbacks = {};
    var _hasFocus = false;

    var _listWidget = dew.makeListWidget(_element, {
        itemSelector: 'li',
        hoverSelection: true,
        hoverClass: 'selected',
        selectedClass: 'selected',
        wrapAround: true
    });

    if (_data) {
        push(_data.firstChild);
        render();
    }

    _listWidget.on('selectionChange', ({ index, element }) => {
        emit('selectionChange', { index: index, element: element, path: _paths[index] })
    });


    _listWidget.on('select', function ({ index, element }) {
        emit('select', { index: index, element: element, path: _paths[index] })
        push(_paths[index]);
    });

    dew.ui.on('action', function ({ action }) {
        if (!_hasFocus)
            return;
        switch (action) {
            case dew.ui.Actions.B:
                dew.ui.playSound(dew.ui.Sounds.B);
                pop();
                break;
        }
    });

    return {
        focus: function () {
            _hasFocus = true;
            _listWidget.focus();
        },
        blur: function () {
            _hasFocus = false;
            _listWidget.blur();
        },
        on: function (eventType, callback) {
            if (!_callbacks[eventType])
                _callbacks[eventType] = [];
            _callbacks[eventType].push(callback);
        },
        atRoot: function () {
            return _stack.length == 1;
        },
        selectedNode: function () {
            return getSelectedPath();
        },
        setSource: function (node) {
            _lastSelectedIndex = -1;
            _selectedIndex = 0;
            _stack = [];
            _paths = [];
            push(node);
            render();
        },
        gotoRoot: function () {
            _selectedIndex = 0;
            while (_stack.length > 1) {
                pop();
            }
            render();
            _listWidget.refresh();
            _listWidget.setSelected(0);
        },
        pop: pop,
        gotoNode: gotoNode
    }

    function gotoNode(node) {
        var path = [];

        for (var x = node; x; x = x.parentNode) {

            if (x.nodeName === 'category') {
                path.push(x);
            }
        }
        while (path.length) {
            var node = path.pop();
            push(node);
            if (path.length === 0) {
                var categoryIndex = -1;
                for (var x = node.parentNode.firstChild; x.nextSibling; x = x.nextSibling) {
                    if (x.nodeName === 'category')
                        categoryIndex++;
                    if (x === node) {
                        _selectedIndex = categoryIndex;
                        renderSelection();
                    }
                }
            }

        }
    }

    function getNodeHelp(node) {
        for (var n = node.firstChild; n; n = n.nextSibling) {
            if (n.nodeName === 'help') {
                return n.textContent;
            }
        }
    }

    function getSelectedPath() {
        return _paths[_selectedIndex];
    }

    function navigate(dir) {
        var oldIndex = _selectedIndex;

        if (dir > 0) {
            _selectedIndex++;
        } else {
            _selectedIndex--;
        }

        var count = _paths.length;

        if (_selectedIndex >= count)
            _selectedIndex = 0;
        if (_selectedIndex < 0)
            _selectedIndex = count - 1;

        renderSelection();

    }

    function emit(eventType, data) {
        if (!_callbacks[eventType])
            return;

        for (let callback of _callbacks[eventType])
            callback(data);
    }

    function pop() {
        if (_stack.length < 2)
            return;

        var current = _stack[_stack.length - 1];
        var lastIndex = current.index;
        _stack.pop();

        render();

        var current = _stack[_stack.length - 1];

        _listWidget.refresh();
        _listWidget.setSelected(lastIndex);

        emit('pop');

        if (current)
            emit('change', { index: lastIndex, path: current.node });
    }

    function push(node) {
        if (!node)
            return;

        if (!node.getElementsByTagName('category').length) {
            return;
        }
        var selected = _listWidget.getSelected();
        var d = { node: node, index: (selected ? selected.index : 0) };
        _stack.push(d);


        render();
        _listWidget.refresh();
        _listWidget.setSelected(0);

        emit('push', node);
        emit('change', { index: 0, path: node });
    }


    function renderNode(node) {
        var html = '';
        _paths = [];
        for (var n = node.firstChild; n; n = n.nextSibling) {
            if (n.nodeName === 'category') {
                var name = n.getAttribute('name');
                var help = getNodeHelp(n);
                _paths.push(n);
                html += `<li class="item" data-help="${help}">${name}</li>`
            }
        }
        return html;
    }

    function render() {
        if (!_stack.length)
            return;
        var currentNode = _stack[_stack.length - 1];
        if (!currentNode)
            return;
        _element.innerHTML = renderNode(currentNode.node);
    }
}
