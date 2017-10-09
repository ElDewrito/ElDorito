var MultiplayerObjectType = 
{
  Ordinary : 0x0,
  Weapon : 0x1,
  Grenade : 0x2,
  Projectile : 0x3,
  Powerup : 0x4,
  Equipment : 0x5,
  LightLandVehicle : 0x6,
  HeavyLandVehicle : 0x7,
  FlyingVehicle : 0x8,
  Teleporter2Way : 0x9,
  TeleporterSender : 0xA,
  TeleporterReceiver : 0xB,
  PlayerSpawnLocation : 0xC,
  PlayerRespawnZone : 0xD,
  HoldSpawnObjective : 0xE,
  CaptureSpawnObjective : 0xF,
  HoldDestinationObjective : 0x10,
  CaptureDestinationObjective : 0x11,
  HillObjective : 0x12,
  InfectionHavenObjective : 0x13,
  TerritoryObjective : 0x14,
  VIPBoundaryObjective : 0x15,
  VIPDestinationObjective : 0x16,
  JuggernautDestinationObjective : 0x17,
};

var STRINGS = {
	object_properties: 'Object Properties',
	respawn_rate: 'Respawn Rate',
	spare_clips: 'Spare Clips',
	on_map_at_start: 'Place at Start',
	teleporter_channel: 'Channel',
	team_affiliation: 'Team',
	shape_type: 'Shape',
	shape_radius: 'Radius',
	shape_top: 'Top',
	shape_bottom: 'bottom',
	shape_width: 'Width',
	shape_depth: 'Depth',
	spawn_order: 'Spawn Order',
	symmetry: 'Symmetry',
	physics: 'Physics',
	general: 'General',
	coordinates: 'Coordinates',
	coordinates_pos_x: 'X',
	coordinates_pos_y: 'Y',
	coordinates_pos_z: 'Z',
	coordinates_yaw: 'Yaw',
	coordinates_pitch: 'Pitch',
	coordinates_roll: 'Roll',
	zone: ' ',
	appearance: 'Appearance',
	appearance_lightmap: 'Lightmap',
	appearance_material: 'Material',
	appearance_material_color_r: 'Color R',
	appearance_material_color_g: 'Color G',
    appearance_material_color_b: 'Color B',
	budget_screen_action: 'Summary',
	summary: 'Summary',
	summary_placed_on_map: 'placed on map',
	summary_runtime_minimum: 'run-time minimum',
	summary_runtime_maximum: 'run-time maximum',
	summary_maximum_allowed: 'maximum allowed',
	summary_total_cost: 'total cost',
	select_all: '[ select all ]',
	deselect_all: '[ deselect all ]',
	light: 'light',
	light_color_r: 'R',
	light_color_g: 'G',
	light_color_b: 'B',
	light_intensity: 'Intensity',
	light_radius: 'radius',
	light_color_intensity: 'Color Intensity',
};


var objectPropertyGridData = {
	strings: STRINGS,
	meta: {
		respawn_rate: [
			{name: 'Never', value: 0}, 
			{name: '1', value: 1},
			{name: '3', value: 5},
			{name: '5', value: 5},
			{name: '10', value: 10},
			{name: '15', value: 15},
			{name: '20', value: 20},
			{name: '30', value: 30},
			{name: '45', value: 45},
			{name: '60', value: 60},
			{name: '90', value: 90},
			{name: '120', value: 120},
			{name: '150', value: 150},
			{name: '180', value: 180}
		],
		spare_clips: [0,1,2,3,4],
		on_map_at_start: [{name: 'no', value: 0}, {name: 'yes', value: 1}],
		teleporter_channel: [
            {name:'0', value: 0,},
            {name:'1', value: 1 },
            {name:'2', value: 2 },
            {name:'3', value: 3 },
            {name:'4', value: 4 },
            {name:'5', value: 5 },
            {name:'6', value: 6 },
            {name:'7', value: 7 },
            {name:'8', value: 8 },
            {name:'9', value: 9 },
            {name:'10', value: 10,},
            {name:'11', value: 11 },
            {name:'12', value: 12 },
            {name:'13', value: 13 },
            {name:'14', value: 14 },
            {name:'15', value: 15 },
            {name:'Death', value: 255 }
		],
		team_affiliation: [
			{name: 'Neutral', value: 8},
			{name: 'Defender', value: 0},
			{name: 'Attacker', value: 1},
			{name: '3rd Team', value: 2},
			{name: '4th Team', value: 3},
			{name: '5th Team', value: 4},
			{name: '6th Team', value: 5},
			{name: '7th Team', value: 6},
			{name: '8th Team', value: 7},
		],
		shape_type: [
			{name: 'Sphere', value: 1},
			{name: 'Cylinder', value: 2},
			{name: 'Box', value: 3}
		],
		shape_radius: { min: 0.5, max: 30.0, step: 0.5 },
		shape_top: { min: 0.5, max: 30.0, step: 0.5 },
		shape_bottom: { min: 0.5, max: 30.0, step: 0.5 },
		shape_width: { min: 0.5, max: 30.0, step: 0.5 },
		shape_depth: { min: 0.5, max: 30.0, step: 0.5 },
		spawn_order: [0,1,2,3,4,5,6,7,8,9,10],
		symmetry: [
			{name: 'Both', value: 0},
			{name: 'Symetric', value: 1},
			{name: 'Asymmetric', value: 2}
		],
		lightmap: [
			{ name: 'On', value: 1 },
			{ name: 'Off', value: 0 }
			
		],
		appearance_material_color_r: { min: 0.0, max: 1.0, step: 0.01 },
		appearance_material_color_g: { min: 0.0, max: 1.0, step: 0.01 },
		appearance_material_color_b: { min: 0.0, max: 1.0, step: 0.01 },
        physics: [{name:'Default', value: 0}, {name:'Phased', value: 1}],
        light_color_r: { min: 0.0, max: 1.0, step: 0.01 },
        light_color_g: { min: 0.0, max: 1.0, step: 0.01 },
        light_color_b: { min: 0.0, max: 1.0, step: 0.01 },
        light_intensity: { min: 0.0, max: 1.0, step: 0.01 },
        light_color_intensity: { min: 0.0, max: 1.0, step: 0.01 },
        light_radius: { min: 0, max: 50, step: 0.01 },
        fx_range: { min: 1, max: 255, step: 1 },
        fx_light_intensity: { min: 0, max: 1, step: 0.01 },
        fx_hue: { min: 0, max: 1, step: 0.01 },
        fx_saturation: { min: 0, max: 1, step: 0.01 },
        fx_desaturation: { min: 0, max: 1, step: 0.01 },
        fx_color_filter_r: { min: 0, max: 1, step: 0.01 },
        fx_color_filter_g: { min: 0, max: 1, step: 0.01 },
        fx_color_filter_b: { min: 0, max: 1, step: 0.01 },
        fx_color_floor_r: { min: 0, max: 1, step: 0.01 },
        fx_color_floor_g: { min: 0, max: 1, step: 0.01 },
        fx_color_floor_b: { min: 0, max: 1, step: 0.01 },
        fx_gamma_inc: { min: 0, max: 1, step: 0.01 },
        fx_gamma_dec: { min: 0, max: 1, step: 0.01 },
        fx_tracing: { min: 0, max: 1, step: 0.01 }
	},
	properties: [
		{ name: 'budget_screen_action', type: 'action'},
		{ name: 'select_all', type: 'action'},
		{ name: 'deselect_all', type: 'action'},
		{
			name: 'general',
			values: [
				{ name: 'respawn_rate', type: 'spinner', meta:'respawn_rate' },
				{ name: 'spare_clips', type: 'spinner', meta:'spare_clips' },
				{ name: 'on_map_at_start', type: 'spinner', meta:'on_map_at_start'  },
				{ name: 'team_affiliation', type: 'spinner', meta:'team_affiliation' },
				{ name: 'spawn_order', type: 'spinner', meta:'spawn_order' },
				{ name: 'symmetry', type: 'spinner', meta:'symmetry' }, 
				{ name: 'physics', type: 'spinner', meta:'physics'},
				{ name: 'appearance_material', type: 'material', meta:'appearance_material'},
                { name: 'appearance_material_color_r', type: 'range', meta:'appearance_material_color_r'},
                { name: 'appearance_material_color_g', type: 'range', meta:'appearance_material_color_g'},
                { name: 'appearance_material_color_b', type: 'range', meta:'appearance_material_color_b'},
				
				
			]
		},
		{
			name: 'zone',
			values: [
				{ name: 'teleporter_channel', type: 'spinner', meta:'teleporter_channel' },
				{ name: 'shape_type', type: 'spinner', meta:'shape_type' },
				{ name: 'shape_radius', type: 'range', meta:'shape_radius' },
				{ name: 'shape_top', type: 'range', meta:'shape_top' },
				{ name: 'shape_bottom', type: 'range', meta:'shape_top' },
				{ name: 'shape_width', type: 'range', meta:'shape_width' },
				{ name: 'shape_depth', type: 'range', meta:'shape_depth' }
			]
		},
		{
			name: 'light',
			values: [
				{ name: 'light_radius', type: 'range', meta:'light_radius'},
				{ name: 'light_intensity', type: 'range', meta:'light_intensity'},
				{ name: 'light_color_intensity', type: 'range', meta:'light_color_intensity'},
				{ name: 'light_color_r', type: 'range', meta:'light_color_r'},		
				{ name: 'light_color_g', type: 'range', meta:'light_color_g'},
				{ name: 'light_color_b', type: 'range', meta:'light_color_b'}
			]
		},
        {
            name: 'fx',
            values: [
                { name: 'fx_range', type: 'range', meta:'fx_range'},
                { name: 'fx_hue', type: 'range', meta:'fx_hue'},
                { name: 'fx_light_intensity', type: 'range', meta:'fx_light_intensity'},
                { name: 'fx_saturation', type: 'range', meta:'fx_saturation'},
                { name: 'fx_desaturation', type: 'range', meta:'fx_desaturation'},
                { name: 'fx_gamma_inc', type: 'range', meta:'fx_gamma_inc'},
                { name: 'fx_gamma_dec', type: 'range', meta:'fx_gamma_dec'},   
                { name: 'fx_color_filter_r', type: 'range', meta:'fx_color_filter_r'},
                { name: 'fx_color_filter_g', type: 'range', meta:'fx_color_filter_g'},
                { name: 'fx_color_filter_b', type: 'range', meta:'fx_color_filter_b'},
                { name: 'fx_color_floor_r', type: 'range', meta:'fx_color_floor_r'},
                { name: 'fx_color_floor_g', type: 'range', meta:'fx_color_floor_g'},
                { name: 'fx_color_floor_b', type: 'range', meta:'fx_color_floor_b'},
                { name: 'fx_tracing', type: 'range', meta:'fx_tracing'}
            ]
        }
	]
};

$(function() {

var objectPropertiesWidget;
var budgetPropertiesWidget;
var materialPickerShowTime;
var quickClosed = false;
var lastObjectIndex = -1;
var materialXmlDoc = null;
var itemsXmlDoc = null;
var recentSet = {};
var objectData = {};

$(document).mouseup(function(e) {
    var container = $(".container");
    if (!container.is(e.target) && container.has(e.target).length === 0) {
    	quickClose();
    }
});

var materialListElement = $('#material-tree');
var materialTreeListWidget = makeTreeList(materialListElement[0]);
	$(materialListElement).on('click', function() {
	handleUiInput(0);
})

materialTreeListWidget.on('select', function(node) {

	 if(node.getElementsByTagName('category').length)
	 	return;
	 


	renderMaterialList($('#material-list')[0], node);
	
	screenManager.push('material_picker_material_list', {
		onSelect: function(item, finished) {
			if(!item || !item.value)
				return;

			objectPropertiesWidget.setValue('appearance_material', item.value);
			if(finished) {
				for(var i = 0; i < 8; i++) {
					var currentScreen = screenManager.currentScreen();
					if(currentScreen && currentScreen.name === 'material_picker_material_list')
						screenManager.pop();
						break;		
				}
			}

			onPropertyChanged('appearance_material', item.value);
		},
		onBack: function(finished) {
			screenManager.pop();
		}
	});
});

	var screenManager = makeScreenManager({
		object_properties_list: {	
			title: 'object_properties',
			activate: () => {
				$('#object-properties-list').show();
			},
			deactivate: () => {
				$('#object-properties-list').hide();
			},
			handleUiButton: (uiButtonCode) => {
				switch(uiButtonCode) {
					case 1:
						screenManager.pop();
						dew.command('Game.PlaySound 0xb01');
					break;
					default:
						objectPropertiesWidget.handleUiInput(uiButtonCode);
					break;
				}
			}
		},
		material_picker: {
			title: 'appearance_material',
			activate: () => {
				$('#material-tree').show();
			},
			deactivate: () => {
				$('#material-tree').hide();
			},
			handleUiButton: (uiButtonCode) => {
				if(uiButtonCode == 1) {
					screenManager.pop();
					dew.command('Game.PlaySound 0xb01');
					return;
				}
				switch(uiButtonCode) {
					case 0:
						dew.command('Game.PlaySound 0xb00');
						break;
					case 8:
					case 9:
						dew.command('Game.PlaySound 0xafe');
					break;

				}

				materialTreeListWidget.handleInput(uiButtonCode);
			}
		},
		material_picker_material_list: makeMaterialListScreens(),
		budget: {
			title: 'summary',
			activate: () => {
				$('#budget-screen').show();
			},
			deactivate: () => {
				$('#budget-screen').hide();
			},
			handleUiButton: (uiButtonCode) => {
				switch(uiButtonCode) {
					case 1:
						screenManager.pop();
						dew.command('Game.PlaySound 0xb01');
					break;
					default:
						budgetPropertiesWidget.handleUiInput(uiButtonCode);

				}
			}
		}
	});



screenManager.on('screenChange', () => {
	updateTitle();
});


function getMaterialsXml(callback) {
	if(!materialXmlDoc) {
		$.get('materials.xml', function(data) {
			var parser = new DOMParser();
	    	doc = parser.parseFromString(data, "application/xml");
			materialXmlDoc = doc;
			callback(materialXmlDoc);
		});
	}
	else {
		return callback(materialXmlDoc);
	}
}

    
function getItemsXml(callback) {
    if(!itemsXmlDoc) {
        $.get('../object_creation/items.xml', function(data) {
            var parser = new DOMParser();
            doc = parser.parseFromString(data, "application/xml");
            itemsXmlDoc = doc;
            callback(itemsXmlDoc);
        });
    }
    else {
        return callback(itemsXmlDoc);
    }
}

function findMaterialItem(materialIndex) {
	for(var x = materialXmlDoc.firstChild; x; x = x.nextSibling) {
		var id = x.getAttribute('id');
		if(id == 'recent')
			continue;

		var items = x.getElementsByTagName('item');
		for(var i = 0; i < items.length; i++) {
			if(parseInt(items[i].getAttribute('value')) == materialIndex)
				return items[i];
		}
	}
}

dew.on('hide', function() {
	if(objectData.has_material) {
		var selectedMaterialIndex = objectPropertiesWidget.getValue('appearance_material');
		if(selectedMaterialIndex == -1)
			return;

		var recentNode = materialXmlDoc.getElementById('recent');

		var item = findMaterialItem(selectedMaterialIndex);
		if(!recentSet[selectedMaterialIndex]) {
			recentSet[selectedMaterialIndex] = true;
			item = item.cloneNode(true);
		} else {
			recentNode.removeChild(item);
		}

		recentNode.prepend(item);
	}
});

dew.on('show',function(e) {
	objectData = { has_material: e.data.has_material };

	if(quickClosed) {
		quickClosed = false;
		if(lastObjectIndex == e.data.object_index)
			return;
		else {
			screenManager.closeAll();
		}
	}

	lastObjectIndex = e.data.object_index;

	getItemsXml(function(itemsXmlDoc) {
	    getMaterialsXml(function(materialsDoc) {
	        var allMaterials  = Array.prototype.slice.call(materialsDoc.getElementsByTagName('item'), 0);

	        objectPropertiesWidget.populate({
	            strings: STRINGS,
	            properties: objectPropertyGridData.properties,
	            meta: Object.assign(objectPropertyGridData.meta, {
	                appearance_material: allMaterials.map(m => ({
	                    name: m.getAttribute('name'),
	                    value: parseInt(m.getAttribute('value'))
	                }))
	            })
	        }, e.data.properties);

	        buildPropertyFilter(e.data);

	        determineMaterialColorVisibility(e.data.properties.appearance_material);

	        var items = itemsXmlDoc.getElementsByTagName('item');
	        for(var i = 0; i < items.length; i++) {   
	            if(parseInt(items[i].getAttribute('tagindex')) === e.data.tag_index) {
	                for(var x = items[i].firstChild; x; x = x.nextSibling) {
	                    if(x.nodeName !== 'setter')
	                        continue;

	                    var target = x.getAttribute('target');
	                    var isHidden = !!x.getAttribute('hidden');
	                    if(isHidden)
	                        objectPropertiesWidget.toggleVisibility(target, false);
	                }
	                break;         
	            }
	        }

	        objectPropertiesWidget.setSelectedIndex(0);

	        materialTreeListWidget.setSource(materialsDoc.firstChild);

	        budgetPropertiesWidget.populate({
	            strings: STRINGS,
	            properties: [
                    { name: 'summary_placed_on_map', type: 'static', meta:'summary_placed_on_map'},
                    { name: 'summary_runtime_minimum', type: 'range', meta:'summary_runtime_minimum' },
                    { name: 'summary_runtime_maximum', type: 'range', meta:'summary_runtime_maximum' },
                    { name: 'summary_maximum_allowed', type: 'static', meta: 'summary_maximum_allowed'},
                    { name: 'summary_total_cost', type: 'static', meta:'summary_total_cost'}
	            ],
	            meta: {
	                summary_placed_on_map: e.data.budget.summary_placed_on_map,
	                summary_runtime_minimum: { type: 'int', min: 0, max: e.data.budget.summary_placed_on_map, step: 1},
	                summary_runtime_maximum: { type: 'int', min: 0, max: e.data.budget.summary_maximum_allowed, step: 1},
	                summary_maximum_allowed: e.data.budget.summary_maximum_allowed,
	                summary_total_cost: e.data.budget.summary_total_cost
	            }
	        }, e.data.budget);
	        budgetPropertiesWidget.setSelectedIndex(0);

		
		
	        screenManager.clear();
	        screenManager.push('object_properties_list');
	    });
	});
   
});

$(function() {
	$(window).on('keydown', function(e) {
		e.preventDefault();
		e.stopPropagation();

		switch(e.keyCode) {
			case 27:
				handleUiInput(1);
				break;
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

	budgetPropertiesWidget = makePropertyGridWidget($('#budget-screen'));

	objectPropertiesWidget = makePropertyGridWidget($('#object-properties-list'));
	objectPropertiesWidget.on('select', function(name) {
		switch(name) {
			case 'appearance_material':
				var value = objectPropertiesWidget.getValue(name);
				showMaterialPicker(value);
			break;
			case 'budget_screen_action':
				screenManager.push('budget');
			break;
			case 'select_all':
				dew.command('Forge.SelectAll');
				objectPropertiesWidget.setSelectedIndex(0);
				objectPropertiesWidget.toggleVisibility('select_all', false);
				objectPropertiesWidget.toggleVisibility('deselect_all', true);
				objectPropertiesWidget.setSelectedIndex(1);
			break;
			case 'deselect_all':
				dew.command('Forge.DeselectAllOf');
				objectPropertiesWidget.setSelectedIndex(0);
				objectPropertiesWidget.toggleVisibility('select_all', true);
				objectPropertiesWidget.toggleVisibility('deselect_all', false);
				objectPropertiesWidget.setSelectedIndex(1);
			break;
			case 'delete_all':
				dew.command('Forge.DeleteAll');
				screenManager.pop();
			break;
		}
	});

	objectPropertiesWidget.on('propertyChange', ({name, value}) => {
		if(value !== undefined)
			onPropertyChanged(name, value);
	});

	budgetPropertiesWidget.on('propertyChange', ({name, value}) => {
		onPropertyChanged(name, value);
	});

	$('#material-picker').on('mouseover', 'li', function(e) {
		if((Date.now() - materialPickerShowTime) < 150)
			return;
		$('#material-picker li').removeClass('selected');	
		$(this).addClass('selected');
	});

});

function onPropertyChanged(name, value) {
	var changes = {};
	switch(name) {
	    case 'appearance_material':
	        determineMaterialColorVisibility(value);
			//$('#appearance_material_material_preview').attr('src', objectPropertyGridData.meta['appearance_material'][value].thumbnail);
		break;
		case 'shape_type':
			switch (value)
			{
			  case 1:
			    objectPropertiesWidget.toggleVisibility('shape_top', false);
			    objectPropertiesWidget.toggleVisibility('shape_bottom', false);
			    objectPropertiesWidget.toggleVisibility('shape_depth', false);
			    objectPropertiesWidget.toggleVisibility('shape_width', false);
			    objectPropertiesWidget.toggleVisibility('shape_radius', true);
			    break;
			  case 2:
			    objectPropertiesWidget.toggleVisibility('shape_top', true);
			    objectPropertiesWidget.toggleVisibility('shape_bottom', true);
			    objectPropertiesWidget.toggleVisibility('shape_depth', false);
			    objectPropertiesWidget.toggleVisibility('shape_width', false);
			    objectPropertiesWidget.toggleVisibility('shape_radius', true);
			    break;
			  case 3:
			    objectPropertiesWidget.toggleVisibility('shape_top', true);
			    objectPropertiesWidget.toggleVisibility('shape_bottom', true);
			    objectPropertiesWidget.toggleVisibility('shape_depth', true);
			    objectPropertiesWidget.toggleVisibility('shape_width', true);
			    objectPropertiesWidget.toggleVisibility('shape_radius', false);
			    break;
			}
		break;
	}

	dew.callMethod('forgeaction',  {
		type: 1,
		data: { [name]: value }
	});
}


function determineMaterialColorVisibility(materialIndex) {

	var visible = objectData.has_material && materialIndex == 119 || materialIndex == 120;
   
    objectPropertiesWidget.toggleVisibility('appearance_material_color_r', visible);
    objectPropertiesWidget.toggleVisibility('appearance_material_color_g', visible);
    objectPropertiesWidget.toggleVisibility('appearance_material_color_b', visible);
    
}

function buildVisiblePropertyArray(properties, filterSet) {
	var result = [];
	for(var i = 0; i < properties.length; i++){
		var property = properties[i];
		if(filterSet && !filterSet[property])
			continue;
		result.push(property);
	}
	return result;
}

function buildPropertyFilter(data) {
	var isTeleporter = false;
	var hasZone = false;
	var hasSpawnOrder = false;
	var hasTeam = false;
	var hasSpawn = false;
	var hasSymmetry = false;

	switch(data.object_type_mp) {
	case MultiplayerObjectType.Weapon:	
	case MultiplayerObjectType.Ordinary:
	case MultiplayerObjectType.Grenade:
	case MultiplayerObjectType.Projectile:
	case MultiplayerObjectType.Powerup:
	case MultiplayerObjectType.Equipment:
	case MultiplayerObjectType.LightLandVehicle:
	case MultiplayerObjectType.HeavyLandVehicle:
	case MultiplayerObjectType.FlyingVehicle:
		hasSpawn = true;
		hasSymmetry = true;
		break;
	case MultiplayerObjectType.PlayerRespawnZone:
		hasSpawnOrder = true;
		hasZone = true;
		hasTeam = true;
		hasSymmetry = true;
		break;	
	case MultiplayerObjectType.Teleporter2Way:
	case MultiplayerObjectType.TeleporterSender:
	case MultiplayerObjectType.TeleporterReceiver:
		hasZone = true;
		isTeleporter = true;
		hasSymmetry = true;
		break;
	case MultiplayerObjectType.VIPDestinationObjective:
		hasSymmetry = true;
	case MultiplayerObjectType.HoldDestinationObjective:
	case MultiplayerObjectType.InfectionHavenObjective:		
		hasSpawn = true;
		hasSpawnOrder = true;
		break;
	case MultiplayerObjectType.HillObjective:
	case MultiplayerObjectType.JuggernautDestinationObjective:
		hasZone = true;
		hasSpawnOrder = true;
		break;
	case MultiplayerObjectType.CaptureDestinationObjective:
	case MultiplayerObjectType.CaptureSpawnObjective:
	case MultiplayerObjectType.TerritoryObjective:
	case MultiplayerObjectType.VIPBoundaryObjective:	
	case MultiplayerObjectType.PlayerSpawnLocation:
		hasTeam = true;
		hasSymmetry = true;
		break;
	}

	objectPropertiesWidget.toggleVisibility('deselect_all', data.is_selected);
	objectPropertiesWidget.toggleVisibility('select_all', !data.is_selected);

	objectPropertiesWidget.toggleVisibility('light', data.is_light);
	objectPropertiesWidget.toggleVisibility('light_radius', data.is_light);
	objectPropertiesWidget.toggleVisibility('light_color_r', data.is_light);
	objectPropertiesWidget.toggleVisibility('light_color_g', data.is_light);
	objectPropertiesWidget.toggleVisibility('light_color_b', data.is_light);
	objectPropertiesWidget.toggleVisibility('light_color_intensity', data.is_light);
	objectPropertiesWidget.toggleVisibility('light_intensity', data.is_light);


	objectPropertiesWidget.toggleVisibility('fx', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_hue', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_saturation', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_desaturation', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_light_intensity', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_color_filter_r', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_color_filter_g', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_color_filter_b', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_color_floor_r', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_color_floor_g', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_color_floor_b', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_gamma_inc', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_gamma_dec', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_range', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_tracing', data.is_screenfx);

	if(!hasSpawn) {
		objectPropertiesWidget.toggleVisibility('on_map_at_start', false);
		objectPropertiesWidget.toggleVisibility('respawn_rate', false);
	}
	if(!hasSymmetry)
		objectPropertiesWidget.toggleVisibility('symmetry', false);
	if(!hasTeam)
		objectPropertiesWidget.toggleVisibility('team_affiliation', false);
	if(!hasSpawnOrder)
		objectPropertiesWidget.toggleVisibility('spawn_order', false);
	if(!isTeleporter)
		objectPropertiesWidget.toggleVisibility('teleporter_channel', false);
	if(!data.has_spare_clips)
		objectPropertiesWidget.toggleVisibility('spare_clips', false);

	if(hasZone) {
		objectPropertiesWidget.toggleVisibility('physics', false);
		switch (data.properties.shape_type)
		{
		  case 1:
		    objectPropertiesWidget.toggleVisibility('shape_top', false);
		    objectPropertiesWidget.toggleVisibility('shape_bottom', false);
		    objectPropertiesWidget.toggleVisibility('shape_depth', false);
		    objectPropertiesWidget.toggleVisibility('shape_width', false);
		    break;
		  case 2:
		    objectPropertiesWidget.toggleVisibility('shape_depth', false);
		    objectPropertiesWidget.toggleVisibility('shape_width', false);
		    break;
		  case 3:
		    objectPropertiesWidget.toggleVisibility('shape_radius', false);
		    break;
		}
	}
	else {
		objectPropertiesWidget.toggleVisibility('teleporter_properties', false);
		objectPropertiesWidget.toggleVisibility('shape_depth', false);
		objectPropertiesWidget.toggleVisibility('shape_width', false);
		objectPropertiesWidget.toggleVisibility('shape_top', false);
		objectPropertiesWidget.toggleVisibility('shape_bottom', false);
		objectPropertiesWidget.toggleVisibility('shape_radius', false);
		objectPropertiesWidget.toggleVisibility('shape_bottom', false);
		objectPropertiesWidget.toggleVisibility('shape_type', false);
	}

	if(!data.has_material) {	
		objectPropertiesWidget.toggleVisibility('appearance_material', false);
	} else {
		objectPropertiesWidget.toggleVisibility('physics', false);
	}
}

function handleUiInput(uiButtonCode) {
	var current = screenManager.currentScreen();
	if(!current)
		return;
	switch(uiButtonCode) {
		case 2:
			quickClose();
		break;
	}
	current.handleUiButton(uiButtonCode);
}

function updateTitle() {
	var screens = screenManager.pushedScreens();
	var title = screens.slice(0, 2).map(screen => STRINGS[screen.title]).join(' - ');
	$('#title').text(title);
}


function showMaterialPicker(materialindex) {
	materialPickerShowTime = Date.now();
	var materialList = $('.material-tree')[0];
	materialTreeListWidget.gotoRoot();
	screenManager.push('material_picker');
}

function renderMaterialList(element, node) {
	var html = '';
	for(var x = node.firstChild; x; x = x.nextSibling) {
		if(x.nodeName !== 'item') continue;
			var thumbnail = x.getAttribute('thumbnail');
			html += `<li data-value="${x.getAttribute('value')}"><img src="${thumbnail}"></img> <span>${x.getAttribute('name')}</span></li>`;
	}
	element.innerHTML = html;
}

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

function quickClose() {
	quickClosed = true;
	 dew.command('Game.PlaySound 0xb01');
     dew.hide();
}

});


function makeMaterialListScreens() {
	var _materialListElement = $('#material-list');
	var _onSelectCallback = null;
	var _onBack = null;

	var materialListController = makeListWidgetController('material_iist', { wrapAround: true }, {
		count: () => _materialListElement.find('li').length
	});

	materialListController.on('selectedIndexChanged', function({index, userInput}) {
		var selectedElement = _materialListElement.find('li').removeClass('selected').eq(index);
		selectedElement.addClass('selected');
		if(userInput)
			selectedElement[0].scrollIntoView(false);
		onMaterialSelect(false);
	});

	_materialListElement.on('mouseover', 'li', function(e) {
		 if(e.target.nodeName == 'LI') {
            materialListController.setSelectedIndex($(e.target).index(), true);
        }    
	});

	_materialListElement.on('click', 'li', function(e) {
		 handleUiInput(0); 
	});

	return {
		name: 'material_picker_material_list',
		activate: (props) => {
			_onSelectCallback = props.onSelect;
			_onBack = props.onBack;

			materialListController.setSelectedIndex(props.selectedIndex || 0);
			_materialListElement.show();		
		},
		deactivate: () => {
			_materialListElement.hide();
		},
		handleUiButton: handleUiInput
	}

	function onMaterialSelect(finished) {
		var selectedElement = _materialListElement.find('li').eq(materialListController.selectedIndex());
		var value = parseInt(selectedElement.data('value'));
		if(_onSelectCallback)
			_onSelectCallback({ name: selectedElement.text(), value: value }, finished);

	}

	function handleUiInput(uiButtonCode) {
		switch(uiButtonCode) {
			case 0:
				onMaterialSelect(true);
				dew.command('Game.PlaySound 0xb00');
				_onBack(true);
			break;
			case 1:
				_onBack();
				dew.command('Game.PlaySound 0xb01');
				break;
			case 8:
			case 9:
				materialListController.navigate(uiButtonCode == 8 ? -1 : 1);
				dew.command('Game.PlaySound 0xafe');
			break;
		}
	}
}