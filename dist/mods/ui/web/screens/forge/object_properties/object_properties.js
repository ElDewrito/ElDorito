(function () {
    let _containerElement = document.getElementById('main_container');
    let _windowElement = document.getElementById('main_window');
    let _descriptionElement = _windowElement.querySelector('.window-info-box');
    let _titleElement = _windowElement.querySelector('.window-title');
    let _subtitleElement = _windowElement.querySelector('.window-subtitle');
    let _screenManager = dew.makeScreenManager();
    let _hidden = false;
    let _lastObjectIndex = -1;
    let _recentMaterials = [];

    _screenManager.setModel({
        main: makeMainScreen(_screenManager),
        color_picker: makeColorPickerScreen(_screenManager),
        summary: makeSummaryScreen(_screenManager),
        material_picker: makeMaterialCategoryScreen(_screenManager),
        material_picker_list: makeMaterialListScreen(_screenManager),
        engine_flags: makeEngineFlagsScreen(_screenManager),
        save_prefab: makeSavePrefabScreen(_screenManager)
    });
    _screenManager.push('main');

    document.addEventListener('mousedown', function (e) {
        if (e.target == _containerElement) {
            e.preventDefault();
            e.stopPropagation();
            hide();
        }
    }, false);

    dew.ui.on('action', function (action) {
        switch (action) {
            case dew.ui.Actions.X:
                hide();
                break;
        }
    });

    function makeMainScreen(screenManager) {
        let _screenElement = document.getElementById('main_screen');
        let _propertryGrid = dew.makePropertyGrid(document.querySelector('.property-grid'));
        let _active = false;
        let _materialsDOM = null;
        let _data = null;
        let _items = null;
        let _lastSelected = false;

        dew.on('show', e => {
            if (_hidden && _lastObjectIndex === e.data.object_index && _lastSelected === e.data.is_selected)
                return;
            _lastSelected = e.data.is_selected;
            _hidden = false;
            _lastObjectIndex = e.data.object_index;

            _screenManager.popAll(false);

            _data = e.data;
            fetchMaterials((err, materials) => {
                _propertryGrid.setModel(buildModel(_data));
            });

            fetchItems((err, items) => {
                var item = _items[_data.tag_index] || { name: 'Unknown', type: 'Unknown', tagIndex: _data.tag_index };
                setSubtitle(`${item.name} [${item.type}] [${item.tagIndex.toString(16)}]`);
            });
        });

        _propertryGrid.on('selectionChange', item => setDescription(item.model.description));

        return {
            activate: function () {
                _active = true;
                _screenElement.classList.add('active');
                setTitle('');
                _propertryGrid.focus();
            },
            deactivate: function (props) {
                if (!props.noHide)
                    _screenElement.classList.remove('active');
                _active = false;
                _propertryGrid.blur();
            },
            onAction: function ({ action }) {
                switch (action) {
                    case dew.ui.Actions.X:
                        hide();
                        break;
                    case dew.ui.Actions.B:
                        dew.ui.playSound(dew.ui.Sounds.B);
                        addRecentMaterial();
                        dew.hide();
                        break;
                }
            }
        }

        function onPropertyChange(properties) {
            Object.assign(_data.properties, properties);
            dew.callMethod('forgeaction', {
                type: 1,
                data: properties
            });
        }

        function buildModel(data) {
            if (!data)
                return;

            let properties = data.properties;

            function makeProperty(name, label, type, meta, description) {
                return {
                    label: label,
                    type: type,
                    getValue: () => properties[name],
                    setValue: (value) => onPropertyChange({ [name]: value }),
                    meta: meta,
                    description: description
                };
            }

            let isGarbageVolume = data.tag_index == 0x00005A8F;
            let isKillVolume = data.tag_index == 0x00005A8E;
            let isMapModifier = data.tag_index == 0x5728;
            let hasSpawn = false, hasSymmetry = false,
                hasZone = false, hasTeam = false,
                hasSpawnOrder = false,
                isTeleporter = false;

            switch (data.object_type_mp) {
                case 0x0://ordinary
                case 0x1://weapon
                case 0x2://grenade
                case 0x3://projectile
                case 0x4://powerup
                case 0x5://equipment
                case 0x6://land vehicle
                case 0x7://heavy land vehicle
                case 0x8://flying vehicle
                    hasSpawn = true;
                    hasSymmetry = true;
                    break;
                case 0xD://player respawn zone
                    hasSpawnOrder = true;
                    hasZone = true;
                    hasTeam = true;
                    hasSymmetry = true;
                    break;
                case 0x9://teleporter two way
                case 0xA://teleporter sender
                case 0xB://teleporter receiver
                    hasZone = true;
                    isTeleporter = true;
                    hasSymmetry = true;
                    break;
                case 0x16://vip dest objective
                    hasSymmetry = true;
                case 0x10://hold dest objective
                case 0x13://infection haven
                    hasSpawn = true;
                case 0x12://hill objective
                case 0x17://juggernaut objective
                    hasZone = true;
                    hasSpawnOrder = true;
                    break;
                case 0x11://capture dest objective
                case 0xF://capture spawn objective
                case 0x14://territory objective
                case 0x15://vip boundry objective
                case 0xC://player spawn location
                    hasTeam = true;
                    hasSymmetry = true;
                    break;
            }

            if (isGarbageVolume || isKillVolume) {
                hasZone = true;
            }

            if(isKillVolume) {
                hasTeam = true;
            }

            if(data.is_screenfx)
                hasTeam = true;

            if(data.has_material || isMapModifier)
                hasSpawn = false;

            let model = _propertryGrid.createModel();

            model.add({
                type: 'action', label: '{Summary}', meta: {},
                action: (item) => {
                    screenManager.push('summary', {
                        data: data.budget
                    });
                }
            });

            if (!data.is_selected) {
                model.add({
                    type: 'action', label: '{Select All}', meta: {}, action: () => {
                        dew.command('Forge.SelectAll');
                        data.is_selected = true;
                        _propertryGrid.setModel(buildModel(_data));
                    },
                    description: 'Selects all of this item on the map'
                });

            } else {
                model.add({
                    type: 'action', label: '{Deselect All}', meta: {}, action: () => {
                        dew.command('Forge.DeselectAllOf');
                        data.is_selected = false;
                        _propertryGrid.setModel(buildModel(_data));
                    },
                    description: 'Deselect all of this item on the map'
                });
                model.add({
                    type: 'action', label: '{Save Prefab}', meta: {}, action: () => {
                        screenManager.push('save_prefab', { noHide: true });
                    },
                    description: 'Save the selected objects'
                });
            }

            model.group('General', model => {
                model.add({
                    type: 'action', label: '{Gametypes}', meta: {},
                    action: (item) => {
                        screenManager.push('engine_flags', {
                            engineFlags: properties.engine_flags,
                            onChange: (flags) => {
                                onPropertyChange({ ['engine_flags']: flags });
                            }
                        });
                    },
                    description: 'This determines which gametypes this item will spawn in'
                });

                if (hasSpawn) {
                    model.add(makeProperty('respawn_rate', 'Respawn Rate', 'spinner', [
                        { label: 'Never', value: 0 },
                        { label: '1', value: 1 },
                        { label: '3', value: 3 },
                        { label: '5', value: 5 },
                        { label: '10', value: 10 },
                        { label: '15', value: 15 },
                        { label: '20', value: 20 },
                        { label: '30', value: 30 },
                        { label: '45', value: 45 },
                        { label: '60', value: 60 },
                        { label: '90', value: 90 },
                        { label: '120', value: 120 },
                        { label: '150', value: 150 },
                        { label: '180', value: 180 }
                    ], ' This controls how many seconds it will take for this type of item to respawn on the map.'))
                    model.add(makeProperty('on_map_at_start', 'Placed at Start', 'spinner',
                        [{ label: 'No', value: 0 }, { label: 'Yes', value: 1 }], 'This controls whether the item is on the map from the start of the game.'));
                }

                if (data.has_spare_clips)
                    model.add(makeProperty('spare_clips', 'Spare Clips', 'spinner', [0, 1, 2, 3, 4], 'This controls how many clips of ammunition are available for this item.'))

                if (hasTeam) {
                    model.add(makeProperty('team_affiliation', 'Team', 'spinner', [
                        { label: 'Neutral', value: 8 },
                        { label: 'Defender', value: 0 },
                        { label: 'Attacker', value: 1 },
                        { label: '3rd Team', value: 2 },
                        { label: '4th Team', value: 3 },
                        { label: '5th Team', value: 4 },
                        { label: '6th Team', value: 5 },
                        { label: '7th Team', value: 6 },
                        { label: '8th Team', value: 7 },
                    ], 'This item will belong to the selected team.'));
                }

                if (hasSpawnOrder) {
                    model.add(makeProperty('spawn_order', 'Spawn Order', 'spinner', [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10], 'This allows you to set the order in which this item becomes active.'))
                }

                if (hasSymmetry) {
                    model.add(makeProperty('symmetry', 'Symmetry', 'spinner', [
                        { label: 'Both', value: 0 },
                        { label: 'Symetric', value: 1 },
                        { label: 'Asymmetric', value: 2 }
                    ], 'This controls whether this object spawns in symmetric games, asymmetric games, or both.'))
                }
            });


            if (!hasZone && !data.has_material ) {
                model.add(makeProperty('physics', 'Physics', 'spinner',
                    [{ label: 'Default', value: 0 }, { label: 'Phased', value: 1 }], 'Overrides this items physical properties'));
            }

            
            if (isMapModifier) {
                let barrierOptions = [
                    { label: 'Disabled', value: 1 },
                    { label: 'Enabled', value: 0 }
                ];
                model.group('Map options', (m) => {
                    m.add(makeProperty('map_disable_push_barrier', 'Push Barrier', 'spinner', barrierOptions));
                    m.add(makeProperty('map_disable_death_barrier', 'Death Barrier', 'spinner', barrierOptions));
                    m.add({
                        type: 'range',
                        label: 'Gravity',
                        meta: { min: 0, max: 1, step: 0.01 },
                        getValue: () => properties.map_physics_gravity,
                        setValue: (value) => onPropertyChange({ ['map_physics_gravity']: 1.0-value })
                    });
                });

                let cameraFxRange = { min: 0, max: 1.0, step: 0.01 };
                model.group('Camera FX', (m) => {
                    m.add(makeProperty('camera_fx_exposure', 'Exposure', 'range', cameraFxRange));
                    m.add(makeProperty('camera_fx_light_intensity', 'Light Intensity', 'range', cameraFxRange));
                    m.add(makeProperty('camera_fx_bloom', 'Bloom', 'range', cameraFxRange));
                    m.add(makeProperty('camera_fx_light_bloom', 'Light Bloom', 'range', { min: 0, max: 2.0, step: 0.01 }));
                });

                model.group('Atmosphere', (m) => {

                    var skyOptions = [ { label: 'Map Default', value: 0 }];
                    var weatherEffectOptions = [ { label: 'Map Default', value: 0 }];

                    for(var i = 0; i < data.options.skies.length; i++)
                        skyOptions.push({label: data.options.skies[i], value: i+1});

                    m.add({
                        type: 'spinner',
                        label: 'Skybox',
                        description: '',
                        meta: skyOptions,
                        getValue: () => properties.atmosphere_properties_skybox,
                        setValue: (value) =>  {
                            onPropertyChange({ ['atmosphere_properties_skybox']: value });
                            _propertryGrid.setModel(buildModel(_data));
                        }
                    });
                    
                    if(properties.atmosphere_properties_skybox) {
                        m.add({
                            type: 'spinner',
                            label: 'Skybox Transform',
                            description: '',
                            meta: [
                                { label: 'Default', value: 0 },
                                { label: 'Custom', value: 1 }
                            ],
                            getValue: () => properties.atmosphere_properties_skybox_override_transform,
                            setValue: (value) =>  {
                                onPropertyChange({ ['atmosphere_properties_skybox_override_transform']: value });
                                _propertryGrid.setModel(buildModel(_data));
                            }
                        });

                        if(properties.atmosphere_properties_skybox_override_transform) {
                            m.add({
                                type: 'range',
                                label: 'Skybox Z',
                                description: '',
                                meta: { min: -500.0, max: 500.0, step: 0.1,  },
                                getValue: () => properties.atmosphere_properties_skybox_offset_z * 500,
                                setValue: (value) => {
                                    value /= 500;
                                    onPropertyChange({ ['atmosphere_properties_skybox_offset_z']: value });
                                }
                            })
                        }
                    }
                   
                    for(var i = 0; i < data.options.weather_effects.length; i++)
                        weatherEffectOptions.push({label: data.options.weather_effects[i], value: i+1})
                    m.add(makeProperty('atmosphere_properties_weather', 'Weather', 'spinner', weatherEffectOptions));
    
                    m.add({
                        type: 'spinner',
                        label: 'Fog',
                        description: '',
                        meta: [
                            { label: 'Map Default', value: 0 },
                            { label: 'Custom', value: 1 }
                        ],
                        getValue: () => properties.atmosphere_properties_enabled,
                        setValue: (value) =>  {
                            onPropertyChange({ ['atmosphere_properties_enabled']: value });
                            _propertryGrid.setModel(buildModel(_data));
                        }
                    });

                    if(properties.atmosphere_properties_enabled) {
                        m.add(makeProperty('atmosphere_properties_brightness', 'Fog Brightness', 'range', { min: 0, max: 1, step: 0.01 }));
                        m.add(makeProperty('atmosphere_properties_fog_density', 'Fog Density', 'range', { min: 0, max: 1, step: 0.01 }));
                        m.add(makeProperty('atmosphere_properties_fog_visibility', 'Fog Visibility', 'range', { min: 0, max: 1, step: 0.01 }));
                        m.add({
                            type: 'color',
                            label: 'Fog Color',
                            description: '',
                            meta: {},
                            getValue: () => {
                                return {
                                    r: Math.floor(properties.atmosphere_properties_fog_color_r * 255),
                                    g: Math.floor(properties.atmosphere_properties_fog_color_g * 255),
                                    b: Math.floor(properties.atmosphere_properties_fog_color_b * 255)
                                };
                            },
                            setValue: (color) => {
                                onPropertyChange({
                                    atmosphere_properties_fog_color_r: color.r / 255,
                                    atmosphere_properties_fog_color_g: color.g / 255,
                                    atmosphere_properties_fog_color_b: color.b / 255
                                })
                            },
                            action: (item) => {
                                screenManager.push('color_picker', {
                                    title: 'Fog Color',
                                    initialColor: item.model.getValue(),
                                    onColorSelected: item.setValue
                                });
                            },
                        });
                    }
                });  
            }

            if (isTeleporter) {
                model.add(makeProperty('teleporter_channel', 'Channel', 'spinner', [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]))
            }

            if (data.has_material) {

                model.group('Material', m => {
                    let supportsColor = materialSupportsChangeColor(properties.reforge_material);
                    let oldMaterialIndex = properties.reforge_material;

                    let allMaterials = [];
                    let materialNodes = _materialsDOM.getElementsByTagName('item');
                    for (let i = 0; i < materialNodes.length; i++) {
                        let node = materialNodes[i];
                        allMaterials.push({
                            label: node.getAttribute('name'),
                            value: parseInt(node.getAttribute('value'))
                        });
                    }

                    m.add(Object.assign(makeProperty('reforge_material', 'Material', 'spinner', allMaterials, 'Material for this object'), {
                        action: (item) => {
                            addRecentMaterial();
                            screenManager.push('material_picker', {
                                materials: _materialsDOM,
                                onMaterialSelected: (material) => {
                                    let props = { ['reforge_material']: material.value };
                                    if (material.color) {
                                        Object.assign(props, {
                                            reforge_material_color_r: material.color.r / 255,
                                            reforge_material_color_g: material.color.g / 255,
                                            reforge_material_color_b: material.color.b / 255
                                        });
                                    }
                                    onPropertyChange(props);
                                    _propertryGrid.setModel(buildModel(_data));
                                }
                            });
                        },
                        setValue: (value) => {
                            onPropertyChange({ ['reforge_material']: value });
                            const specialMaterials = new Set([119,120,121,162,163]);
                            if (specialMaterials.has(properties.reforge_material) || specialMaterials.has(oldMaterialIndex)) {
                                _propertryGrid.setModel(buildModel(_data));
                            }
                        }
                    }));

                    if (supportsColor) {
                        m.add({
                            type: 'color',
                            label: 'Material Color',
                            description: 'Material color for this object',
                            meta: {},
                            getValue: () => {
                                return {
                                    r: Math.floor(properties.reforge_material_color_r * 255),
                                    g: Math.floor(properties.reforge_material_color_g * 255),
                                    b: Math.floor(properties.reforge_material_color_b * 255)
                                };
                            },
                            setValue: (color) => {
                                onPropertyChange({
                                    reforge_material_color_r: color.r / 255,
                                    reforge_material_color_g: color.g / 255,
                                    reforge_material_color_b: color.b / 255
                                })
                            },
                            action: (item) => {
                                screenManager.push('color_picker', {
                                    title: 'Material Color',
                                    initialColor: item.model.getValue(),
                                    onColorSelected: item.setValue
                                });
                            },
                        });
                    } else {
                        m.add({
                            type: 'spinner',
                            label: 'Texture Mode',
                            description: 'Texture mode to use for this object',
                            meta: [
                                { label: 'Default', value: 0 },
                                { label: 'Custom', value: 1 }
                            ],
                            getValue: () => properties.reforge_material_tex_override,
                            setValue: (value) =>  {
                                onPropertyChange({ ['reforge_material_tex_override']: value });
                                _propertryGrid.setModel(buildModel(_data));
                            }
                        });
                        if(properties.reforge_material_tex_override) {
                            m.add(makeProperty('reforge_material_tex_scale', 'Texture Scale', 'range', { min: 0, max: 4.0, step: 0.01 }));
                            m.add(makeProperty('reforge_material_tex_offset_x', 'Texture Offset X', 'range', { min: 0, max: 1.0, step: 0.01 }));
                            m.add(makeProperty('reforge_material_tex_offset_y', 'Texture Offset Y', 'range', { min: 0, max: 1.0, step: 0.01 }));
                        }
                    }

                    m.add(makeProperty('reforge_material_allows_projectiles', 'Projectile Passthough', 'spinner', [
                        { label: 'Disabled', value: 0 },
                        { label: 'Enabled', value: 1 },
                    ], 'This determines whether projectiles (bullets, grenades) will pass through this item.'));

                })

            }

            if (hasZone) {
                model.group('Zone', (m) => {
                    m.add({
                        label: 'Shape',
                        type: 'spinner',
                        description: 'This determines the shape of the zone for this item.',
                        meta: [
                            { label: 'Sphere', value: 1 },
                            { label: 'Cylinder', value: 2 },
                            { label: 'Box', value: 3 }
                        ],
                        getValue: () => properties.shape_type,
                        setValue: (value) => {
                            onPropertyChange({ ['shape_type']: value });
                            _propertryGrid.setModel(buildModel(_data));
                        }
                    });

                    switch (properties.shape_type) {
                        case 3:
                            m.add(makeProperty('shape_width', 'Width', 'range', { min: 0.1, max: 100, step: 0.1 }, 'This determines how wide the zone is for this item.'));
                            m.add(makeProperty('shape_depth', 'Depth', 'range', { min: 0.1, max: 100, step: 0.1 }, 'This determines how deep the zone is for this item.'));
                            m.add(makeProperty('shape_top', 'Top', 'range', { min: 0.0, max: 100, step: 0.1 }, 'This determines where the top of zone is for this item.'));
                            m.add(makeProperty('shape_bottom', 'Bottom', 'range', { min: 0.0, max: 100, step: 0.1 }, 'This determines where the bottom of zone is for this item.'));
                            break;
                        case 2:
                            m.add(makeProperty('shape_top', 'Top', 'range', { min: 0.0, max: 100, step: 0.1 }, 'This determines where the top of zone is for this item.'));
                            m.add(makeProperty('shape_bottom', 'Bottom', 'range', { min: 0.0, max: 100, step: 0.1 }, 'This determines where the bottom of the zone is for this item.'));
                        case 1:
                            m.add(makeProperty('shape_radius', 'Radius', 'range', { min: 0.1, max: 100, step: 0.1 }, 'This determines the radius of the zone for this item.'));
                            break;
                    }
                });
            }

            if (data.is_light) {
                model.group('Light', (m) => {

                    m.add({
                        type: 'spinner',
                        label: 'Type',
                        description: '',
                        meta: [
                            { label: 'Point', value: 0 },
                            { label: 'Directional', value: 1}
                        ],
                        getValue: () => properties.light_type,
                        setValue: (value) =>  {
                            onPropertyChange({ ['light_type']: value });
                            _propertryGrid.setModel(buildModel(_data));
                        }
                    });

                    if(properties.light_type) {
                        m.add(makeProperty('light_field_of_view', 'Field of View', 'range', { min: 0, max: 180, step: 0.01 }));
                        //m.add(makeProperty('light_near_width', 'Near Width', 'range', { min: 0, max: 1.0, step: 0.01 }));
                    }

                    m.add(makeProperty('light_radius', 'Radius', 'range', { min: 0, max: 50, step: 0.01 }));
                    m.add(makeProperty('light_intensity', 'Intensity', 'range', { min: 0, max: 2.0, step: 0.01 }));

                    m.add({
                        type: 'color',
                        label: 'Light Color',
                        description: '',
                        meta: {},
                        getValue: () => {
                            return {
                                r: Math.floor(properties.light_color_r * 255),
                                g: Math.floor(properties.light_color_g * 255),
                                b: Math.floor(properties.light_color_b * 255)
                            };
                        },
                        setValue: (color) => {
                            onPropertyChange({
                                light_color_r: color.r / 255,
                                light_color_g: color.g / 255,
                                light_color_b: color.b / 255
                            });
                        },
                        action: (item) => {
                            screenManager.push('color_picker', {
                                title: 'Light Color',
                                initialColor: item.model.getValue(),
                                onColorSelected: item.setValue
                            });
                        },
                    });
                    
                    m.add({
                        type: 'spinner',
                        label: 'Function',
                        description: 'Illumination function',
                        meta: [
                            { label: 'None', value: 0 },
                            { label: 'Pulse', value: 1 },
                            { label: 'Flicker', value: 2 }
                        ],
                        getValue: () => properties.light_illumination_function_type,
                        setValue: (value) =>  {
                            onPropertyChange({ ['light_illumination_function_type']: value });
                            _propertryGrid.setModel(buildModel(_data));
                        }
                    });
                    if(properties.light_illumination_function_type) {
                        m.add(makeProperty('light_illumination_function_base', 'Base', 'range', { min: 0.0, max: 1.0, step: 0.01 }, 'The minumum percentage of intensity'));
                        m.add(makeProperty('light_illumination_function_freq', 'Frequency', 'range',{ min: 0.0, max: 2.0, step: 0.01 }, 'The rate of change'));
                    }
                });
            }

            if (data.is_screenfx) {
                model.group('Screen Fx', (m) => {
                    let normalRange = { min: 0, max: 1, step: 0.01 };
                    m.add(makeProperty('fx_range', 'Range', 'range', { min: 0, max: 255, step: 1 }));
                    m.add(makeProperty('fx_light_intensity', 'Light Intensity', 'range', normalRange));
                    m.add(makeProperty('fx_saturation', 'Saturation', 'range', normalRange));
                    m.add(makeProperty('fx_desaturation', 'Desaturation', 'range', normalRange));
                    m.add(makeProperty('fx_gamma_inc', 'Gamma Decrease', 'range', normalRange));
                    m.add(makeProperty('fx_gamma_dec', 'Gamma Increase', 'range', normalRange));
                    m.add(makeProperty('fx_tracing', 'Tracing', 'range', normalRange));
                    m.add({
                        type: 'color',
                        label: 'Filter Color',
                        description: '',
                        meta: {},
                        getValue: () => {
                            return {
                                r: Math.floor(properties.fx_color_filter_r * 255),
                                g: Math.floor(properties.fx_color_filter_g * 255),
                                b: Math.floor(properties.fx_color_filter_b * 255)
                            };
                        },
                        setValue: (color) => {

                            onPropertyChange({
                                fx_color_filter_r: color.r / 255,
                                fx_color_filter_g: color.g / 255,
                                fx_color_filter_b: color.b / 255
                            });
                        },
                        action: (item) => {
                            screenManager.push('color_picker', {
                                title: 'Filter Color',
                                initialColor: item.model.getValue(),
                                onColorSelected: (color) => {
                                    item.setValue(color);
                                }
                            });
                        },

                    });

                    m.add({
                        type: 'color',
                        label: 'Floor Color',
                        description: '',
                        meta: {},
                        getValue: () => {
                            return {
                                r: Math.floor(properties.fx_color_floor_r * 255),
                                g: Math.floor(properties.fx_color_floor_g * 255),
                                b: Math.floor(properties.fx_color_floor_b * 255)
                            };
                        },
                        setValue: (color) => {
                            onPropertyChange({
                                fx_color_floor_r: color.r / 255,
                                fx_color_floor_g: color.g / 255,
                                fx_color_floor_b: color.b / 255
                            });
                        },
                        action: (item) => {
                            screenManager.push('color_picker', {
                                title: 'Floor Color',
                                initialColor: item.model.getValue(),
                                onColorSelected: (color) => {
                                    item.setValue(color);
                                }
                            });
                        },

                    });
                });
            }

            if (isGarbageVolume) {
                let collectionOptions = [{ label: 'Collect', value: 1 }, { label: 'Ignore', value: 0 }];

                model.group('Garbage Collection', (m) => {
                    m.add(makeProperty('garbage_volume_interval', 'Delay', 'spinner', [
                        { label: 'Instant', value: 0 },
                        { label: '3 Seconds', value: 1 },
                        { label: '5 Seconds', value: 2 },
                        { label: '10 Seconds', value: 3 },
                    ], 'The time delay before collection'));
                    m.add(makeProperty('garbage_volume_collect_dead_biped', 'Dead Bodies', 'spinner', collectionOptions));
                    m.add(makeProperty('garbage_volume_collect_weapons', 'weapons', 'spinner', collectionOptions));
                    m.add(makeProperty('garbage_volume_collect_objectives', 'Objectives', 'spinner', collectionOptions));
                    m.add(makeProperty('garbage_volume_collect_grenades', 'Grenades', 'spinner', collectionOptions));
                    m.add(makeProperty('garbage_volume_collect_equipment', 'Equipment', 'spinner', collectionOptions));
                    m.add(makeProperty('garbage_volume_collect_vehicles', 'Vehicles', 'spinner', collectionOptions));
                });
            }

            if (isKillVolume) {
                model.group('Kill Volume', (m) => {
                    m.add(makeProperty('kill_volume_always_visible', 'Visibility', 'spinner',
                        [{ label: 'Never', value: 0 }, { label: 'Always', value: 1 }], 'Determines whether the volume will be visible outside of forge'), );
                    m.add(makeProperty('kill_volume_destroy_vehicles', 'Vehicles', 'spinner',
                        [{ label: 'Ignore', value: 0 }, { label: 'Destroy', value: 1 }], 'Destroys vehicles'));
                      m.add(makeProperty('kill_volume_damage_cause', 'Damage Cause', 'spinner',
                        [{ label: 'Default', value: 0 }, { label: 'Guardians', value: 1 },  { label: 'Falling', value: 2 }], 'Determines the cause of damage'));
                });
            }

            return model;
        }

        function fetchMaterials(callback) {
            if (_materialsDOM)
                return callback(null, _materialsDOM);
            let xhr = new XMLHttpRequest();
            xhr.open('GET', './materials.xml', true);
            xhr.onload = function (e) {
                if (this.status === 200) {
                    let domParser = new DOMParser();
                    _materialsDOM = domParser.parseFromString(this.responseText, "text/xml");
                    callback(null, _materialsDOM);
                } else {
                    callback({ code: this.status, message: this.statusText });
                }
            }
            xhr.send();
        }

        function fetchItems(callback) {
            if (_items)
                return callback(null, _items);

            var items = {};
            let xhr = new XMLHttpRequest();
            xhr.open('GET', './../object_creation/items.xml', true);
            xhr.onload = function (e) {
                if (this.status === 200) {
                    let domParser = new DOMParser();
                    let dom = domParser.parseFromString(this.responseText, "text/xml");
                    var flattened = flattenItems(dom);

                    for (let item of flattened) {
                        if (items[item.tagIndex]) {
                            items[item.tagIndex].name = 'Custom';
                            continue;
                        }
                        items[item.tagIndex] = item;
                    }
                    _items = items;
                    callback(null, items);
                } else {
                    callback({ code: this.status, message: this.statusText });
                }
            }
            xhr.send();
        }

        function flattenItems(itemDom) {
            let _result = [];
            let _pathStack = [];
            let _nodeVisitors = {
                item: visitItem,
                category: visitCategory
            }

            flatternInternal(itemDom.firstChild);
            return _result;

            function flatternInternal(node) {
                for (let x of node.children)
                    _nodeVisitors[x.nodeName](x);
            }

            function visitItem(node) {
                let name = node.getAttribute('name');
                let tagIndex = parseInt(node.getAttribute('tagindex'))
                let type = node.getAttribute('type');

                if (_pathStack.length > 2) {
                    _pathStack = _pathStack.slice(_pathStack.length - 2);
                }
                _result.push({
                    path: _pathStack,
                    name: name,
                    tagIndex: tagIndex,
                    type: type
                })
            }

            function visitCategory(node) {
                _pathStack.push(node.getAttribute('name'));
                flatternInternal(node);
                _pathStack.pop();
            }
        }


        function getMaterialName(materialIndex) {
            let items = _materialsDOM.getElementsByTagName('item');
            for (let item of items) {
                if (parseInt(item.getAttribute('value')) === materialIndex)
                    return item.getAttribute('name') || 'Unknown';
            }
            return null;
        }

        function addRecentMaterial() {

            let materialColor = {
                r: (_data.properties.reforge_material_color_r * 255) | 0,
                g: (_data.properties.reforge_material_color_g * 255) | 0,
                b: (_data.properties.reforge_material_color_b * 255) | 0
            };

            if (!_recentMaterials.find(x => x.value === _data.properties.reforge_material
                && (!x.color || (x.color.r === materialColor.r && x.color.g === materialColor.g && x.color.b === materialColor.b)))) {

                let materialName = getMaterialName(_data.properties.reforge_material);
                let recentItem = {
                    label: materialName,
                    value: _data.properties.reforge_material,
                };
                if (materialSupportsChangeColor(recentItem.value)) {
                    recentItem.color = materialColor;
                }

                _recentMaterials.unshift(recentItem);
            }
        }
    }

    function makeColorPickerScreen(screenManager) {
        let _screenElement = document.getElementById('color_picker_screen');
        let _active = false;
        let _onColorSelected;

        let _colorPicker = dew.makeColorPicker(_screenElement.querySelector('.color-picker'));
        dew.on('controllerinput', function (e) {
            if (!_active)
                return;
            _colorPicker.controllerInput(e.data);
        });

        _colorPicker.on('select', function (color) {
            let rgb = ColorUtil.hsvToRgb(color.h, color.s, color.v);
            _onColorSelected({ r: rgb[0], g: rgb[1], b: rgb[2] });
        });

        return {
            activate: function (props) {
                _active = true;
                _onColorSelected = props.onColorSelected;
                let c = ColorUtil.rgbToHsv(props.initialColor.r, props.initialColor.g, props.initialColor.b);
                _colorPicker.setColor({ h: c.h * 360, s: c.s, v: c.v });
                _screenElement.classList.add('active');
                if (props.title) {
                    setTitle(props.title);
                }
            },
            deactivate: function () {
                _active = false;
                _screenElement.classList.remove('active');
            },
            onAction: function ({ action }) {
                switch (action) {
                    case dew.ui.Actions.B:
                        dew.ui.playSound(dew.ui.Sounds.B);
                        screenManager.pop();
                        break;
                    case dew.ui.Actions.X:
                        hide();
                        break;
                }
            }
        }
    }

    function makeSummaryScreen(screenManager) {
        let _screenElement = document.querySelector('.summary-screen');
        let _grid = dew.makePropertyGrid(_screenElement.querySelector('.summary-property-grid'));
        let _active = false;
        let _summaryData = {};

        _grid.on('selectionChange', item => setDescription(item.model.description));

        return {
            activate: function (props) {
                _screenElement.classList.add('active');
                _active = true;
                _grid.focus();
                _summaryData = props.data;
                _grid.setModel(buildModel());
                setTitle('Summary');

            }, deactivate: function () {
                _screenElement.classList.remove('active');
                _active = false;
                _grid.blur();
            },
            onAction: function ({ action }) {
                switch (action) {
                    case dew.ui.Actions.X:
                        hide();
                        break;
                    case dew.ui.Actions.B:
                        dew.ui.playSound(dew.ui.Sounds.B);
                        screenManager.pop();
                        break;
                }
            }
        }

        function onPropertyChange(properties) {
            Object.assign(_summaryData, properties);
            dew.callMethod('forgeaction', {
                type: 1,
                data: properties
            });
        }

        function makeProperty(name, label, type, meta, description) {
            return {
                label: label,
                type: type,
                getValue: () => _summaryData[name],
                setValue: (value) => onPropertyChange({ [name]: value }),
                meta: meta,
                description: description
            };
        }

        function buildModel() {
            let model = _grid.createModel();
            model.add(makeProperty('summary_runtime_minimum', 'Runtime Min', 'range', { min: 0, max: _summaryData.summary_placed_on_map, step: 1, isInteger: true },
                'The item will automatically spawn if the number of this type of item ever falls below the minimum.'));
            model.add(makeProperty('summary_runtime_maximum', 'Runtime Max', 'range',
                { min: _summaryData.summary_placed_on_map, max: _summaryData.summary_maximum_allowed, step: 1, isInteger: true },
                'This sets the maximum number of this type of item that can be on the map.'));
            model.add(makeProperty('summary_placed_on_map', 'Placed On Map', 'static', {}, 'This displays how many of this type of item are currently placed on the map.'));
            model.add(makeProperty('summary_maximum_allowed', 'Design Time Max', 'static', {}, 'This displays the maximum number of this type of item that can ever be on the map.'));
            model.add(makeProperty('summary_total_cost', 'Total Cost', 'static', {}, 'This displays the total amount of budget that you are using for this type of item.'));
            return model;
        }
    }

    function makeMaterialCategoryScreen(screenManager) {
        let _screenElement = document.querySelector('#material_picker_screen');
        let _categoryTreeElement = _screenElement.querySelector('.material-category-tree');
        let _treeList = dew.makeTreeList(_categoryTreeElement);
        let _onMaterialSelected = null;

        _treeList.on('select', function ({ index, element, path }) {
            if (path.getElementsByTagName('category').length)
                return;

            let items = null;
            if (path.getAttribute('id') === 'recent') {
                items = _recentMaterials;
            }
            else {
                items = Array.prototype.slice.call(path.children)
                    .map(x => ({
                        label: x.getAttribute('name'),
                        value: parseInt(x.getAttribute('value'))
                    }));
            }
            screenManager.push('material_picker_list', { materials: items, onMaterialSelected: _onMaterialSelected });
        });

        return {
            activate: function (props) {
                _screenElement.classList.add('active');
                if (props && props.materials) {
                    _onMaterialSelected = props.onMaterialSelected;
                    _treeList.setSource(props.materials.firstChild);
                    _treeList.gotoRoot();
                }
                _treeList.focus();
                setTitle('Material');
            },
            deactivate: function () {
                _screenElement.classList.remove('active');
                _treeList.blur();
            },
            onAction: function ({ action }) {
                switch (action) {
                    case dew.ui.Actions.B:
                        if (_treeList.atRoot())
                            screenManager.pop();
                        break;
                    case dew.ui.Actions.X:
                        hide();
                        break;
                }
            }
        };
    }

    function makeMaterialListScreen(screenManager) {
        let _screenElement = document.querySelector('.material-picker-list-screen');
        let _materialListElement = _screenElement.querySelector('.material-list');
        let _materials = [];
        let _materialList = dew.makeListWidget(_materialListElement, {
            itemSelector: 'li',
            hoverClass: 'selected',
            hoverSelection: true,
            wrapAround: true
        });
        let _onMaterialSelected = null;

        _materialList.on('select', function ({ index, element }) {
            let materialIndex = parseInt(element.getAttribute('data-value'));
            if (_onMaterialSelected) {
                let item = _materials[index];
                _onMaterialSelected(item);
            }
        });

        return {
            activate: function (props) {
                _screenElement.classList.add('active');
                if (props.materials) {
                    _onMaterialSelected = props.onMaterialSelected;
                    _materials = props.materials;
                    renderMaterialList(_materialListElement);
                    _materialList.focus();
                    _materialList.setSelected(0);
                }
                else {
                    _materialList.focus();
                }
            },
            deactivate: function () {
                _screenElement.classList.remove('active');
                _materialList.blur();
            },
            onAction: function ({ action }) {
                switch (action) {
                    case dew.ui.Actions.B:
                        dew.ui.playSound(dew.ui.Sounds.B);
                        screenManager.pop();
                        break;
                    case dew.ui.Actions.X:
                        hide();
                        break;
                }
            }
        }

        function renderMaterialList(element) {
            let html = '';
            for (let i = 0; i < _materials.length; i++) {
                let item = _materials[i];
                html += `<li data-value="${item.value}">`;
                if (item.color) {
                    let colorHex = ((item.color.r << 16) | (item.color.g << 8) | item.color.b).toString(16).padStart(6, '0');
                    html += `<div>${item.label} - <span class="mute">#${colorHex}</span></div><div class="preview" style="background:#${colorHex}"></div>`;
                } else {
                    html += ` <div>${item.label}</div><div class="preview material-thumb material-thumb-${item.value}"></div>`;
                }
                html += `</li>`;
            }
            element.innerHTML = html;
        }
    }

    function makeEngineFlagsScreen(screenManager) {
        const engineNames = [
            'Capture The Flag',
            'Slayer',
            'Oddball',
            'King Of The Hill',
            'Juggernaut',
            'Territories',
            'Assault',
            'VIP',
            'Infection',
        ]

        let _screenElement = document.getElementById('engine_flags_screen');
        let _engineListElement = _screenElement.querySelector('.engine-list');
        let _onChange = null;
        let _engineFlags = 0;
        let _engineList = dew.makeListWidget(_engineListElement, {
            itemSelector: 'li',
            hoverClass: 'selected',
            hoverSelection: true,
            wrapAround: true
        });

        _engineList.on('select', function ({ index, element }) {
            let mask = (1 << index);
            if (_engineFlags & mask) _engineFlags &= ~mask;
            else _engineFlags |= mask;

            render();
            _engineList.refresh();
            _engineList.setSelected(index);

            if (_onChange)
                _onChange(_engineFlags);
        });

        return {
            activate: function (props) {
                _engineFlags = props.engineFlags;
                _onChange = props.onChange

                _screenElement.classList.add('active');
                render();
                _engineList.focus();
                _engineList.setSelected(0);

                setTitle('Gametypes');

            },
            deactivate: function () {
                _screenElement.classList.remove('active');
                _engineList.blur();
            },
            onAction: function ({ action }) {
                switch (action) {
                    case dew.ui.Actions.B:
                        dew.ui.playSound(dew.ui.Sounds.B);
                        screenManager.pop();
                        break;
                    case dew.ui.Actions.X:
                        hide();
                        break;
                }
            }
        }

        function render() {
            let html = '';
            for (let i = 0; i < 9; i++) {
                html += `<li class="${((1 << i) & _engineFlags) ? 'checked' : ''}"><div class="checkbox"></div><div>${engineNames[i]}</div></li>`;
            }
            _engineListElement.innerHTML = html;
        }
    }

    function makeSavePrefabScreen(screenManager) {
        let _screenElement = document.querySelector('#save_prefab_screen')
        let _overlay = document.querySelector('.modal-overlay');
        let _nameInputElement = _screenElement.querySelector('input');
        let _errorStatusElement = _screenElement.querySelector('.error');

        _screenElement.addEventListener("animationend", () => {
            _screenElement.classList.remove('shake');
        }, false);

        return {
            activate: function (props) {
                _screenElement.classList.add('active');
                _overlay.classList.remove('hidden');
                _nameInputElement.addEventListener('keydown', onKeyDown);
                _errorStatusElement.classList.add('hidden');
                _nameInputElement.focus();
            },
            deactivate: function () {
                _screenElement.classList.remove('active');
                _overlay.classList.add('hidden');
                _nameInputElement.value = '';
                _nameInputElement.removeEventListener('keydown', onKeyDown);
                _nameInputElement.blur();
            },
            onAction: function ({ inputType, action }) {
                switch (action) {
                    case dew.ui.Actions.A:
                        if (inputType === 'controller')
                            onConfirm();
                        break;
                    case dew.ui.Actions.B:
                        onReject();
                        break;
                }
            }
        };

        function onConfirm() {
            let value = _nameInputElement.value.trim();
            if (!value.length) {
                dispalyError("Prefab name can't be empty");
                return;
            }

            if (!value.match(/^[^\\/:\*\?"<>\|]+$/)) {
                dispalyError("Prefab name can't contain \\/:*?\"<>|");
                return;
            }

            dew.command(`Forge.SavePrefab "${_nameInputElement.value}"`)
                .then(response => {
                    showToast('Prefab Saved');
                    setTimeout(() => screenManager.pop(), 1);
                })
                .catch(error => dispalyError(error.message));
        }

        function onReject() {
            dew.ui.playSound(dew.ui.Sounds.B);
            screenManager.pop();
        }

        function onKeyDown(e) {
            if (e.which == 13)
                onConfirm();
        }

        function dispalyError(message) {
            _errorStatusElement.textContent = message;
            _screenElement.classList.add('shake');
            _errorStatusElement.classList.remove('hidden');
            dew.ui.playSound(dew.ui.Sounds.Error);
        }
    }

    function setTitle(text) {
        _titleElement.innerHTML = 'Object Properties ' + (text ? (' - ' + text) : '');
    }

    function setSubtitle(text) {
        _subtitleElement.innerHTML = text || '';
    }

    function setDescription(text) {
        _descriptionElement.innerHTML = text || '';
    }

    function hide() {
        _hidden = true;
        dew.ui.playSound(dew.ui.Sounds.B);
        dew.hide();
    }

    function showToast(message) {
        let toastElement = document.querySelector('.toast');
        toastElement.querySelector('div').innerHTML = message;
        toastElement.classList.add('toast-show');
        setTimeout(function () {
            toastElement.classList.remove('toast-show');
        }, 3000);
    }
    
    function materialSupportsChangeColor(materialIndex) {
        return materialIndex == 120 || materialIndex == 119 || materialIndex == 162 || materialIndex == 163
    }

})();