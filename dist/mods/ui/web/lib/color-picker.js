dew.makeColorPicker = function(elem) {
    var _container = elem,
        _callbacks = {},
        _hueSliderElement = null,
        _thumbElement = null,
        _hexInputElement = null,
        _paletteContext = null,
        _palettePixelBuffer = null,
        _thumbRadius = 0,
        _prevState = null,
        _state = { 
            selectedColor: { h: 240, s: 1, v: 1 }, 
            isDragging: false
        };

    init();

    return {
        on: function (type, callback) {
            if (!_callbacks[type])
                _callbacks[type] = [];
            _callbacks[type].push(callback);
        },
        setColor:  setColorInternal,
        getColor: function () {
            return _state.selectedColor;
        },
        controllerInput: controllerInput,
        refresh() {
            _palettePixelBuffer = null;
            setColorInternal(_state.selectedColor, false);
        }
    }

    function init() {
        doRender();
        attachEventHandlers();
    }

    function attachEventHandlers() {
        window.addEventListener('mousemove', function (e) {
            if (!_state.isDragging)
                return;
            if(e.which !== 1) {
                _state.isDragging = false;
                return;
            }
            handleDragging(e);
        }, false);

        _paletteContainer.addEventListener('mousedown', function (e) {
            if (e.which  === 1) {
                _state.isDragging = true;
                handleDragging(e);
            }
        }, false);

        _hueSliderElement.addEventListener('input', function (e) {
            setColorInternal({ h: e.target.value }, true);
        }, false);

        _hexInputElement.addEventListener('input', function(e) {
            var hsv = ColorUtil.hexToHsv(e.target.value.substr(1));
            if(hsv)
                setColorInternal(hsv, true);
        }, false);
    }

    function doRender() {
        render(_prevState, _state);
        prevState = Object.assign({}, _state);
    }

    function renderPalette(state) {
        var width = _paletteContext.canvas.width;
        var height = _paletteContext.canvas.height;
        if(!_palettePixelBuffer)
            _palettePixelBuffer = _paletteContext.createImageData(width, height);

        for (var y = 0; y < height; y++) {
            for (var x = 0; x < width; x++) {
                var rgb = ColorUtil.hsvToRgb(_state.selectedColor.h, x / width, 1.0 - (y / height));
                var index = (y * width + x) * 4;

                _palettePixelBuffer.data[index] = rgb[0];
                _palettePixelBuffer.data[index + 1] = rgb[1];
                _palettePixelBuffer.data[index + 2] = rgb[2];
                _palettePixelBuffer.data[index + 3] = 255;
            }
        }
        _paletteContext.putImageData(_palettePixelBuffer, 0, 0);
    }

    function leftPad(val, size, ch) {
        var result = String(val);
        if(!ch) {
            ch = " ";
        }
        while (result.length < size) {
            result = ch + result;
        }
        return result;
    }

    function render(prevState, state) {
        if (!_prevState) {
            _prevState = {};
            _container.innerHTML =
            `
                <div class ="ed-color-picker">
                    <div class="ed-color-picker__palette-wrapper">
                        <canvas class="ed-color-picker__palette"></canvas>
                        <div class ="ed-color-picker__palette-thumb"></div>
                    </div>
                    <div class="ed-color-picker__hue-slider-wrapper">
                        <input class="ed-color-picker__hue-slider" type="range" min="0" max="360">
                    </div>
                    <div class="ed-color-picker__detail">
                        <div>R: 0</div>
                        <div>G: 0</div>
                        <div>B: 0</div>
                        <div>HEX: <input type="text" class='ed-color-picker__detail_hex-input' /></div>
                    </div>
                </div>
            `;
            _paletteContainer = _container.querySelector('.ed-color-picker__palette-wrapper');
            _hueSliderElement = _container.querySelector('.ed-color-picker__hue-slider');
            _outputElements = _container.querySelectorAll('.ed-color-picker__detail >div');           
            _thumbElement = _paletteContainer.querySelector('.ed-color-picker__palette-thumb');
            _hexInputElement = _container.querySelector('.ed-color-picker__detail_hex-input');
            _paletteContext = _container.querySelector('.ed-color-picker__palette').getContext('2d');
        }

        if (!_prevState.selectedColor || _state.selectedColor != _prevState.selectedColor) {

            if (!_prevState.selectedColor || _state.selectedColor.h != _prevState.selectedColor.h)
                renderPalette(state);

            var containerRect = _paletteContainer.getBoundingClientRect();      
            var scaleX = _paletteContainer.clientWidth / (containerRect.right - containerRect.left),
                scaleY = _paletteContainer.clientHeight / (containerRect.bottom - containerRect.top);
            var thumbRect = _thumbElement.getBoundingClientRect();
            var thumbPosX = _state.selectedColor.s * (containerRect.right - containerRect.left),
                thumbPosY = (1.0 - _state.selectedColor.v) * (containerRect.bottom - containerRect.top);

            _thumbRadius =  (thumbRect.bottom - thumbRect.top) * 0.5;
            _thumbElement.style =  `left: ${((thumbPosX - _thumbRadius) * scaleX)}px; top: ${((thumbPosY - _thumbRadius) * scaleY)}px`;

            _hueSliderElement.value = _state.selectedColor.h;

            var rgb = ColorUtil.hsvToRgb(_state.selectedColor.h, _state.selectedColor.s, _state.selectedColor.v);
            _outputElements[0].innerHTML = `R: ${rgb[0]}`;
            _outputElements[1].innerHTML = `G: ${rgb[1]}`;
            _outputElements[2].innerHTML = `B: ${rgb[2]}`;
            _hexInputElement.value = `#${leftPad(ColorUtil.rgbToHex(rgb[0],rgb[1],rgb[2]), 6, '0')}`;
        }
    }

    function clamp(value, min, max) {
        return value < min ? min : (value > max ? max : value);
    }

    function handleDragging(e) {
        var containerRect = _paletteContainer.getBoundingClientRect();
        var scaleX = _paletteContainer.clientWidth / (containerRect.right - containerRect.left),
            scaleY = _paletteContainer.clientHeight / (containerRect.bottom - containerRect.top);

        var innerWidth = (containerRect.right - containerRect.left) * scaleX,
            innerHeight = (containerRect.bottom - containerRect.top) * scaleY;

        var x = clamp((e.pageX - containerRect.left) * scaleX, 0, innerWidth),
            y = clamp((e.pageY - containerRect.top) * scaleY, 0, innerHeight);

        setColorInternal({ 
            s: x / innerWidth, 
            v: 1.0 - (y / innerHeight)
        }, true);
    }

    function controllerInput(data) {
        const axisThreshold = 0.08,
            paletteSpeed = 0.02;
        hueSliderSpeed = 2;

        var { h, s, v } = _state.selectedColor;
        var changed = false;

        if(Math.abs(data.AxisRightX) > axisThreshold) {
            s = clamp(s + paletteSpeed * data.AxisRightX, 0.0, 1.0);
            changed = true;
        }
        if(Math.abs(data.AxisRightY) > axisThreshold) {
            v = clamp(v + paletteSpeed * data.AxisRightY, 0.0, 1.0);
            changed = true;
        }

        if(Math.abs(data.AxisLeftX) > axisThreshold) {
            h = clamp(h + hueSliderSpeed * data.AxisLeftX, 0, 360);
            changed = true;
        }

        if(changed)
            setColorInternal({h,s,v}, true);
    }
    
    function emit(type, data) {
        var callbacks = _callbacks[type];
        if (!callbacks)
            return;

        for (var i = 0; i < callbacks.length; i++) {
            callbacks[i](data);
        }
    }

    function setColorInternal(color, shouldEmit) {
        var newColor = Object.assign({}, _state.selectedColor, color);
        if(isNaN(newColor.h) || isNaN(newColor.s) || isNaN(newColor.v))
            return;

        _state.selectedColor = newColor;

        doRender();

        if (shouldEmit)
            emit('select', _state.selectedColor);
    }
}

var ColorUtil = {
    hsvToRgb (h, s, v) {
        h /= 360;
        v = Math.round(v * 255);
        var i = Math.floor(h * 6);
        var f = h * 6 - i;
        var p = Math.round(v * (1 - s));
        var q = Math.round(v * (1 - f * s));
        var t = Math.round(v * (1 - (1 - f) * s));
        var mod = i % 6;
        if (mod == 0) return [v, t, p];
        else if (mod == 1) return [q, v, p];
        else if (mod == 2) return [p, v, t];
        else if (mod == 3) return [p, q, v];
        else if (mod == 4) return [t, p, v];
        else if (mod == 5) return [v, p, q];
    },
    rgbToHsv(r, g, b) {
        if (arguments.length === 1) {
            g = r.g, b = r.b, r = r.r;
        }
        var max = Math.max(r, g, b), min = Math.min(r, g, b),
            d = max - min,
            h,
            s = (max === 0 ? 0 : d / max),
            v = max / 255;

        switch (max) {
            case min: h = 0; break;
            case r: h = (g - b) + d * (g < b ? 6 : 0); h /= 6 * d; break;
            case g: h = (b - r) + d * 2; h /= 6 * d; break;
            case b: h = (r - g) + d * 4; h /= 6 * d; break;
        }

        return { h: h, s: s, v: v };
    },
    hexToHsv(hex) {
        if (!(typeof hex === "string") || hex.length !== 6)
            return;

        var n = parseInt(hex, 16);
        if (isNaN(n))
            return null;

        var r = (n >> 16) & 255,
            g = (n >> 8) & 255,
            b = n & 255;

        var hsv = this.rgbToHsv(r, g, b);
        return { h: hsv.h * 360, s: hsv.s, v: hsv.v };
    },
    rgbToHex(r, g, b) {
        return (((r & 255) << 16) | ((g & 255) << 8) | (b & 255)).toString(16);
    }
}
