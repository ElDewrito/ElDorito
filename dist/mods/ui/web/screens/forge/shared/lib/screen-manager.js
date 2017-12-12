dew.makeScreenManager = function (settings) {
    var _screenStack = [];
    var _screensByName = {};
    var _queue = [];
    var _lastTimeRequestHandled = null;

    dew.ui.on('action', function (action) {
        if (_screenStack.length) {
            var activeScreen = _screenStack[_screenStack.length - 1];
            if (activeScreen.onAction)
                activeScreen.onAction(action);
        }
    });

    dew.ui.on('tick', function () {
        if ((Date.now() - _lastTimeRequestHandled) < 70)
            return;
        _lastTimeRequestHandled = Date.now();

        while (_queue.length)
            _queue.shift()();
    });

    return {
        count: () => _screenStack.length,
        setModel: setModel,
        push: push,
        pop: pop,
        any: () => _screenStack.length > 0,
        popAll: (includeRoot = true) => {
            _activationQueue = [];
            while (_screenStack.length > (includeRoot ? 0 : 1)) {
                var screen = _screenStack.pop();
                if (screen) {
                    screen.deactivate({});
                }
            }

            if (!includeRoot && _screenStack.length)
                _screenStack[_screenStack.length - 1].activate({});


        },
        currentScreen: () => _screenStack.length ? _screenStack[_screenStack.length - 1] : null
    }

    function setModel(model) {
        var keys = Object.keys(model);
        _screensByName = model;
    }

    function push(name, props = {}) {
        _queue.push(() => {
            var screen = _screensByName[name];
            if (!screen) {
                console.error(`failed to find screen '${name}'`);
                return;
            }
            screen.name = name;
            if (_screenStack.length)
                _screenStack[_screenStack.length - 1].deactivate(props);
            _screenStack.push(screen);
            screen.activate(props);
        });
    }

    function pop() {
        _queue.push(() => {
            if (_screenStack.length < 2)
                return;

            var screen = _screenStack.pop();
            if (screen)
                screen.deactivate({});

            var current = _screenStack[_screenStack.length - 1];
            current.activate(null);
        });
    }
};