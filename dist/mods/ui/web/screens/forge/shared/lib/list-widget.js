dew.makeListWidget = function (_elem, settings) {
    var _scrollIntoView = false;
    var _callbacks = {};
    var _itemNodes = [];
    var _prevCurrentNode = null;
    var _currentNode = null;
    var _domObserver = null;
    var _hasFocus = false;

    var _settings = Object.assign({
        wrapAround: false,
        hoverSelection: false,
        itemSelector: null,
        initialSelectedIndex: -1,
        pageSize: 4,
        mouseDownFilter: null
    }, settings);

    init();

    return {
        navigate: navigate,
        setSelected: function (index, scrollto) {
            if (index == -1) {
                clearSelection();
                render();
            } else {
                setSelectedIndex(index, false, scrollto);
            }
        },
        getSelected: function () {
            if (!_currentNode)
                return null;
            return { index: _itemNodes.indexOf(_currentNode), element: _currentNode }
        },
        itemCount: function () {
            return _itemNodes.length;
        },
        on: function (type, callback) {
            if (!_callbacks[type])
                _callbacks[type] = [];
            _callbacks[type].push(callback);
        },
        focus: function () {
            cacheItemNodes();
            _hasFocus = true;
            _elem.addEventListener('mousedown', onMouseDown);
            _elem.classList.add('has-focus');
        },
        blur: function () {
            _hasFocus = false;
            _elem.removeEventListener('mousedown', onMouseDown);
            _elem.classList.remove('has-focus');
        },
        hasFocus: () => _hasFocus,
        refresh: function () {
            cacheItemNodes();
            render();
        }
    }

    function init() {
        attachEventHandlers();
        cacheItemNodes();
        render();

        dew.ui.on('scroll', function (type, axis, direction) {
            if (!_hasFocus)
                return;
            if (axis == 0) {
                if (type < 2)
                    navigate(direction);
                else if (type == 2) {
                    var index = _itemNodes.indexOf(_currentNode);

                    if ((direction < 0 && index == 0) || (direction > 0 && index == (_itemNodes.length - 1)))
                        return;

                    var newIndex = index + _settings.pageSize * direction;
                    if (newIndex >= _itemNodes.length) {
                        newIndex = _itemNodes.length - 1;
                    } else if (newIndex < 0) {
                        newIndex = 0;
                    }
                    dew.ui.playSound(dew.ui.Sounds.LeftTrigger + (direction > 0 ? 1 : 0));
                    setSelectedIndex(newIndex, true, true);
                }

            }
        });

        dew.ui.on('action', function ({ action }) {
            if (!_hasFocus)
                return;
            switch (action) {
                case dew.ui.Actions.A:
                    if (_currentNode) {
                        dew.ui.playSound(dew.ui.Sounds.A);
                        emit('select', { index: _itemNodes.indexOf(_currentNode), element: _currentNode });
                    }
                    break;
            }
        });
    }

    function onMouseDown(e) {
        if (!_hasFocus)
            return;

        var node = getParentItemNode(e.target);
        if (node) {
            setCurrentNode(node, false);
            if (!_settings.mouseDownFilterSelector || !e.target.matches(_settings.mouseDownFilterSelector)) {
                dew.ui.playSound(dew.ui.Sounds.A);
                emit('select', { index: _itemNodes.indexOf(node), element: _currentNode });
                e.preventDefault();
            }
        }
    }

    function attachEventHandlers() {
        _elem.addEventListener('mousemove', function (e) {
            if (!_hasFocus)
                return;
            if (!_settings.hoverSelection)
                return;
            var parent = getParentItemNode(e.target);
            if (parent === _elem)
                return;
            if (e.target === _elem)
                return;
            setCurrentNode(parent, true);
        }, false);

        _domObserver = new MutationObserver(function (mutations) {
            cacheItemNodes();
        }), false;
        _domObserver.observe(_elem, { childList: true, subtree: true });
    }

    function cacheItemNodes() {
        var nodes = _settings.itemSelector
            ? _elem.querySelectorAll(_settings.itemSelector) : _elem.children;

        _itemNodes = Array.prototype.slice.call(nodes);
    }

    function clearSelection() {
        setCurrentNode(null);
        render();
    }

    function setCurrentNode(node, notify) {
        if (_currentNode == node)
            return;
        _prevCurrentNode = _currentNode;
        _currentNode = node;
        render();

        if (notify && node)
            emit('selectionChange', { index: _itemNodes.indexOf(node), element: node });
    }

    function setSelectedIndex(index, notify, scrollto) {
        if (index >= _itemNodes.length)
            index = _settings.wrapAround ? 0 : (_itemNodes.length - 1);
        if (index < 0)
            index = _settings.wrapAround ? (_itemNodes.length - 1) : 0;

        var item = _itemNodes[index];
        setCurrentNode(item, notify);
        if (_currentNode && scrollto)
            _currentNode.scrollIntoView(false);

        return index;
    }

    function navigate(count) {
        let oldIndex = _itemNodes.indexOf(_currentNode);
        let newIndex = setSelectedIndex(_itemNodes.indexOf(_currentNode) + count, true, true);
        if (oldIndex != -1 && newIndex != -1 && oldIndex != newIndex) {
            dew.ui.playSound(dew.ui.Sounds.VerticalNavigation);
        }
    }

    function render() {
        if (_prevCurrentNode) {
            _prevCurrentNode.classList.remove(_settings.hoverClass);
        }

        if (_itemNodes.length && _currentNode) {
            _currentNode.classList.add(_settings.hoverClass);
        }
    }

    function getParentItemNode(node) {
        var prevNode = node;
        while ((node = node.parentNode) && node != _elem) {

            prevNode = node;
        }

        if (_itemNodes.indexOf(prevNode) == -1)
            return null;
        return prevNode;
    }

    function emit(type, data) {
        var eventCallbacks = _callbacks[type];
        if (!eventCallbacks)
            return;
        for (var i = 0; i < eventCallbacks.length; i++)
            eventCallbacks[i](data);
    }
}
