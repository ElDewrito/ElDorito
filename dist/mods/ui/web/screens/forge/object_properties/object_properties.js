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
	zone: '',
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
		teleporter_channel: [0,1,2,3,4,5,6,7,8,9],
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
			{ name: 'Material #1', value: 0 },
			{ name: 'Material #2', value: 1 },
			{ name: 'Material #3', value: 2 },
			{ name: 'Material #4', value: 3 },
			{ name: 'Material #5', value: 4 },
			{ name: 'Material #6', value: 5 },
			{ name: 'Material #7', value: 6 },
			{ name: 'Material #8', value: 7 },
			{ name: 'Material #9', value: 8 },
			{ name: 'Material #10', value: 9 },
			{ name: 'Material #11', value: 10 },
			{ name: 'Material #12', value: 11 },
			{ name: 'Material #13', value: 12 },
			{ name: 'Material #14', value: 13 },
			{ name: 'Material #15', value: 14 },
			{ name: 'Material #16', value: 15 },
			{ name: 'Material #17', value: 16 },
			{ name: 'Material #18', value: 17 },
			{ name: 'Material #19', value: 18 },
			{ name: 'Material #20', value: 19 },
			{ name: 'Material #21', value: 20 },
			{ name: 'Material #22', value: 21 },
			{ name: 'Material #23', value: 22 },
			{ name: 'Material #24', value: 23 },
			{ name: 'Material #25', value: 24 },
			{ name: 'Material #26', value: 25 },
			{ name: 'Material #27', value: 26 },
			{ name: 'Material #28', value: 27 },
			{ name: 'Material #29', value: 28 },
			{ name: 'Material #30', value: 29 },
			{ name: 'Material #31', value: 30 },
			{ name: 'Material #32', value: 31 },
			{ name: 'Material #33', value: 32 },
			{ name: 'Material #34', value: 33 },
			{ name: 'Material #35', value: 34 },
			{ name: 'Material #36', value: 35 },
			{ name: 'Material #37', value: 36 },
			{ name: 'Material #38', value: 37 },
			{ name: 'Material #39', value: 38 },
			{ name: 'Material #40', value: 39 },
			{ name: 'Material #41', value: 40 },
			{ name: 'Material #42', value: 41 },
			{ name: 'Material #42', value: 42 },
			{ name: 'Material #43', value: 43 },
			{ name: 'Material #44', value: 44 },
			{ name: 'Material #45', value: 45 },
			{ name: 'Material #46', value: 46 },
			{ name: 'Material #47', value: 47 },
			{ name: 'Material #48', value: 48 },
			{ name: 'Material #49', value: 49 },
			{ name: 'Material #50', value: 50 },
			{ name: 'Material #51', value: 51 },
			{ name: 'Material #53', value: 52 },
			{ name: 'Material #54', value: 53 },
			{ name: 'Material #55', value: 54 },
			{ name: 'Material #56', value: 55 },
			{ name: 'Material #57', value: 56 },
			{ name: 'Material #58', value: 57 },
			{ name: 'Material #59', value: 58 },
			{ name: 'Material #60', value: 59 },
			{ name: 'Material #61', value: 60 },
			{ name: 'Material #62', value: 61 },
			{ name: 'Material #63', value: 62 },
			{ name: 'Material #64', value: 63 },
			{ name: 'Material #65', value: 64 },
			{ name: 'Material #66', value: 65 },
			{ name: 'Material #67', value: 66 },
			{ name: 'Material #68', value: 67 },
			{ name: 'Material #69', value: 68 },
			{ name: 'Material #70', value: 69 },
			{ name: 'Material #71', value: 70 }
		],
		lightmap: [
			{ name: 'On', value: 1 },
			{ name: 'Off', value: 0 }
			
		],
		physics: [{name:'Default', value: 0}, {name:'Phased', value: 1}]
	},
	properties: [
		{ name: 'budget_screen_action', type: 'action'},
		{ name: 'select_all', type: 'action'},
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
		}
	]
};


var objectPropertiesWidget;
var budgetPropertiesWidget;

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

	$('#material-picker').on('mouseenter mouseleave', 'li', function(e) {
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
    	stickTicks.up > 0 && (stickTicks.up * e.data.SecondsPerTick) > .5 ||
    	stickTicks.down > 0 && (stickTicks.down * e.data.SecondsPerTick) > .5 ||
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