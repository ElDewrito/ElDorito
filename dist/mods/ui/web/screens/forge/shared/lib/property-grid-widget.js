(function (dew) {
    let _uidCounter = 0;

    dew.makePropertyGrid = function (_elem) {
        var _items = [];
        var _inputTargets = {};
        var _hasFocus = false;
        var _callbacks = {};

        var _listWidget = dew.makeListWidget(_elem, {
            itemSelector: '.item',
            selectedClass: 'selected',
            hoverClass: 'selected',
            wrapAround: true,
            hoverSelection: true,
            mouseDownFilterSelector: 'select,input'
        });

        init();

        return {
            blur: onBlur,
            focus: onFocus,
            hasFocus: () => _hasFocus,
            createModel: makePropertyGridModel,
            setModel: setModel,
            refresh: refresh,
            on: function (event, callback) {
                if (!_callbacks[event])
                    _callbacks[event] = [];
                _callbacks[event].push(callback);
            }
        }

        function emit(event, data) {
            var eventCallbacks = _callbacks[event];
            if (!eventCallbacks)
                return;
            for (var i = 0; i < eventCallbacks.length; i++)
                eventCallbacks[i](data);
        }

        function setModel(model) {
            var selected = _listWidget.getSelected();
            var selectedIndex = selected ? selected.index : 0;

            _uidCounter = 0;
            _model = model;
            _items = [];
            _nodeMap = {};
            _inputTargets = {};

            render();
            _listWidget.refresh();
            _listWidget.setSelected(selectedIndex, false);

            if (selectedIndex < 0 || selectedIndex >= _items.length)
                return;

            if (_hasFocus) {
                emit('selectionChange', _items[selectedIndex]);
            }
        }

        function init() {
            _elem.addEventListener('input', function (e) {
                if (!_hasFocus) {
                    e.preventDefault();
                    return false;
                }

                if (e.target.type === 'text')
                    return;

                var item = _inputTargets[e.target.id];
                item.setValue(e.target.value);

            }, false);

            _elem.addEventListener('mousedown', function (e) {
                if (!_hasFocus) {
                    e.preventDefault();
                    return;
                }

                if (e.target.nodeName === 'SELECT') {
                    e.stopPropagation();
                    return false;
                }
            });

            _elem.addEventListener('change', function (e) {
                if (!_hasFocus) {
                    e.preventDefault();
                    return false;
                }

                if (e.target.type == 'text') {
                    var item = _inputTargets[e.target.id];
                    item.setValue(e.target.value);
                }
            }, false);

            _elem.addEventListener('keydown', function (e) {
                if (!_hasFocus) {
                    e.preventDefault();
                    return false;
                }

                if (e.keyCode == 40 || e.keyCode == 38) {
                    var item = _inputTargets[e.target.id];
                    if (!item)
                        return;

                    if (e.target.type && e.target.type == 'text') {
                        e.preventDefault();
                    }
                    item.setValue(e.target.value);
                }
            }, false);

            dew.ui.on('scroll', function (type, axis, direction) {
                if (!_hasFocus)
                    return;

                document.activeElement.blur();
                if (type < 2 && axis == 1) {
                    var selected = _listWidget.getSelected();
                    if (!selected)
                        return;
                    var item = _items[selected.index];
                    item.scroll(type, direction);
                }
            });

            _listWidget.on('selectionChange', (data) => {
                if (data.index < 0 || data.index >= _items.length)
                    return;
                var item = _items[data.index];
                if (!item)
                    return;

                emit('selectionChange', item);
            });

            _listWidget.on('select', function (action) {
                if (!_hasFocus)
                    return;

                document.activeElement.blur();
                var selected = _listWidget.getSelected();
                if (!selected)
                    return;
                var item = _items[selected.index];

                if (item.model.action) {
                    item.model.action(item);
                }
            });
        }

        function onFocus() {
            _hasFocus = true;
            _listWidget.focus();
        }

        function onBlur() {
            _hasFocus = false;
            _listWidget.blur();
        }

        function renderItem(containerElem, model) {
            if (!model.type) {
                var labelNode = document.createElement('div');
                labelNode.classList.add('heading');
                labelNode.textContent = model.label;
                containerElem.appendChild(labelNode);
                for (var i = 0; i < model.data.properties.length; i++) {
                    var item = model.data.properties[i];
                    renderItem(containerElem, item);
                }
            }
            else {
                var renderData = PropertyItemViewFactory[model.type](model);
                var itemNode = document.createElement('div');
                var labelNode = document.createElement('div');

                labelNode.innerHTML = model.label;
                labelNode.classList.add('label');
                itemNode.classList.add('item');
                itemNode.appendChild(labelNode);
                if (renderData.rootElement)
                    itemNode.appendChild(renderData.rootElement);
                containerElem.appendChild(itemNode);

                _items.push(renderData);

                // bind the input targets so the change event handlers can update the values more efficiently
                for (var i = 0; i < renderData.inputTargets.length; i++) {
                    _inputTargets[renderData.inputTargets[i].id] = renderData;
                }
            }
        }

        function render() {
            if (!_model)
                return;
            _elem.innerHTML = '';
            var html = '';
            for (var i = 0; i < _model.properties.length; i++) {
                var itemModel = _model.properties[i];
                renderItem(_elem, itemModel);

            }
        }

        function refresh() {
            for (var i = 0; i < _items.length; i++) {
                _items[i].refresh();
            }
        }
    };

    var PropertyItemViewFactory = {
        action: function (model) {
            return {
                model: model,
                rootElement: null,
                inputTargets: [],
                setValue: (value) => { },
                scroll: (type, direction) => { },
                refresh: function () { }
            }
        },
        static: function (model) {
            var container = document.createElement('div');
            container.classList.add('item-editor');
            var static = document.createElement('div');
            container.appendChild(static);

            var data = {
                rootElement: container,
                inputTargets: [],
                model: model,
                value: model.getValue(),
                setValue: (value, notifyModel = true) => {
                    static.innerHTML = value;
                },
                scroll: (type, direction) => { },
                refresh: function () { this.setValue(model.getValue(), false) }
            }
            data.setValue(model.getValue(), false);
            return data;
        },
        color: function (model) {
            var container = document.createElement('div');
            container.classList.add('item-editor');
            var colorPreview = document.createElement('div');
            colorPreview.classList.add('color-preview');
            container.appendChild(colorPreview);

            var data = {
                rootElement: container,
                inputTargets: [],
                model: model,
                value: model.getValue(),
                setValue: (value, notifyModel = true) => {
                    if (value !== undefined) {
                        colorPreview.style.backgroundColor = `rgb(${value.r},${value.g},${value.b})`;
                        if (notifyModel) {
                            model.setValue(value);
                        }
                    }
                },
                scroll: (type, direction) => {
                },
                refresh: function () { this.setValue(model.getValue(), false) }
            }
            data.setValue(model.getValue(), false);
            return data;
        },
        spinner: function (model) {
            var container = document.createElement('div');
            container.classList.add('item-editor');
            var select = document.createElement('select');
            select.id = generateElementUID();
            container.appendChild(select);

            for (var i = 0; i < model.meta.length; i++) {
                var optionModel = model.meta[i];
                var option = document.createElement('option');
                if (optionModel.constructor === Object) {
                    option.value = optionModel.value;
                    option.text = optionModel.label
                }
                else {
                    option.text = optionModel;
                    option.value = optionModel;
                }
                select.appendChild(option);
            }

            var data = {
                model: model,
                rootElement: container,
                inputTargets: [select],
                setValue: (value, notifyModel = true) => {
                    if (value.constructor === String)
                        value = parseInt(value);
                    select.value = value;
                    if (notifyModel)
                        model.setValue(value);
                },
                scroll: (type, direction) => {
                    var amount = 1;
                    var newIndex = select.selectedIndex + direction * amount;
                    if (newIndex >= select.options.length) newIndex = 0;
                    if (newIndex < 0) newIndex = select.options.length - 1;
                    select.selectedIndex = newIndex;
                    model.setValue(parseInt(select.value));
                    dew.ui.playSound(dew.ui.Sounds.HorizontalNavigation);
                },
                refresh: function () { this.setValue(model.getValue(), false) }
            }

            data.setValue(model.getValue(), false);
            return data;
        },
        range: function (model) {
            var range = document.createElement('input');
            var input = document.createElement('input');
            input.setAttribute('type', 'text');
            input.setAttribute('min', model.meta.min);
            input.setAttribute('max', model.meta.max);
            input.setAttribute('step', model.meta.step);
            input.id = generateElementUID();
            range.setAttribute('type', 'range');
            range.setAttribute('min', model.meta.min);
            range.setAttribute('max', model.meta.max);
            range.setAttribute('step', model.meta.step);
            range.id = generateElementUID();

            var container = document.createElement('div');
            container.classList.add('item-editor');
            container.appendChild(input);
            container.appendChild(range);

            var data = {
                model: model,
                rootElement: container,
                inputTargets: [range, input],
                setValue: (value, notifyModel = true) => {
                    if (value.constructor === String) {
                        value = parseFloat(value);
                    }

                    if (value > model.meta.max)
                        value = model.meta.max;
                    if (value < model.meta.min || isNaN(value))
                        value = model.meta.min;

                    if (model.meta.isInteger) {
                        value = Math.floor(value);
                        range.value = value;

                        if(model.meta.displayTextOverride) {
                            input.value = model.meta.displayTextOverride(value);
                        } else {
                            input.value = value;
                        }
                    } else {
                        range.value = value;
                        if(model.meta.displayTextOverride) {
                            input.value = model.meta.displayTextOverride(value);
                        } else {
                            input.value = value.toFixed(2);
                        }
                    }

                    if (notifyModel)
                        model.setValue(value);
                },
                scroll: (type, direction) => {
                    var amount = type == 1 ? model.meta.step * 4 : model.meta.step;
                    var value = parseFloat(range.value);
                    value += amount * direction;

                    data.setValue(value);
                    dew.ui.playSound(dew.ui.Sounds.HorizontalNavigation);
                },
                refresh: function () { this.setValue(model.getValue(), false) }
            };
            data.setValue(model.getValue(), false);

            return data;
        }
    }

    function generateElementUID() {
        return Math.floor((1 + Math.random()) * 0x10000).toString(16) + _uidCounter++;
    }

    function makePropertyGridModel() {
        var _properties = [];
        return {
            properties: _properties,
            add: function (data) {
                _properties.push(data);
            },
            group: function (label, buildModelFn) {
                var groupModel = {
                    label: label,
                    data: makePropertyGridModel()
                };
                buildModelFn(groupModel.data);
                _properties.push(groupModel);
            }
        }
    };

})(dew);

