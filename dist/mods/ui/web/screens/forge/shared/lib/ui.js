dew.ui = (function () {
    const HOLD_DELAY = .5;
    const HELD_SCROLL_SPEED = .1;
    const SINGLE_SCROLL_SPEED = .125;
    const CONTROLLER_AXIS_THRESHOLD = .5;
    const UiAction = {
        LeftTrigger: 0,
        RightTrigger: 1,
        Up: 2,
        Down: 3,
        Left: 4,
        Right: 5,
        Start: 6,
        Select: 7,
        LeftStick: 8,
        RightStick: 9,
        A: 10,
        B: 11,
        X: 12,
        Y: 13,
        LeftBumper: 14,
        RightBumper: 15
    };
    const UiSound = {
        Error: 0,
        VerticalNavigation: 1,
        HorizontalNavigation: 2,
        A: 3,
        B: 4,
        X: 5,
        Start: 7,
        Back: 8,
        LeftBumper: 9,
        RightBumper: 10,
        LeftTrigger: 11,
        RightTrigger: 12
    };

    var _callbacks = {};
    var _stickTicks = { left: 0, right: 0, up: 0, down: 0 }
    var _lastHeldUpdate = 0;
    var _lastScrollTime = 0;
    var _keyStates = {};
    var _keyTicks = {};

    init();

    dew.on('hide', function () {
        _keyStates = {};
        _keyTicks = {};
        _stickTicks = {};
        _lastHeldUpdate = 0;
        _lastScrollTime = 0;
    })

    return {
        on: function (event, callback) {
            (_callbacks[event] || (_callbacks[event] = [])).push(callback);
        },
        playSound(index) {
            var tagIndex = getUiSoundTagIndex(index);
            if (tagIndex == -1)
                return;
            dew.command('Game.PlaySound 0x' + tagIndex.toString(16));
        },

        Actions: UiAction,
        Sounds: UiSound,
    }

    function getUiSoundTagIndex(index) {
        switch (index) {
            case UiSound.Error: return 0xafd;
            case UiSound.VerticalNavigation: return 0xafe;
            case UiSound.HorizontalNavigation: return 0xaff;
            case UiSound.A: return 0xb00;
            case UiSound.B: return 0xb01;
            case UiSound.X: return 0xb02;
            case UiSound.Start: return 0xb03;
            case UiSound.Back: return 0xb04;
            case UiSound.LeftBumper: return 0xb05;
            case UiSound.RightBumper: return 0xb06;
            case UiSound.LeftTrigger: return 0xb05;
            case UiSound.RightTrigger: return 0xb06;
            default: return -1;
        }
    }


    function init() {
        dew.on('controllerinput', tick);
        window.addEventListener('keydown', function (e) { _keyStates[e.keyCode] = true }, false);
        window.addEventListener('keyup', function (e) { _keyStates[e.keyCode] = false }, false);
    }

    function emit(event) {
        var eventCallbacks = _callbacks[event];
        if (eventCallbacks) {
            var args = Array.prototype.splice.call(arguments, 1);
            for (var i = 0; i < eventCallbacks.length; i++) {
                eventCallbacks[i].apply(null, args);
            }
        }
    }


    function tick(e) {
        var controller = e.data,
            secondsPerTick = e.data.secondsPerTick,
            gameTicks = e.data.gameTicks;

        emit('tick');

        if (_keyStates[40]) _keyTicks[UiAction.Down]++; else _keyTicks[UiAction.Down] = 0;
        if (_keyStates[38]) _keyTicks[UiAction.Up]++; else _keyTicks[UiAction.Up] = 0;
        if (_keyStates[37]) _keyTicks[UiAction.Left]++; else _keyTicks[UiAction.Left] = 0;
        if (_keyStates[39]) _keyTicks[UiAction.Right]++; else _keyTicks[UiAction.Right] = 0;
        if (_keyStates[32] || _keyStates[13]) _keyTicks[UiAction.A]++; else _keyTicks[UiAction.A] = 0;
        if (_keyStates[27]) _keyTicks[UiAction.B]++; else _keyTicks[UiAction.B] = 0;
        if (_keyStates[33]) _keyTicks[UiAction.LeftTrigger]++; else _keyTicks[UiAction.LeftTrigger] = 0;
        if (_keyStates[34]) _keyTicks[UiAction.RightTrigger]++; else _keyTicks[UiAction.RightTrigger] = 0;
        if (_keyStates[46]) _keyTicks[UiAction.Y]++; else _keyTicks[UiAction.Y] = 0;

        if (_stickTicks.right > 255) _stickTicks.right = 255;
        if (_stickTicks.left > 255) _stickTicks.left = 255;
        if (_stickTicks.up > 255) _stickTicks.up = 255;
        if (_stickTicks.down > 255) _stickTicks.down = 255;

        if (e.data.AxisLeftX > CONTROLLER_AXIS_THRESHOLD) _stickTicks.right++; else _stickTicks.right = 0;
        if (e.data.AxisLeftX < -CONTROLLER_AXIS_THRESHOLD) _stickTicks.left++; else _stickTicks.left = 0;
        if (e.data.AxisLeftY > CONTROLLER_AXIS_THRESHOLD) _stickTicks.up++; else _stickTicks.up = 0;
        if (e.data.AxisLeftY < -CONTROLLER_AXIS_THRESHOLD) _stickTicks.down++; else _stickTicks.down = 0;

        if (controller.LeftTrigger == 1 || _keyTicks[UiAction.LeftTrigger] == 1) {
            emit('scroll', 2, 0, -1);
        }
        if (controller.RightTrigger == 1 || _keyTicks[UiAction.RightTrigger] == 1) {
            emit('scroll', 2, 0, 1);
        }

        if ((gameTicks - _lastScrollTime) * secondsPerTick > SINGLE_SCROLL_SPEED) {
            if ((_stickTicks.up == 1 || controller.Up == 1 || _keyTicks[UiAction.Up] == 1)) {
                _lastScrollTime = gameTicks;
                emit('scroll', 0, 0, -1);
            }
            if ((_stickTicks.down == 1 || controller.Down == 1 || _keyTicks[UiAction.Down] == 1)) {
                _lastScrollTime = gameTicks;
                emit('scroll', 0, 0, 1);
            }
            if ((_stickTicks.right == 1 || controller.Right == 1 || _keyTicks[UiAction.Right] == 1)) {
                _lastScrollTime = gameTicks;
                emit('scroll', 0, 1, 1);
            }
            if ((_stickTicks.left == 1 || controller.Left == 1 || _keyTicks[UiAction.Left] == 1)) {
                _lastScrollTime = gameTicks;
                emit('scroll', 0, 1, -1);
            }
        }


        if ((gameTicks - _lastHeldUpdate) * secondsPerTick > HELD_SCROLL_SPEED) {
            if ((_stickTicks.up > 1 && (_stickTicks.up * secondsPerTick) > HOLD_DELAY) ||
                ((controller.Up > 1 && (controller.Up * secondsPerTick) > HOLD_DELAY)) ||
                (_keyTicks[UiAction.Up] * secondsPerTick) > HOLD_DELAY) {
                _lastHeldUpdate = gameTicks;
                emit('scroll', 1, 0, -1);
            }
            if ((_stickTicks.down > 1 && (_stickTicks.down * secondsPerTick) > HOLD_DELAY) ||
                ((controller.Down > 1 && (controller.Down * secondsPerTick) > HOLD_DELAY)) ||
                (_keyTicks[UiAction.Down] * secondsPerTick) > HOLD_DELAY) {
                _lastHeldUpdate = gameTicks;
                emit('scroll', 1, 0, 1);
            }
            if ((_stickTicks.right > 1 && (_stickTicks.right * secondsPerTick) > HOLD_DELAY) ||
                ((controller.Right > 1 && (controller.Right * secondsPerTick) > HOLD_DELAY)) ||
                (_keyTicks[UiAction.Right] * secondsPerTick) > HOLD_DELAY) {
                _lastHeldUpdate = gameTicks;
                emit('scroll', 1, 1, 1);
            }
            if ((_stickTicks.left > 1 && (_stickTicks.left * secondsPerTick) > HOLD_DELAY) ||
                ((controller.Left > 1 && (controller.Left * secondsPerTick) > HOLD_DELAY)) ||
                (_keyTicks[UiAction.Left] * secondsPerTick) > HOLD_DELAY) {
                _lastHeldUpdate = gameTicks;
                emit('scroll', 1, 1, -1);
            }
        }

        if (controller.A == 1) emit('action', { inputType: 'controller', action: UiAction.A });
        if (controller.B == 1) emit('action', { inputType: 'controller', action: UiAction.B });
        if (controller.X == 1) emit('action', { inputType: 'controller', action: UiAction.X });
        if (controller.Y == 1) emit('action', { inputType: 'controller', action: UiAction.Y });

        for (var i = 0; i < 16; i++) {
            if (_keyTicks[i] == 1) {
                emit('action', { inputType: 'keyboard', action: i });
            }
        }
    }
})();