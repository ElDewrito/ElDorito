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
		spare_clips: [0, 1, 2],
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
			{name: 'Ignore', value: 0},
			{name: 'Symetric', value: 1},
			{name: 'Asymmetric', value: 2}
		],
		appearance_material: [
			{ name: 'Material #001', value: 000 },
			{ name: 'Material #002', value: 001 },
			{ name: 'Material #003', value: 002 },
			{ name: 'Material #004', value: 003 },
			{ name: 'Material #005', value: 004 },
			{ name: 'Material #006', value: 005 },
			{ name: 'Material #007', value: 006 },
			{ name: 'Material #008', value: 007 },
			{ name: 'Material #009', value: 008 },
			{ name: 'Material #010', value: 009 },
			{ name: 'Material #011', value: 010 },
			{ name: 'Material #012', value: 011 },
			{ name: 'Material #013', value: 012 },
			{ name: 'Material #014', value: 013 },
			{ name: 'Material #015', value: 014 },
			{ name: 'Material #016', value: 015 },
			{ name: 'Material #017', value: 016 },
			{ name: 'Material #018', value: 017 },
			{ name: 'Material #019', value: 018 },
			{ name: 'Material #020', value: 019 },
			{ name: 'Material #021', value: 020 },
			{ name: 'Material #022', value: 021 },
			{ name: 'Material #023', value: 022 },
			{ name: 'Material #024', value: 023 },
			{ name: 'Material #025', value: 024 },
			{ name: 'Material #026', value: 025 },
			{ name: 'Material #027', value: 026 },
			{ name: 'Material #028', value: 027 },
			{ name: 'Material #029', value: 028 },
			{ name: 'Material #030', value: 029 },
			{ name: 'Material #031', value: 030 },
			{ name: 'Material #032', value: 031 },
			{ name: 'Material #033', value: 032 },
			{ name: 'Material #034', value: 033 },
			{ name: 'Material #035', value: 034 },
			{ name: 'Material #036', value: 035 },
			{ name: 'Material #037', value: 036 },
			{ name: 'Material #038', value: 037 },
			{ name: 'Material #039', value: 038 },
			{ name: 'Material #040', value: 039 },
			{ name: 'Material #041', value: 040 },
			{ name: 'Material #042', value: 041 },
			{ name: 'Material #043', value: 042 },
			{ name: 'Material #044', value: 043 },
			{ name: 'Material #045', value: 044 },
			{ name: 'Material #046', value: 045 },
			{ name: 'Material #047', value: 046 },
			{ name: 'Material #048', value: 047 },
			{ name: 'Material #049', value: 048 },
			{ name: 'Material #050', value: 049 },
			{ name: 'Material #051', value: 050 },
			{ name: 'Material #052', value: 051 },
			{ name: 'Material #053', value: 052 },
			{ name: 'Material #054', value: 053 },
			{ name: 'Material #055', value: 054 },
			{ name: 'Material #056', value: 055 },
			{ name: 'Material #057', value: 056 },
			{ name: 'Material #058', value: 057 },
			{ name: 'Material #059', value: 058 },
			{ name: 'Material #060', value: 059 },
			{ name: 'Material #061', value: 060 },
			{ name: 'Material #062', value: 061 },
			{ name: 'Material #063', value: 062 },
			{ name: 'Material #064', value: 063 },
			{ name: 'Material #065', value: 064 },
			{ name: 'Material #066', value: 065 },
			{ name: 'Material #067', value: 066 },
			{ name: 'Material #068', value: 067 },
			{ name: 'Material #069', value: 068 },
			{ name: 'Material #070', value: 069 },
			{ name: 'Material #071', value: 070 },
			{ name: 'Material #072', value: 071 },
			{ name: 'Material #073', value: 072 },
			{ name: 'Material #074', value: 073 },
			{ name: 'Material #075', value: 074 },
			{ name: 'Material #076', value: 075 },
			{ name: 'Material #077', value: 076 },
			{ name: 'Material #078', value: 077 },
			{ name: 'Material #079', value: 078 },
			{ name: 'Material #080', value: 079 },
			{ name: 'Material #081', value: 080 },
			{ name: 'Material #082', value: 081 },
			{ name: 'Material #083', value: 082 },
			{ name: 'Material #084', value: 083 },
			{ name: 'Material #085', value: 084 },
			{ name: 'Material #086', value: 085 },
			{ name: 'Material #087', value: 086 },
			{ name: 'Material #088', value: 087 },
			{ name: 'Material #089', value: 088 },
			{ name: 'Material #090', value: 089 },
			{ name: 'Material #091', value: 090 },
			{ name: 'Material #092', value: 091 },
			{ name: 'Material #093', value: 092 },
			{ name: 'Material #094', value: 093 },
			{ name: 'Material #095', value: 094 },
			{ name: 'Material #096', value: 095 },
			{ name: 'Material #097', value: 096 },
			{ name: 'Material #098', value: 097 },
			{ name: 'Material #099', value: 098 },
			{ name: 'Material #100', value: 099 },
			{ name: 'Material #101', value: 100 },
			{ name: 'Material #102', value: 101 },
			{ name: 'Material #103', value: 102 },
			{ name: 'Material #104', value: 103 },
			{ name: 'Material #105', value: 104 },
			{ name: 'Material #106', value: 105 },
			{ name: 'Material #107', value: 106 },
			{ name: 'Material #108', value: 107 },
			{ name: 'Material #109', value: 108 },
			{ name: 'Material #110', value: 109 },
			{ name: 'Material #111', value: 110 },
			{ name: 'Material #112', value: 111 },
			{ name: 'Material #113', value: 112 },
			{ name: 'Material #114', value: 113 },
			{ name: 'Material #115', value: 114 },
			{ name: 'Material #116', value: 115 },
			{ name: 'Material #117', value: 116 },
			{ name: 'Material #118', value: 117 },
			{ name: 'Material #119', value: 118 },
			{ name: 'Material #120', value: 119 },
			{ name: 'Material #121', value: 120 },
			{ name: 'Material #122', value: 121 },
			{ name: 'Material #123', value: 122 },
			{ name: 'Material #124', value: 123 },
			{ name: 'Material #125', value: 124 },
			{ name: 'Material #126', value: 125 },
			{ name: 'Material #127', value: 126 },
			{ name: 'Material #128', value: 127 }
		],
		lightmap: [
			{ name: 'On', value: 1 },
			{ name: 'Off', value: 0 }
			
		],
		physics: [{name:'Default', value: 0}, {name:'Phased', value: 1}],
		light_color_r: { min: 0.0, max: 1.0, step: 0.01 },
		light_color_g: { min: 0.0, max: 1.0, step: 0.01 },
		light_color_b: { min: 0.0, max: 1.0, step: 0.01 },
		light_intensity: { min: 0.0, max: 1.0, step: 0.01 },
		light_color_intensity: { min: 0.0, max: 1.0, step: 0.01 },
		light_radius: { min: 0, max: 50, step: 0.01 },
		fx_hue: { min: 0, max: 1, step: 0.01 },
		fx_saturation: { min: 0, max: 1, step: 0.01 },
		fx_tint_r: { min: 0, max: 1, step: 0.01 },
		fx_tint_g: { min: 0, max: 1, step: 0.01 },
		fx_tint_b: { min: 0, max: 1, step: 0.01 },
		fx_light_intensity: { min: 0, max: 1, step: 0.01 },
		fx_darkness: { min: 0, max: 1, step: 0.01 },
		fx_brightness: { min: 0, max: 1, step: 0.01 },
		fx_color_mute: { min: 0, max: 1, step: 0.01 },
	    fx_range: { min: 1, max: 255, step: 1 }
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
				{ name: 'fx_hue', type: 'range', meta:'fx_hue'},
				{ name: 'fx_saturation', type: 'range', meta:'fx_saturation'},
				{ name: 'fx_tint_r', type: 'range', meta:'fx_tint_r'},
				{ name: 'fx_tint_g', type: 'range', meta:'fx_tint_g'},		
				{ name: 'fx_tint_b', type: 'range', meta:'fx_tint_b'},
                { name: 'fx_color_mute', type: 'range', meta:'fx_color_mute'},
                { name: 'fx_light_intensity', type: 'range', meta:'fx_light_intensity'},
                { name: 'fx_darkness', type: 'range', meta:'fx_darkness'},
                { name: 'fx_brightness', type: 'range', meta:'fx_brightness'},
                { name: 'fx_range', type: 'range', meta:'fx_range'},
            ]
        }
	]
};


var objectPropertiesWidget;
var budgetPropertiesWidget;
var materialPickerShowTime;

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
			$('#material-picker').show();
			$('#material-picker li')
			.removeClass('selected')
			.eq(materialListController.selectedIndex())
			.addClass('selected')[0]
			.scrollIntoView(false);
		},
		deactivate: () => {
			$('#material-picker').hide();
		},
		handleUiButton: (uiButtonCode) => {
			switch(uiButtonCode) {
				case 1:
					screenManager.pop();
					dew.command('Game.PlaySound 0xb01');
					break;
				case 0:
					var index = materialListController.selectedIndex();
					onPropertyChanged('appearance_material', index);
					screenManager.pop();
					dew.command('Game.PlaySound 0xb00');
				break;
				case 8:
				case 9:
					materialListController.navigate(uiButtonCode == 8 ? -1 : 1);
					dew.command('Game.PlaySound 0xafe');
				default:
				break;
			}
		}
	},
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

var materialListController = makeListWidgetController('material_iist', { wrapAround: true }, {
	count: () => objectPropertyGridData.meta['appearance_material'].length
});

materialListController.on('selectedIndexChanged', ({index, userInput}) => {
	if($('#material-picker li').is(':visible')) {
		$('#material-picker li').removeClass('selected').eq(index).addClass('selected')[0].scrollIntoView(false);
	}

	if(userInput) {
		onPropertyChanged('appearance_material', index);
	}
});

dew.on('show',function(e) {
	
	for(var key in e.data.properties)
	{
		switch(key) {
			case 'appearance_material':
				materialListController.setSelectedIndex(e.data.properties[key]);
			break;
		}
	}

	objectPropertiesWidget.populate(objectPropertyGridData, e.data.properties);
	buildPropertyFilter(e.data);

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

	objectPropertiesWidget.setSelectedIndex(0);
	budgetPropertiesWidget.setSelectedIndex(0);
	screenManager.clear();
	screenManager.push('object_properties_list');
	//dew.command('Game.PlaySound 0xb02');
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
				showMaterialPicker((selectedIndex) => { onPropertyChanged(name, selectedIndex); });
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
		switch(name) {
			case 'appearance_material':
				materialListController.setSelectedIndex(value);
			break;
		}
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

	$('#material-picker').on('click', 'li', function(e) {
		materialListController.setSelectedIndex($(this).index(), false);
		handleUiInput(0);
	});

});

function onPropertyChanged(name, value) {
	var changes = {};
	switch(name) {
		case 'appearance_material':
			$('#appearance_material_material_preview').attr('src', objectPropertyGridData.meta['appearance_material'][value].thumbnail);
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
	var filter = {};

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
	objectPropertiesWidget.toggleVisibility('fx_light_intensity', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_tint_r', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_tint_g', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_tint_b', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_color_mute', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_brightness', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_darkness', data.is_screenfx);
	objectPropertiesWidget.toggleVisibility('fx_range', data.is_screenfx);

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

	return filter;
}

function handleUiInput(uiButtonCode) {
	var current = screenManager.currentScreen();
	if(!current)
		return;
	current.handleUiButton(uiButtonCode);
}

function updateTitle() {
	var screens = screenManager.pushedScreens();
	var title = screens.map(screen => STRINGS[screen.title]).join(' - ');
	$('#title').text(title);
}


function showMaterialPicker(callback) {
	var materials = objectPropertyGridData.meta['appearance_material'];
	materialPickerShowTime = Date.now();
	$('#material-picker').html(`
		<ul class="material-list">
			${materials.reduce((html, m) => (html + `
				<li><img src="${m.thumbnail}"></img> <span>${m.name}</span></li>
				`), '')}
		</ul>
	 `);

	screenManager.push('material_picker');
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
  
});