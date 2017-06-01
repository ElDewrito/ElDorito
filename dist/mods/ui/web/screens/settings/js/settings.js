var loadedSettings = false;
var settingsToLoad = [['pName', 'Player.Name'], ['renderWeapon', 'Player.RenderWeapon'], ['armorHelmet', 'Player.Armor.Helmet'], ['armorChest', 'Player.Armor.Chest'], ['armorShoulders', 'Player.Armor.Shoulders'], ['armorArms', 'Player.Armor.Arms'], ['armorLegs', 'Player.Armor.Legs '], ['armorAcc', 'Player.Armor.Accessory'], ['colorsPrimary', 'Player.Colors.Primary'], ['colorsSecondary', 'Player.Colors.Secondary'], ['colorsVisor', 'Player.Colors.Visor'], ['colorsLights', 'Player.Colors.Lights'], ['colorsHolo', 'Player.Colors.Holo'],['sName', 'Server.Name'],['sCountdown', 'Server.Countdown'], ['sMaxPlayers', 'Server.MaxPlayers'], ['sMaxTeamSize', 'Server.MaxTeamSize'], ['sShouldAnnounce', 'Server.ShouldAnnounce'],['sSprintEnabled', 'Server.SprintEnabled'], ['sUnlimitedSprint', 'Server.UnlimitedSprint'], ['sDualWieldEnabled', 'Server.DualWieldEnabled'], ['sAssassinationEnabled', 'Server.AssassinationEnabled'], ['cCenteredCrosshair' , 'Camera.Crosshair'], ['cFOV', 'Camera.FOV'], ['cHideHUD', 'Camera.HideHUD'], ['cSpeed', 'Camera.Speed'],['inputRaw','Input.RawInput'], ['gfxSaturation', 'Graphics.Saturation'], ['gfxBloom', 'Graphics.Bloom'], ['sPass', 'Server.Password'], ['sMessage', 'Server.Message'], ['lookSensitivity', 'Input.ControllerSensitivityY'], ['controllerPort','Input.ControllerPort'], ['gAnnouncerVol','Game.AnnouncerVolume'], ['gExpandScoreboard','Game.ExpandedScoreboard'], ['invertLook','Input.ControllerInvertY'], ['gHideChat','Game.HideChat'],['gSuppressJuggling','Game.SuppressJuggling']];
var binds = ["Sprint", "Jump", "Crouch", "Use", "DualWield", "Fire", "FireLeft", "Reload", "ReloadLeft", "Zoom", "SwitchWeapons", "Melee", "Grenade", "SwitchGrenades", "VehicleAccelerate", "VehicleBrake", "VehicleBoost", "VehicleRaise", "VehicleDive", "VehicleFire", "VehicleAltFire", "BansheeBomb", "Menu", "Scoreboard", "ForgeDelete", "Chat", "TeamChat", "UseEquipment"];
var buttons = ["","A","B","X","Y","RB","LB","LT","RT","Start","Back","LS","RS","Left","Right","Up","Down"];
var renderWeapons = [
	["Assault Rifle","assault_rifle"],
	/*["Assault Rifle DMG","ar_variant_2"],
	["Assault Rifle ROF","ar_variant_3"],
	["Assault Rifle ACC","ar_variant_5"],
	["Assault Rifle PWR","ar_variant_6"],*/
	["Battle Rifle","battle_rifle"],
	/*["Battle Rifle ROF","br_variant_1"],
	["Battle Rifle ACC","br_variant_2"],
	["Battle Rifle MAG","br_variant_3"],
	["Battle Rifle DMG","br_variant_4"],
	["Battle Rifle RNG","br_variant_5"],
	["Battle Rifle PWR","br_variant_6"],
	["Covenant Carbine","covenant_carbine"],
	["Covenant Carbine MAG","covenant_carbine_variant_1"],
	["Covenant Carbine DMG","covenant_carbine_variant_2"],
	["Covenant Carbine ACC","covenant_carbine_variant_3"],
	["Covenant Carbine ROF","covenant_carbine_variant_4"],
	["Covenant Carbine RNG","covenant_carbine_variant_5"],
	["Covenant Carbine PWR","covenant_carbine_variant_6"],*/
	["DMR","dmr"],
	/*["DMR MAG","dmr_variant_1"],
	["DMR ACC","dmr_variant_2"],
	["DMR ROF","dmr_variant_3"],
	["DMR DMG","dmr_variant_4"],
	["DMR RNG","dmr_variant_5"],
	["DMR PWR","dmr_variant_6"],*/
	["Plasma Rifle","plasma_rifle"],
	/*["Plasma Rifle PWR","plasma_rifle_variant_6"],*/
	["SMG","smg"],
	/*["SMG ROF","smg_variant_1"],
	["SMG ACC","smg_variant_2"],
	["SMG DMG","smg_variant_4"],
	["SMG PWR","smg_variant_6"]*/
];
var armorList = [
	["Air Assault","air_assault"],
	["Ballista","ballista"],
	["Chameleon","chameleon"],
	["Cyclops","cyclops"],
	["Demo","demo"],
	["Dutch","dutch"],
	["Gladiator","gladiator"],
	["Gungnir","gungnir"],
	["Halberd","halberd"],
	["Hammerhead","hammerhead"],
	["Hoplite","hoplite"],
	["Juggernaut","juggernaut"],
	["Mac","mac"],
	["Mercenary","mercenary"],
	["Nihard","nihard"],
	["Omni","omni"],
	["Oracle","oracle"],
	["Orbital","orbital"],
	["Renegade","renegade"],
	["Scanner","scanner"],
	["Shark","shark"],
	["Silverback","silverback"],
	["Spectrum","spectrum"],
	["Stealth","stealth"],
	["Strider","strider"],
	["Widow Maker","widow_maker"]
];
var accessoryList = [
    ["None", ""],
    ["Ammo Belt", "ammo_belt"],
    ["Ammo Pack", "ammo_pack"],
    ["Antenna 1", "antenna_01"],
    ["Antenna 2", "antenna_02"],
    ["Bullest Shield", "bullet_shield"],
    ["Chest Battery", "chest_battery"],
    ["Dog Tag", "dog_tag_01"],
    ["Electronic Tool", "electronic_tool"],
    ["Flashlight", "flashlight"],
    ["Generator Device", "generator_device"],
    ["Grenade", "grenade_01"],
    ["Holo Scope", "holo_scope"],
    ["HUD Screen", "hud_screen"],
    ["Katana", "katana"],
    ["Knife 1", "knife_01"],
    ["Knife 2", "knife_02"],
    ["Medkit", "medkit"],
    ["Reactive Armor Arm", "reactive_armor_arm"],
    ["Reactive Armor Leg", "reactive_armor_leg"],
    ["Reactive Armor Plate", "reactive_armor_plate"],
    ["Shotgun Ammo", "shotgun_ammo"],
    ["Target Painter", "target_painter"],
    ["Throwing Knives", "throwing_knives"],
    ["Tool Bag", "tools_bag_1"]
];
var controllerPresets = [
    ["Halo Online Default","LS,A,X,RB,LB,RT,LT,RB,LB,RS,Y,B,LT,Right,,,LT,A,X,RT,LT,B,Start,Back,Y,,,LB"],
    ["Halo 3 Default","X,A,LS,RB,LB,RT,LT,RB,LB,RS,Y,B,LT,LB,,,LT,A,LS,RT,LT,B,Start,Back,Y,,,X"],
    ["Halo 3 Southpaw","X,A,LS,RB,LB,LT,RT,RB,LB,RS,Y,B,RT,LB,,,RT,A,LS,LT,RT,B,Start,Back,Y,,,X"],
    ["Halo 3 Boxer","X,A,LS,RB,LB,RT,LT,RB,LB,RS,Y,LT,B,LB,,,LT,A,LS,RT,LT,B,Start,Back,Y,,,X"],
    ["Halo 3 Green Thumb","X,A,LS,RB,LB,RT,LT,RB,LB,B,Y,RS,LT,LB,,,LT,A,LS,RT,LT,B,Start,Back,Y,,,X"],
    ["Halo 3 Bumper Jumper","X,LB,LS,B,A,RT,LT,B,A,RS,Y,RB,LT,A,,,LT,LB,LS,RT,LT,B,Start,Back,Y,,,X"],
    ["Halo 3 Walkie Talkie","Up,A,LS,B,X,RT,LT,B,X,RS,Y,RB,LT,A,,,LT,A,LS,RT,LT,B,Start,Back,Y,,LB,Up"],
    ["Halo Reach Default","LB,A,LS,X,LB,RT,LT,X,LB,RS,Y,RB,LT,B,,,LT,A,LS,RT,LT,B,Start,Back,Y,Down,Up,LB"],
    ["Halo Reach Southpaw","RB,A,LS,X,RB,LT,RT,X,RB,RS,Y,LB,RT,B,,,RT,A,LS,LT,RT,B,Start,Back,Y,Down,Up,RB"],
    ["Halo Reach Boxer","LB,A,LS,X,LB,RT,LT,X,LB,RS,Y,LT,RB,B,,,LT,A,LS,RT,LT,B,Start,Back,Y,Down,Up,LB"],
    ["Halo Reach Green Thumb","LB,A,LS,X,LB,RT,LT,X,LB,RB,Y,RS,LT,B,,,LT,A,LS,RT,LT,B,Start,Back,Y,Down,Up,LB"],
    ["Halo Reach Bumper Jumper","X,LB,LS,B,LB,RT,LT,B,LB,RS,Y,RB,LT,A,,,LT,LB,LS,RT,LT,RB,Start,Back,Y,Down,Up,X"],
    ["Halo Reach Recon","LB,A,LS,RB,LB,RT,LT,RB,LB,RS,Y,B,LT,X,,,LT,A,LS,RT,LT,B,Start,Back,Y,Down,Up,LB"],
    ["Halo 4 Default","LS,A,B,X,LB,RT,LT,X,LB,RS,Y,RB,LT,Right,,,LT,A,B,RT,LT,RB,Start,Back,Y,,,LB"],
    ["Halo 4 Southpaw","LS,A,B,X,RB,LT,RT,X,RB,RS,Y,LB,RT,Right,,,RT,A,B,LT,RT,LB,Start,Back,Y,,,LB"],
    ["Halo 4 Boxer","B,A,LS,X,LB,RT,LT,X,LB,RS,Y,LT,RB,Right,,,LT,A,LS,RT,LT,B,Start,Back,Y,,,LB"],
    ["Halo 4 Green Thumb","LS,A,B,X,LB,RT,LT,X,LB,RB,Y,RS,LT,Right,,,LT,A,B,RT,LT,RB,Start,Back,Y,,,LB"],
    ["Halo 4 Bumper Jumper","A,LB,LS,B,X,RT,LT,B,X,RS,Y,RB,LT,Right,,,LT,LB,LS,RT,LT,RB,Start,Back,Y,,,X"],
    ["Halo 4 Recon","X,A,LS,RB,LB,RT,LT,RB,LB,RS,Y,B,LT,Right,,,LT,A,LS,RT,LT,B,Start,Back,Y,,,LB"],
    ["Halo 4 Fishstick","LS,A,B,X,LB,RT,LT,X,LB,LT,Y,RS,RB,Right,,,LT,A,B,RT,LT,RS,Start,Back,Y,,,LB"],
    ["Halo 5 Default","LS,A,RS,X,LB,RT,LT,X,LB,LT,Y,RB,LB,Right,,,LT,A,LS,RT,LT,B,Start,Back,Y,,,B"],
    ["Halo 5 Boxer","B,A,LS,X,LB,RT,LT,X,LB,RS,Y,LT,RB,Right,,,LT,A,LS,RT,LT,B,Start,Back,Y,,,LB"]
];

$(document).ready(function() {
    $(document).keydown(function(e){
        if(e.keyCode == 9 || e.keyCode == 27){
            closeBrowser();
        } 
        if(e.keyCode == 192 || e.keyCode == 112){
            dew.show("console");
        }
    });
    if(window.location.href.indexOf("#") == -1) {
       window.location.replace("#playerSettings");
    }
    $('.color').colorPicker({
        opacity: false,    
        renderCallback: function($elm, toggled) {
            var colors = this.color.colors;
            updateSetting($elm[0].name, '#'+colors.HEX);
        }
    });
    $('#settingsWindow input:not(input[type=color]), #settingsWindow select').on('change', function(){
        updateSetting(this.name, this.value);
    });
    $('#settingsWindow input[type=range]').on('input', function(){
        updateSetting(this.name, this.value);
    });
    $('#settingsWindow textarea').on('change', function(){
        updateSetting(this.name, "\"" + this.value + "\"");
    });
    $('#controllerSettings .bind').on('change', function(){
        updateBinding(this.id, this.value);
        updateBindLabels();
    });    
    $('#controllerPort, #invertLook').on('change', function(){
        updateSetting(this.name, "\"" + this.value + "\"");         
    });
    setButtonLists();
	setOptionList('presetMenu', controllerPresets);
	setOptionList('renderWeapon', renderWeapons);
	setOptionList('helmet', armorList);
	setOptionList('chest', armorList);
	setOptionList('shoulders', armorList);
	setOptionList('arms', armorList);
	setOptionList('legs', armorList);
    setOptionList('accessory', accessoryList);
    $('.wheelable').on('mousewheel', function(e) {
        if(e.originalEvent.wheelDelta > 0) {
            var elementIndex = $('#'+this.id+' option:selected').index();
            if(elementIndex > 0){
                var newElement = elementIndex - 1;
                $('#'+this.id+' option').eq(newElement).prop('selected', true);
                updateSetting(this.name, this.value);
            }
        }else{
            var elementIndex = $('#'+this.id+' option:selected').index();
            var elementLength = $('#'+this.id).children('option').length;
            if(elementIndex < elementLength){
                var newElement = elementIndex + 1;
                $('#'+this.id+' option').eq(newElement).prop('selected', true);
                updateSetting(this.name, this.value);
            }
        }
    });
    loadSettings(0);
    initializeBindings();
});

function loadSettings(i) {
	if (i != settingsToLoad.length) {
		dew.command(settingsToLoad[i][1], {}).then(function(response) {
            if(settingsToLoad[i][1].startsWith("Player.Colors")){
                $("input[name='"+settingsToLoad[i][0]+"']").css("background-color",response);   
                $("input[name='"+settingsToLoad[i][0]+"']").val(response); 
                if(getLuminance(response)> 0.22){
                    $("input[name='"+settingsToLoad[i][0]+"']").css("color","#222");
                }else{
                    $("input[name='"+settingsToLoad[i][0]+"']").css("color","#ddd");
                }
            }else if(settingsToLoad[i][1].startsWith("Input.ControllerSensitivity")){ 
                    var yVal = response/10;
                    $("input[name='"+settingsToLoad[i][0]+"']").val(yVal);
            }else{
                if ($("input[name='"+settingsToLoad[i][0]+"']").is(':checkbox')){
                    if (response == "1"){
                        $("input[name='"+settingsToLoad[i][0]+"']").prop('checked', true);
                    }                
                }else{
                    $("input[name='"+settingsToLoad[i][0]+"']").val(response);
                }
                $("select[name='"+settingsToLoad[i][0]+"']").val(response);
                $("textarea[name='"+settingsToLoad[i][0]+"']").val(response);
            }
			i++;
			loadSettings(i);
		});
	} else {
		loadedSettings = true;
	}
}

function updateSetting(setting, value){
    if ($("input[name='"+setting+"']").is(':checkbox')){
        if ($("input[name='"+setting+"']").is(':checked')){
            value = "1";
        } else {
            value = "0";
        }
    }
    dew.command(settingsToLoad[arrayInArray(setting, settingsToLoad)][1] + " \"" + value + "\"", {}).then(function(){
        dew.command("writeconfig");
    });
}

function closeBrowser() {
    dew.hide();
}

function arrayInArray(needle, haystack) {
    for(i=0; i<haystack.length; i++) {
        if (haystack[i].indexOf(needle) > -1){
            return i
        }
    }
}

String.prototype.startsWith = function(needle){
    return(this.indexOf(needle) == 0);
};

function applyBindString(bindString){
    var bindArray = new Array(bindString.split(','));
    for (i = 0; i < bindArray[0].length; i++) { 
        $("select[id='"+binds[i]+"']").val(bindArray[0][i]);
        updateBinding(binds[i], bindArray[0][i]);
    }
    updateBindLabels();
}

function initializeBindings(){
    dew.command("Input.DumpBindingsJson", {}).then(function(response){
        var controllerBinds = JSON.parse(response);
        for (i = 0; i < controllerBinds.length; i++){
            if(controllerBinds[i].controllerButton=="Select"){
                controllerBinds[i].controllerButton="Back";
            }
            $('#'+controllerBinds[i].actionName).val(controllerBinds[i].controllerButton);
        }
        updateBindLabels();
        //getCurrentBindString();
    });
}

function updateBinding(action, bind){
    if (bind == "Back") { bind = "Select"; }
    if (bind) { bind = "\"" + bind + "\""; }
    dew.command("Input.ControllerAction \"" + action + "\" " + bind, {}).then(function(){
        dew.command("writeconfig");
    });
}

function updateBindLabels(){
    $('#controllerGraphic').children('div').empty();
    for (i = 0; i < binds.length; i++) { 
        var bind = document.getElementById(binds[i]).value;
        var action = binds[i];
        if(document.getElementById(bind)){
            var actionString = action;
            if(document.getElementById(bind).innerHTML.length > 0){
                actionString = ", " + action;
            }
            $("#" + bind).append(actionString);
        }
    }
}

var controllerShown = false;
function showController(){
    if (!controllerShown){
        $("#controllerSettings").css("display", "block");
        controllerShown = true;
    } else {
        $("#controllerSettings").css("display", "none");
        controllerShown = false;
    }
}

var extendedShown = false;
function showExtended(){
    if (!extendedShown){
        $("#extendedControls").css("display", "block");
        $("#expandCollapse").css("bottom", "-24.957vh");     
        extendedShown = true;
    } else {
        $("#extendedControls").css("display", "none");
        $("#expandCollapse").css("bottom", "-1.3275vh");
        extendedShown = false;
    }
}

function setButtonLists(){
    for(var i = 0; i < binds.length; i++) {
        var sel = document.getElementById(binds[i]);
        for(var x = 0; x < buttons.length; x++) {
            var opt = document.createElement('option');
            opt.innerHTML = buttons[x];
            opt.value = buttons[x];
            sel.appendChild(opt);
        }
    }
}

function setOptionList(ElementID, ArrayVar){
    var sel = document.getElementById(ElementID);
    for(var i = 0; i < ArrayVar.length; i++) {
        var opt = document.createElement('option');
        opt.innerHTML = ArrayVar[i][0];
        opt.value = ArrayVar[i][1];
        sel.appendChild(opt);
    }
}

function getLuminance(hex) {
    var converted = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    var div = 255,
        RGB = [parseInt(converted[1], 16)/div, parseInt(converted[2], 16)/div, parseInt(converted[3], 16)/div],
        luminance = {r: 0.2126, g: 0.7152, b: 0.0722};
    for (var i = RGB.length; i--; ) {
        RGB[i] = RGB[i] <= 0.03928 ? RGB[i] / 12.92 : Math.pow(((RGB[i] + 0.055) / 1.055), 2.4);
    }
    return ((luminance.r * RGB[0]) + (luminance.g * RGB[1]) + (luminance.b * RGB[2]));
}

function updateSensitivity(value){
    var xVal = value * 20;
    var yVal = value * 10;
    dew.command("Input.ControllerSensitivityY " + yVal, {}).then(function(){
        dew.command("writeconfig");
    });
    dew.command("Input.ControllerSensitivityX " + xVal, {}).then(function(){
        dew.command("writeconfig");
    });
}

function buttonAction(i){
    switch (i) {
        case 1: // B
        case 9: // Start
            closeBrowser();
            break;
        default:
            // console.log("nothing associated with " + i);
    }  
}

function getCurrentBindString(){
    var currentBinds = "";
    for(var i = 0; i < binds.length; i++) {
        if($('#'+binds[i]).val()){
            currentBinds += $('#'+binds[i]).val() + ",";
        }else{
            currentBinds += ",";
        }
    }
    console.log(currentBinds.slice(0, -1));
}