var activePage;
var selectedItem;
var itemNumber = 0;
var tabIndex = 0;
var colorIndex = 0;
var changeArray = [];
var commandValues = [];
var vetoEnabled;
var unlimitedSprint;
var hasGP = false;
var axisThreshold = .5;
var stickTicks = { left: 0, right: 0, up: 0, down: 0 };
var repGP;
var lastHeldUpdated = 0;
var playSound = true;

var settingsToLoad = [
    ['sControlsMethod','Settings.Gamepad', 'Control Method', 'Keyboard or Gamepad - Choose your preference.'],
	['sInfantryMouseSensV','Settings.MouseSensitivityVertical', 'Infantry Mouse Sensitivity Vertical', 'Adjusts the infantry mouse sensitivity on the Y-axis.'],
    ['sInfantryMouseSensH','Settings.MouseSensitivityHorizontal', 'Infantry Mouse Sensitivity Horizontal', 'Adjusts the infantry mouse sensitivity on the X-axis.'],
    ['sVehicleMouseSensV','Settings.MouseSensitivityVehicleVertical', 'Vehicle Mouse Sensitivity Vertical', 'Adjusts the vehicle mouse sensitivity on the Y-axis.'],
    ['sVehicleMouseSensH','Settings.MouseSensitivityVehicleHorizontal', 'Vehicle Mouse Sensitivity Horizontal', 'Adjusts the vehicle mouse sensitivity on the X-axis.'],
    ['sMouseAcceleration','Settings.MouseAcceleration', 'Mouse Acceleration', 'Increases the speed multiplier of your mouse the faster you move it.'],
    ['sMouseFilter','Settings.MouseFilter', 'Mouse Filter', 'Smoothes out mouse movement.'],
    ['sInvertMouse','Settings.InvertMouse', 'Invert Mouse', 'Inverts the mouse look vertically.'],
    ['sToggleCrouch','Settings.ToggleCrouch', 'Toggle Crouch', 'Toggles crouch on press instead of disabling on release.'],
    ['sScreenResolution','Settings.ScreenResolution', 'Screen Resolution', 'Sets the rendering resolution. Higher resolutions require more graphics processing power and video memory.'],
    ['sBrightness','Settings.Brightness', 'Brightness', 'Adjust the game\'s brightness.'],
    ['sFullscreen','Settings.Fullscreen', 'Fullscreen', 'Toggles Fullscreen Mode.'],
    ['sVsync','Settings.VSync', 'Vsync', 'Vsync elimates the screen tearing by synchronizing the game\'s frame rate with the display\'s refresh rate. Turning it ON will limit your maximum FPS.'],
    ['sAntiAliasing','Settings.Antialiasing', 'Anti-Aliasing', 'Anti-Aliasing smoothes edges, transparent textures and particles, reducing jagged visuals on both low and high resolutions.'],
    ['sTextureResolution','Settings.TextureResolution', 'Texture Resolution', 'Adjusts the resolution and fidelity of the in-game textures. Higher settings use up more VRAM.'],
    ['sTextureFiltering','Settings.TextureFilteringQuality', 'Texture Filtering', ' Increases the sharpness of textures viewed at a distance or angle. If this setting is lower textures will appear muddy from far away.'],
    ['sLightningQuality','Settings.LightingQuality', 'Lighting Quality', 'Enhances the realism of all lighting effects.'],
    ['sEffectsQuality','Settings.EffectsQuality', 'Effects Quality', 'Controls the number of visual effects seen at any one time, and their level of detail.'],
    ['sShadowQuality','Settings.ShadowQuality', 'Shadow Quality', 'Determines the resolution and filtering of shadows in the game.'],
    ['sDetailsLevel','Settings.DetailsQuality', 'Details Level', 'Adjusts the distance objects will be rendered. The lower the detail level, the shorter distance objects will be rendered.'],
    ['sPostprocessing','Settings.PostprocessingQuality', 'Postprocessing', 'Adjusts the quality of frames after they have been rendered.'],
    ['sMotionBlur','Settings.MotionBlur', 'Motion Blur', 'Motion Blur is a general velocity-based blurring that is applied when either the camera or objects move rapidly on screen.'],
    ['sMasterVolume','Settings.MasterVolume', 'Master Volume', 'Adjusts all volumes.'],
    ['sSFXVolume','Settings.SfxVolume', 'SFX Volume', 'Adjusts the volume of sound effects.'],
    ['sMusicVolume','Settings.MusicVolume', 'Music Volume', 'Adjusts the volume of music.'],
    ['sHudShake','Settings.HUDShake', 'HUD Shake', 'Enables/Disables HUD shake.'],
    ['sPlayerMarkerColors','Settings.PlayerMarkerColors', 'Player Marker Colors', 'Changes the colour scheme of over-head player markers.'],
    ['sCameraFOV','Camera.FOV', 'Camera FOV', 'Adjusts the first person field of view.'],
    ['cCenteredCrosshairFirst', 'Camera.CenteredCrosshairFirst', 'Centered Crosshair (First Person)', 'Centers the crosshair in first person.'], 
    ['cCenteredCrosshairThird', 'Camera.CenteredCrosshairThird', 'Centered Crosshair (Third Person)', 'Centers the crosshair in third person.'], 
    ['cHideHUD', 'Camera.HideHUD', 'Hide HUD', 'Hides the HUD.'], 
    ['inputRaw','Input.RawInput', 'Use Raw Input', 'Uses raw input, bypassing Windows mouse acceleration.'],  
    ['lookSensitivity', 'Input.ControllerSensitivityY', 'Look Sensitivity', 'Adjusts the sensitivity of the look joystick.'], 
    ['controllerPort','Input.ControllerPort', 'Controller Port Number', 'Specifies which controller port to receive input from.'], 
    ['gExpandScoreboard','Game.ExpandedScoreboard', 'Always Expand Scoreboard', 'Always displays the expanded scoreboard.'], 
    ['invertLook','Input.ControllerInvertY', 'Invert Look', 'Inverts the look joystick vertically.'], 
    ['gHideChat','Game.HideChat', 'Hide Chat', 'Hides the in-game text chat.'], 
    ['gSuppressJuggling','Game.SuppressJuggling', 'Suppress Juggling Announcer Spam', 'Supresses the audio spam from juggling.'], 
    ['wOffsetConfig','Weapon.JSON.File', 'Viewmodel Config', 'Specifies which viewmodel config file to use.'],
    ['gMedalPack','Game.MedalPack', 'Medal Pack', 'Specifies which Medals Pack to use.'], 
    ['iSpectateSens','Input.SpectateSensitivity', 'Spectator Camera Senstivity', 'Adjusts the Camera Sensentivity for the Spectate Cam.'],
    ['iDisableSprint','Input.ToggleSprint', 'Toggle Sprint', 'Toggles sprint on press instead of disabling on release.'],
    ['gIconSet','Game.IconSet', 'Controller Button Icon Set', 'Sets the button icon set for the menus.'],
    ['vEnabled','VoIP.Enabled', 'VoIP Enabled', 'Toggles Voice over IP.'],
    ['vMicrophoneID','VoIP.MicrophoneID', 'Microphone ID', 'Specifies the Microphone ID.'],
    ['vPTTEnable','VoIP.PTT_Enabled', 'Push To Talk', 'Transmits voice only while a bound key or button is held.'],
    ['vAGC','VoIP.AGC', 'Automatic Gain Control', 'Controls signal amplitude at its output, despite variation of the amplitude in the input signal.'],
    ['vNoiseSupress','VoIP.NoiseSupress', 'Noise Suppress', 'Removes background noise from the captured signal.'],
    ['vEchoCancelation','VoIP.EchoCancelation', 'Echo Cancellation', 'Removes echo by subtracting it from the transmitted or received signal.'],
    ['vStereoVoice','VoIP.StereoVoice', 'Allow Stereo Voice', 'Enables Stereo audio from other players.'],
	['vPTTSound','VoIP.PTTSoundEnabled', 'Enable PTT Noise', 'Play a noise when pressing the push to talk button'],
    ['tAgressiveAudioDiscard', 'Tweaks.AggressiveAudioDiscarding', 'Aggressive Audio Discarding', 'Prioritizies gun sounds over others to make audio cutoff less noticeable.'], 
    ['tDisableFog', 'Tweaks.DisableReactorFog', 'Reacthor', 'Removes some of the fog on Reactor which causes FPS drops indoors.'], 
    ['tDisableWeapOutline', 'Tweaks.DisableWeaponOutline', 'Weapon Outline Removal', 'Removes outlines from weapons on the ground.'], 
    ['tDisableHitmarkers', 'Tweaks.DisableHitMarkers', 'Disable weapon hitmarkers', 'Stops red indicators when any of your weapons cause damage.'], 
    ['tGruntBirthdayParty', 'Tweaks.GruntBirthdayParty', 'Grunt Birthday Party', 'Confetti flies from headshots as the children cheer.'],
    ['tReachFrags','Tweaks.ReachStyleFrags', 'Reach Style Frags', 'Give tossed grenades a Halo: Reach styled orangey-yellow trail.'],
    ['tIntelBloomPatch','Tweaks.IntelBloomPatch', 'Bloom Patch', 'Fixes extreme brightness on sunny maps.'],
    ['sAudioDevice','Settings.AudioOutputDevice', 'Audio Device', 'Specifies which audio device the game should use.'],
    ['sContrast','Settings.Contrast', 'Contrast', 'Adjusts the game\'s contrast.'],
    ['controllerVibration', 'Input.ControllerVibrationIntensity', 'Controller Vibration', 'Adjusts the strength of controller vibration.'],
    ['stickLayout', 'Input.ControllerStickLayout', 'Stick Layout', 'Adjusts the controller layout.'],
    ['xSens', 'Input.ControllerSensitivityX', 'Controller Sensitivity - X Axis', 'Adjusts the controller sensitivity on the X-axis.'],
    ['ySens', 'Input.ControllerSensitivityY', 'Controller Sensitivity - Y Axis', 'Adjusts the controller sensitivity on the Y-axis.'],
    ['sQualityPreset', '', 'Quality Preset', 'Adjusts the overall graphics quality.'],
    ['presetMenu', '', 'Button Layout', 'Changes the button layout.'],
    ['gdEnabled','Game.Discord.Enable', 'Discord Rich Presence Enabled', 'Toggles Rich Presense in Discord'],
    ['gdAutoAccept','Game.Discord.AutoAccept', 'Discord Auto Accept', 'Always accept join requests'],
    ['gCefMedals','Game.CefMedals', 'Use H3 Medal System', 'Uses Built-in medals instead of ED medal system']
];
var binds = [
    ['Sprint','Sprint','Infantry'],
    ['Jump','Jump','Infantry'],
    ['Crouch','Crouch','Infantry'],
    ['Use','Use','Infantry'],
    ['DualWield','Dual Wield','Infantry'],
    ['Fire','Fire','Infantry'],
    ['FireLeft','Fire Dual','Infantry'],
    ['Reload','Reload','Infantry'],
    ['ReloadLeft','Reload Dual','Infantry'],
    ['Zoom','Zoom','Infantry'],
    ['SwitchWeapons','Switch Weapons','Infantry'],
    ['Melee','Melee','Infantry'],
    ['Grenade','Grenade','Infantry'],
    ['SwitchGrenades','Switch Grenades','Infantry'],
    ['VehicleAccelerate','Vehicle Accelerate','Vehicle'],
    ['VehicleBrake','Vehicle Brake','Vehicle'],
    ['VehicleBoost','Vehicle Boost','Vehicle'],
    ['VehicleRaise','Vehicle Raise','Vehicle'],
    ['VehicleDive','Vehicle Dive','Vehicle'],
    ['VehicleFire','Vehicle Fire','Vehicle'],
    ['VehicleAltFire','Vehicle Alt Fire','Vehicle'],
    ['BansheeBomb','Banshee Bomb','Vehicle'],
    ['Menu','Menu','UI'],
    ['Scoreboard','Scoreboard','UI'],
    ['Chat','Chat','UI'],
    ['TeamChat','Team Chat','UI'],
    ['UseEquipment','Use Equipment','Infantry'],
    ['VoiceChat','Voice Chat PTT','UI'],
    ['Flashlight','Toggle Flashlight','Infantry'],
    ['Forward','Forward','Infantry'],
    ['Back','Back','Infantry'],
    ['Left','Left','Infantry'],
    ['Right','Right','Infantry'],
    ['NextPlayer','Spectate Next Player','UI'],
    ['PrevPlayer','Spectate Prev Player','UI'],
    ['UiLeftBumper','UiLeftBumper','UI'],
    ['UiRightBumper','UiRightBumper','UI']
];

var buttons = ["","A","B","X","Y","RB","LB","LT","RT","Start","Back","LS","RS","Left","Right","Up","Down"];

var controllerPresets = [
    ["Halo Online Default","LS,A,X,RB,LB,RT,LT,RB,LB,RS,Y,B,LT,Right,,,LT,A,X,RT,LT,B,Start,Back,,,LB,Down,Up"],
    ["Halo 3 Default","Right,A,LS,RB,LB,RT,LT,RB,LB,RS,Y,B,LT,LB,,,LT,A,LS,RT,LT,B,Start,Back,,,X,Down,Up"],
    ["Halo 3 Southpaw","Right,A,LS,RB,LB,LT,RT,RB,LB,RS,Y,B,RT,LB,,,RT,A,LS,LT,RT,B,Start,Back,,,X,Down,Up"],
    ["Halo 3 Boxer","Right,A,LS,RB,LB,RT,LT,RB,LB,RS,Y,LT,B,LB,,,LT,A,LS,RT,LT,B,Start,Back,,,X,Down,Up"],
    ["Halo 3 Green Thumb","Right,A,LS,RB,LB,RT,LT,RB,LB,B,Y,RS,LT,LB,,,LT,A,LS,RT,LT,B,Start,Back,,,X,Down,Up"],
    ["Halo 3 Bumper Jumper","Right,LB,LS,B,A,RT,LT,B,A,RS,Y,RB,LT,A,,,LT,LB,LS,RT,LT,RB,Start,Back,,,X,Down,Up"],
    ["Halo 3 Walkie Talkie","Right,A,LS,B,X,RT,LT,B,X,RS,Y,RB,LT,A,,,LT,A,LS,RT,LT,B,Start,Back,,,Up,LB,Up"],
    ["Halo Reach Default","LB,A,LS,X,B,RT,LT,X,LB,RS,Y,RB,LT,B,,,LT,A,LS,RT,LT,B,Start,Back,,,Right,Down,Up"],
    ["Halo Reach Southpaw","RB,A,LS,X,B,LT,RT,X,RB,RS,Y,LB,RT,B,,,RT,A,LS,LT,RT,B,Start,Back,,,Right,Down,Up"],
    ["Halo Reach Boxer","LB,A,LS,X,B,RT,LT,X,LB,RS,Y,LT,RB,B,,,LT,A,LS,RT,LT,B,Start,Back,,,Right,Down,Up"],
    ["Halo Reach Green Thumb","LB,A,LS,X,B,RT,LT,X,LB,RB,Y,RS,LT,B,,,LT,A,LS,RT,LT,B,Start,Back,,,Right,Down,Up"],
    ["Halo Reach Bumper Jumper","X,LB,LS,B,A,RT,LT,B,A,RS,Y,RB,LT,A,,,LT,LB,LS,RT,LT,RB,Start,Back,,,Right,Down,Up"],
    ["Halo Reach Recon","LB,A,LS,RB,X,RT,LT,RB,LB,RS,Y,B,LT,X,,,LT,A,LS,RT,LT,B,Start,Back,,,Right,Down,Up"],
    ["Halo 4 Default","LS,A,B,X,LB,RT,LT,X,LB,RS,Y,RB,LT,Right,,,LT,A,B,RT,LT,RB,Start,Back,,,LB,Down,Up"],
    ["Halo 4 Southpaw","LS,A,B,X,RB,LT,RT,X,RB,RS,Y,LB,RT,Right,,,RT,A,B,LT,RT,LB,Start,Back,,,LB,Down,Up"],
    ["Halo 4 Boxer","B,A,LS,X,LB,RT,LT,X,LB,RS,Y,LT,RB,Right,,,LT,A,LS,RT,LT,B,Start,Back,,,LB,Down,Up"],
    ["Halo 4 Green Thumb","LS,A,B,X,LB,RT,LT,X,LB,RB,Y,RS,LT,Right,,,LT,A,B,RT,LT,RB,Start,Back,,,LB,Down,Up"],
    ["Halo 4 Bumper Jumper","A,LB,LS,B,X,RT,LT,B,X,RS,Y,RB,LT,Right,,,LT,LB,LS,RT,LT,RB,Start,Back,,,X,Down,Up"],
    ["Halo 4 Recon","X,A,LS,RB,LB,RT,LT,RB,LB,RS,Y,B,LT,Right,,,LT,A,LS,RT,LT,B,Start,Back,,,LB,Down,Up"],
    ["Halo 4 Fishstick","LS,A,B,X,LB,RT,LT,X,LB,LT,Y,RS,RB,Right,,,LT,A,B,RT,LT,RS,Start,Back,,,LB,Down,Up"]
];
var controllerIconPacks = [
    ['Xbox 360','360'],
    ['Xbox One','XboxOne']
];
var bindChangeArray = [];
var subPages = ['#page4','#page5','#page8','#page9','#page11'];

$(document).ready(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            if(window.location.hash != '#page5'){
                if(!activePage.endsWith('alertBox')){
                    cancelButton();
                }else{
                    if($('#dismissButton:visible').length){
                        hideAlert(true);
                    }
                }
            }
        }
        if (e.keyCode == 44) {
            dew.command('Game.TakeScreenshot');  
        }
    });
    $(document).keydown(function(e){
        if(e.keyCode == 192 || e.keyCode == 223){
            dew.show('console');
        }
        if(window.location.hash != '#page5'){
            if(e.keyCode == 81){//Q
                prevPage();
            }
            if(e.keyCode == 69){//E
                nextPage();
            }
        }
    });
    setButtonLists();
    setOptionList('presetMenu', controllerPresets);
    setOptionList('gIconSet', controllerIconPacks);
    dew.command('Game.ListMedalPacks', {}).then(function(response) {
        var packArray = [];
        var packs = response.split(',');
        for (i = 0; i < packs.length; i++){
            if(packs[i].indexOf(" ") == -1){
                packArray.push([packs[i],packs[i]]);
            }
        }
        setOptionList('gMedalPack', packArray);
    });
    dew.command('Weapon.JSON.List', {}).then(function(response) {
        var offsetArray = [];
        var offsets = response.split(',');
        for (i = 0; i < offsets.length; i++){
            if(offsets[i].indexOf(" ") == -1){
                offsetArray.push([offsets[i],offsets[i]]);
            }
        }
        setOptionList('wOffsetConfig', offsetArray);
    });
    $('.tabs li a').off('click').on('click',function(e){
        $('.tabs li').removeClass('selected');
        $(this).parent().addClass('selected');
        window.location.href = e.target.href;
        activePage = e.target.hash;
        itemNumber = 0;
        $(e).ready(function(){
            if(hasGP){
                updateSelection(0, false, true, true);
            }
            tabIndex = $('.tabs li:visible a').index($("a[href='"+activePage+"']"));
        });
        dew.command('Game.PlaySound 0x0B00');
    });
    $('.tinySetting').on('change', function(){
        var newID = $(this).attr('id');
        if(newID.endsWith('Text')){
            newID = newID.slice(0, -4);
        }
        $('#'+newID).val($(this).val());
    });
    $('input[type=range]').on('input', function(){
        var newID = $(this).attr('id') + 'Text';
        $('#'+newID).val($(this).val());
    });
    var lastSoundPlayedTime = Date.now();
    $('#settingsWindow input:not(#lookSensitivity,#lookSensitivityText), #settingsWindow select,#settingsWindow textarea').on('change', function(e){
        var elementID = e.target.id;
        if($('#'+elementID).hasClass('tinySetting') && e.target.id.endsWith('Text')){
            elementID = elementID.slice(0, -4);
        }
        var newValue = e.target.value;
        if(e.target.computedRole == 'checkbox'){
            if($('#'+elementID).is(':checked')){
                newValue = '1';
            }else{
                newValue = '0';
            }
        }
        $.grep(settingsToLoad, function(result){
            if(result[0] == elementID){
                queueChange([result[1], newValue]);
            };
        });
        if(playSound){
            var currentTime = Date.now();
            if((currentTime - lastSoundPlayedTime) > 100) {
                lastSoundPlayedTime = currentTime;
                dew.command('Game.PlaySound 0x0B00');
            }
        }
    });
    $('#lookSensitivity, #lookSensitivityText').on('change', function(e){
        var yVal = 30 + (e.target.value * 10);
        var xVal = yVal * 2;
        queueChange(['Input.ControllerSensitivityX', xVal]);
        queueChange(['Input.ControllerSensitivityY', yVal]);
    });
    $('#controllerVibration, #controllerVibrationText').on('change', function(e){
        dew.command('Input.ControllerVibrationIntensity ' + $(this).val(), {}).then(function(x){
            dew.command('Input.ControllerVibrationTest');
        });
    });
    dew.command('Graphics.SupportedResolutions', {}).then(function(response){
        var supportedArray = JSON.parse(response);
        var resolutionArray = [['Default','default']];
        for(i = 0; i < supportedArray.length; i++){
            resolutionArray.push([supportedArray[i],supportedArray[i]]);
        }
        setOptionList('sScreenResolution', resolutionArray);
    });
    $('#applyButton').off('click').on('click', function(e){
        applyButton();
    });
    $('#resetButton').off('click').on('click', function(e){
        resetButton();
    });
    $('#cancelButton').off('click').on('click', function(e){
        cancelButton();
    });
    $('#sTextureResolution, #sTextureFiltering, #sLightningQuality, #sEffectsQuality, #sShadowQuality, #sDetailsLevel, #sPostprocessing').on('change', function(e){
        if($('#sTextureResolution').val() == e.target.value && $('#sTextureFiltering').val() == e.target.value && $('#sLightningQuality').val() == e.target.value && $('#sEffectsQuality').val() == e.target.value && $('#sDetailsLevel').val() == e.target.value && $('#sPostprocessing').val() == e.target.value){
            $('#sQualityPreset').val(e.target.value);
        }else{
            $('#sQualityPreset').val('custom');
        }
    });
    $('#sQualityPreset').on('change', function(e){
        if(e.target.value != 'custom'){
            $('#sTextureResolution').val(e.target.value);
            $('#sTextureFiltering').val(e.target.value);
            $('#sLightningQuality').val(e.target.value);
            $('#sEffectsQuality').val(e.target.value);
            $('#sDetailsLevel').val(e.target.value);
            $('#sPostprocessing').val(e.target.value);
            $('#sShadowQuality').val(e.target.value);
            if(e.target.value == 'high'){
                 $('#sMotionBlur').prop('checked', true);
            }else{
                $('#sMotionBlur').prop('checked', false);
            }
            playSound = false;
            $('.video').trigger('change');
            playSound = true;
        }
    });
    $('#presetMenu').on('change', function(){
        applyBindString(this.value);
    });
    $('#wOffsetConfig').on('change', function(){
        changeArray.push(['Weapon.JSON.Load', '']);
    });    
    navigator.mediaDevices.enumerateDevices().then(function(devices){
        var deviceArray = [['Default','']];
        for (i = 0; i < devices.length; i++){
            if(devices[i].kind == "audioinput" && devices[i].label){
                deviceArray.push([devices[i].label,devices[i].label]);
            }
        }
        setOptionList('vMicrophoneID', deviceArray);
    });
    setControlValues();
    initializeBindings();
    $('.bind').on('change', function(){
        queueBindChange([this.id, this.value]);
        updateBindLabels();
    });
    
    dew.command('Server.SprintEnabled', {}).then(function(a){
        dew.command('Server.UnlimitedSprint', {}).then(function(b){
            if(a == '1' && b == '1'){
                $('#sSprint').val('2');
            }else if(a == '1' && b == '0'){
                $('#sSprint').val('1');
            }else{
                $('#sSprint').val('0');
            }
        });       
    });
    dew.command('Settings.AudioOutputDeviceList', {}).then(function(response) {
        var audioDevArray = [];
        var devs = JSON.parse(response);
        for (i = 0; i < devs.length; i++){
                audioDevArray.push([devs[i],i]);
        }
        setOptionList('sAudioDevice', audioDevArray);
    });
    
    dew.on('controllerinput', function(e){    
        if(hasGP){    
            if(e.data.A == 1){
                if(activePage.endsWith('alertBox')){
                    if($('#dismissButton:visible').length){
                        dismissButton();
                    }else{
                        hideAlert(true);
                    }
                }else if($('#'+selectedItem).prev()[0].computedRole == 'button'){
                    $('#'+selectedItem).prev().click();
                }else{    
                    toggleSetting();
                }
            }
            if(e.data.B == 1){
                if(!activePage.endsWith('alertBox')){
                    cancelButton();
                }else{
                    hideAlert(true);
                }
            }
            if(e.data.X == 1){
                if($('#'+selectedItem).prev()[0].computedRole == 'button'){
                    $('#'+selectedItem).prev().click();
                }
            }
            if(e.data.Y == 1){
                resetButton();
            }
            if(e.data.Up == 1){
                upNav();
            }
            if(e.data.Down == 1){
                downNav();
            }
            if(e.data.Left == 1){
                leftToggle();
            }
            if(e.data.Right == 1){
                rightToggle();
            }
            if(e.data.LeftBumper == 1){
                prevPage();
            }
            if(e.data.RightBumper == 1){
                nextPage();
            }
            if(e.data.Start == 1){
                applyButton();
                hideAlert(true);
            }
            if(e.data.LeftTrigger != 0){
                if(itemNumber > 0){
                    itemNumber = 0;
                    updateSelection(itemNumber, true, true, true);
                }
            }
            if(e.data.RightTrigger != 0){
                if(itemNumber < $(activePage + ' label:visible').length-1){
                    itemNumber = $(activePage + ' label:visible').length-1;
                    updateSelection(itemNumber, true, true, true);
                }
            }
            if(e.data.AxisLeftX > axisThreshold){
                stickTicks.right++;
            }else{
                stickTicks.right = 0;
            }
            if(e.data.AxisLeftX < -axisThreshold){
                stickTicks.left++;
            }else{
                stickTicks.left = 0; 
            }
            if(e.data.AxisLeftY > axisThreshold){
                stickTicks.up++;
            }else{
                stickTicks.up = 0;
            }
            if(e.data.AxisLeftY < -axisThreshold){
                stickTicks.down++;
            }else{
                stickTicks.down = 0;
            }
        }
    });
    var clicking = false;
    var currentPos = {x: null, y: null};
    $('#playerWindow').mousedown(function(){
        clicking = true;
    });
    $(document).mouseup(function(){
        clicking = false;
    })
    $('span').has('.setting').off('mouseover').on('mouseover', function(){
        $('.selectedElement').removeClass('selectedElement');
        $(this).addClass('selectedElement');
        itemNumber = $(activePage+' span').has('.setting').index($(this));
        if(itemNumber > -1){
            updateSelection(itemNumber, false, false, false); 
            setInfoBox($(this).find('.setting').attr('id'));
        }
    });
    $('#sVsync').on('change', function(){
        alertBox('This change requires a restart to take effect.', false);
    });
    $('.cefMedals').on('change', function(){
        if(!$('#gCefMedals').is(":checked")){
            alertBox('This setting only effects Custom medal packs. Turning them on now.', false);
            $('#gCefMedals').prop('checked', true);
            $('#gCefMedals').trigger('change');
        }
    });
    $('#okButton').off('click').on('click', function(){
        if($('#dismissButton:visible').length){
            dismissButton();
        }else{
            hideAlert(true);
        }
    });
    $('#dismissButton').off('click').on('click', function(){
        hideAlert(true);
    });
});

function checkGamepad(){
    var shouldUpdateHeld = false;
    if(Date.now() - lastHeldUpdated > 100) {
        shouldUpdateHeld = true;
        lastHeldUpdated = Date.now();
    }
    if(stickTicks.up == 1 || (shouldUpdateHeld && stickTicks.up > 25)){
        upNav();
    }
    if(stickTicks.down == 1 || (shouldUpdateHeld && stickTicks.down > 25)){
        downNav();
    }
    if(stickTicks.left == 1 || (shouldUpdateHeld && stickTicks.left > 25)){
        leftToggle();
    }
    if(stickTicks.right == 1 || (shouldUpdateHeld && stickTicks.right > 25)){
        rightToggle();
    }
};

function setButtons(){
    dew.command('Game.IconSet', {}).then(function(response){
        $('#randomArmor img').attr('src','dew://assets/buttons/' + response + '_X.png');
        $('#randomColors img').attr('src','dew://assets/buttons/' + response + '_Y.png');
        $('#resetButton img').attr('src','dew://assets/buttons/' + response + '_Y.png');
        $('#applyButton img').attr('src','dew://assets/buttons/' + response + '_Start.png');
        $('#cancelButton img').attr('src','dew://assets/buttons/' + response + '_B.png');
        $('#dismissButton img').attr('src','dew://assets/buttons/' + response + '_B.png');
        $('#okButton img').attr('src','dew://assets/buttons/' + response + '_A.png');
        $('.tabs img').eq(0).attr('src','dew://assets/buttons/' + response + '_LB.png');
        $('.tabs img').eq(1).attr('src','dew://assets/buttons/' + response + '_RB.png');
    });
}

var mapName = "mainmenu";
dew.on('show', function(e){
    if(!jQuery.isEmptyObject(e.data)){
        switch(e.data){
            case "game": 
                tabIndex = 0;
            break;
            case "controls": 
                tabIndex = 1;
            break;
            case "video": 
                tabIndex = 2;
            break;
            case "sound": 
                tabIndex = 3;
            break;
            default:
        }
    }else{
        tabIndex = 0;
    };
    $('#settingsWindow').hide();
    $('#blackLayer').hide();
    dew.getSessionInfo().then(function(i){
        mapName = i.mapName;
        if(i.mapName == "mainmenu"){
            $('#blackLayer').fadeIn(200, function() {
                dew.command('game.hideh3ui 1');
                $('#settingsWindow').show();
                $('#blackLayer').show();
                initActive();
                initGamepad();
            }).fadeOut(200);
        } else {
            $('#settingsWindow').show();
            initActive();
            initGamepad();
        }
    });
    setControlValues();

});

function showWeaponOffsets(){
    if(mapName != "mainmenu"){
        dew.hide();
        dew.show('weapon_offset');
    }else{
        alertBox('Weapon offsets can only be edited in-game.', false);
    }
}

function initGamepad(){
    dew.command('Settings.Gamepad', {}).then(function(result){
        if(result == 1){
            hasGP = true;
            if(!repGP){
                repGP = window.setInterval(checkGamepad,1000/60);
            }
            onControllerConnect();
            setButtons();
            $('button img,.tabs img').show();
        }else{
            onControllerDisconnect();
            hasGP = false;
            if(repGP){
                window.clearInterval(repGP);
                repGP = null;
            }
            $('button img,.tabs img').hide();
        }
    });
}

dew.on('hide', function(e){
    if(repGP){
        window.clearInterval(repGP);
        repGP = null;
    }
    hideAlert(false);
    dew.command('Game.HideH3UI 0');
});

function initActive(){
    $('.selected').removeClass('selected');
    $('.tabs li:visible').eq(tabIndex).addClass('selected');
    location.hash = $('.selected a')[0].hash;
    activePage = window.location.hash;
}

function setControlValues(){
    commandValues = [];
    dew.getCommands().then(function (commands){
        for(i = 0; i < commands.length; i++){
            var setValue = commands[i].value;
            $.grep(settingsToLoad, function(result){
                if(result[1] == commands[i].name){
                    commandValues.push([result[0],commands[i].name,commands[i].value]);
                    if(result[1].startsWith('Player.Colors')){
                        $('#'+result[0]).css('background-color',setValue);   
                        $('#'+result[0]).val(setValue); 
                        if(getLuminance(setValue)> 0.22){
                            $('#'+result[0]).css('color','#222');
                        }else{
                            $('#'+result[0]).css('color','#ddd');
                        }
                    }else if(result[1].startsWith('Input.ControllerSensitivityY')){
                        $('#ySens, #ySensText').val(setValue);
                        var h3Val = (setValue-30)/10;
                        $('#lookSensitivity, #lookSensitivityText').val(h3Val);
                    }else if(result[1].startsWith('Settings.PostprocessingQuality')){
                        $('#'+result[0]).val(setValue);
                        if($('#sTextureResolution').val() == setValue && $('#sTextureFiltering').val() == setValue && $('#sLightningQuality').val() == setValue && $('#sEffectsQuality').val() == setValue && $('#sShadowQuality').val() == setValue && $('#sDetailsLevel').val() == setValue && $('#sPostprocessing').val() == setValue){
                            $('#sQualityPreset').val(setValue);
                        }else{
                            $('#sQualityPreset').val('custom');
                        }
                    }else{
                        if($('#'+result[0]).is(':checkbox')){
                            if(setValue == '1'){
                                $('#'+result[0]).prop('checked', true);
                            }else{
                                $('#'+result[0]).prop('checked', false);                               
                            }
                        }else{
                            if($('#'+result[0]).hasClass('tinySetting')){
                                setValue = parseFloat(setValue);
                            }
                            $('#'+result[0]).val(setValue);
                        }
                        $('#'+result[0]).val(setValue);
                        if($('#'+result[0]).hasClass('hasTiny')){
                            if(setValue.isFloat){
                                $('#'+result[0]+'Text').val(parseFloat(setValue.toFixed(2)));
                            }else{                            
                                $('#'+result[0]+'Text').val(setValue);
                            }
                        }
                    }
                };
            });
        }
    })
}

function switchPage(pageHash){
    itemNumber = 0;
    location.href=pageHash;
    activePage=pageHash;    
    if(hasGP){
        updateSelection(0, true, true, true);
    }
    if(subPages.indexOf(pageHash) != -1){
        $('#cancelButton').html('<img class="button">Back');
    }else{
        if(changeArray.length || bindChangeArray.length){
            $('#cancelButton').html('<img class="button">Cancel');
        }else{
            $('#cancelButton').html('<img class="button">Close');
        }
    }
    initGamepad();
}

function editControls(which){
    if(which == 0){
        switchPage('#page5');
    }else{
        switchPage('#page8');
    }
}

function applySettings(i){
    if(i != changeArray.length){
        dew.command(changeArray[i][0] + ' "' + changeArray[i][1] + '"', {}).then(function(){
			i++;
			applySettings(i);            
        }).catch(function (error){
            console.log(error);
            i++;
			applySettings(i);  
        });
    }else{
        dew.notify("settings-update", changeArray);
        changeArray = [];
        dew.command('writeconfig');
        dew.command('VoIP.Update');
        if(subPages.indexOf(activePage) != -1){
            $('#cancelButton').html('<img class="button">Back');
        }else{
            $('#cancelButton').html('<img class="button">Close');
        }
        $('#applyButton').hide();
        initGamepad();
    }
}

function applyBindChanges(i){
    if(i != bindChangeArray.length){    
        if (bindChangeArray[i][1] == "Back") { bindChangeArray[i][1] = "Select"; }
        if (bindChangeArray[i][1]) { bindChangeArray[i][1] = "\"" + bindChangeArray[i][1] + "\""; }
        dew.command("Input.ControllerAction \"" + bindChangeArray[i][0] + "\" " + bindChangeArray[i][1], {}).then(function(){
            i++;
            applyBindChanges(i);            
        }).catch(function (error){
            console.log(error);
            i++;
            applyBindChanges(i);  
        });
    }else{
        bindChangeArray = [];
        dew.command('writeconfig');
        if(subPages.indexOf(activePage) != -1){
            $('#cancelButton').html('<img class="button">Back');
        }else{
            $('#cancelButton').html('<img class="button">Close');
        }
        initGamepad();
    }
}

function applyButton(){
    if(window.location.hash == '#page5'){
        applyBinds();
        switchPage('#page2'); 
        if(!changeArray.length && !bindChangeArray.length){
            $('#applyButton').hide();
        }
    }else if(window.location.hash == '#page4'){
        applySettings(0);
        switchPage('#page3');     
    }else if(window.location.hash == '#page11'){ 
        switchPage('#page8');        
    }else{
        if(changeArray.length || bindChangeArray.length){
            applySettings(0); 
            applyBindChanges(0);              
            setButtons();            
        }else{
            effectReset();
        }
    }
}

function resetButton(){
    if(window.location.hash == '#page5' || window.location.hash == '#page8' || window.location.hash == '#page9'){
        dew.command('Input.ResetBindings').then(function(){
            initializeBindings(); 
        });
    }else{
        dew.command('Settings.Reset').then(function(){
            setControlValues();
            initGamepad();
        });
    }
}

function cancelButton(){
    if(window.location.hash == '#page5'){
        initializeBindings(); 
        switchPage('#page2'); 
        $('#cancelButton').html('<img class="button">Close');
        if(!changeArray.length){
            $('#applyButton').hide();
        }
        if(hasGP){
            setButtons();
            $('button img,.tabs img').show();
        }        
    }else if(window.location.hash == '#page4'){
        setControlValues();      
        switchPage('#page3');
    }else if(window.location.hash == '#page9'){ 
        switchPage('#page8');
    }else if(window.location.hash == '#page8'){ 
        switchPage('#page2');
    }else if(window.location.hash == '#page11'){ 
        switchPage('#page8');
    }else if(changeArray.length || bindChangeArray.length){
        alertBox('You have unapplied settings', true);
    }else{
        itemNumber = 0;
        effectReset();
    }
}

function dismissButton(){
    hideAlert(false);  
    itemNumber = 0;
    effectReset();
    setControlValues();
    changeArray = [];
    if(bindChangeArray.length){
        initializeBindings();
        bindChangeArray = [];
    }
    $('#applyButton').hide();
}

exiting = false;
function effectReset(){
    // Prevent escape spamming
    if(exiting)
        return;
    exiting = true;

    dew.command('Game.PlaySound 0x0B04');
    dew.getSessionInfo().then(function(i){
        if(i.mapName == "mainmenu"){
            $('#blackLayer').fadeIn(200, function(){
                dew.command('game.hideh3ui 0');
                $('#settingsWindow').hide();
                $('#blackLayer').fadeOut(200, function(){
                    dew.hide();
                    $('#settingsWindow').show();
                    exiting = false;
                });
            });
        }else{
            dew.hide();
            exiting = false;
        }
    })
}

function applyBinds(){
    for(i=0; i<$('#bindBox tbody tr').length; i++){
        var attr = $('#bindBox tbody tr').eq(i).attr('data-action');
        if (typeof attr !== typeof undefined){
            var action = $('#bindBox tbody tr').eq(i).attr('data-action');
            var primaryKey = $('#bindBox tbody tr').eq(i).find('input').eq(0).val();
            var secondaryKey = $('#bindBox tbody tr').eq(i).find('input').eq(1).val();
            dew.command('Input.KeyboardAction ' + action + ' ' + primaryKey + ' ' + secondaryKey);
        }
    }
    dew.command('writeconfig');
}

function setOptionList(ElementID, ArrayVar){
    var sel = document.getElementById(ElementID);
    for(var i = 0; i < ArrayVar.length; i++){
        var opt = document.createElement('option');
        opt.innerHTML = ArrayVar[i][0];
        opt.value = ArrayVar[i][1];
        sel.appendChild(opt);
    }
}

function applyBindString(bindString){
    var bindArray = new Array(bindString.split(','));
    for (i = 0; i < bindArray[0].length; i++) { 
        $('#'+binds[i][0]).val(bindArray[0][i]);
        queueBindChange([binds[i][0],bindArray[0][i]])
    }
    updateBindLabels();
}

function initializeBindings(){
    dew.command("Input.DumpBindingsJson", {}).then(function(response){
        $('#bindBox tbody').empty();
        $('#bindBox tbody').each(function(i, e){
            if(i > 0)
                $(this).append('<tr style="height: 2.5vh"><th colspan="3"></th></tr>');
        });
        var bindDump = JSON.parse(response);
        for (i = 0; i < bindDump.length; i++){
            if(bindDump[i].controllerButton=="Select"){
                bindDump[i].controllerButton="Back";
            }
            $('#'+bindDump[i].actionName).val(bindDump[i].controllerButton);
            $.grep(binds, function(result, index){   
                if(result){
                if(result[0] == bindDump[i].actionName){            
                    var primaryBind = bindDump[i].primaryKey;
                    if(bindDump[i].primaryMouseButton != 'none'){
                        primaryBind = bindDump[i].primaryMouseButton;
                    }
                    var secondaryBind = bindDump[i].secondaryKey;
                    if(bindDump[i].secondaryMouseButton != 'none'){
                        secondaryBind = bindDump[i].secondaryMouseButton;
                    }
                    $('#bindBox .'+result[2]).append($('<tr data-action="'+result[0]+'"><td>'+result[1]+'</td><td><input class="keybind" value='+primaryBind+' data-initialvalue='+primaryBind+'></td><td><input class="keybind" value='+secondaryBind+' data-initialvalue='+secondaryBind+'></td></tr>'))
                }
                }
            })
        }
        
        updateBindLabels();
        getCurrentBindString();

        dew.on('mouse-xbutton-event', function(m){
            if(!document.activeElement.classList.contains('keybind'))
                return;

            if(m.data.xbutton == 1){
                document.activeElement.value = 'Mouse4';
            }else{
                document.activeElement.value = 'Mouse5';
            }; 
            document.activeElement.blur();
        });

        $('.keybind').on('focus blur', function(e){
            var this_ = $(this);

            function keyHandler(e){
                e.preventDefault();
                e.stopPropagation();

                // not escape
                if(e.keyCode == 27){
                    if(this_.data('initialvalue') ){
                        if(this_.data('initialvalue')  == this_.val()){
                            this_.val('none');
                        }else{
                            this_.val(this_.data('initialvalue') );
                        }
                    }
                    this_.blur();
                    return;
                }

                // A-Z, 0-9
                if((e.keyCode >= 65 && e.keyCode <= 90) || (e.keyCode >= 48 && e.keyCode < 58))
                    this_.val( String.fromCharCode(e.keyCode));

                if(e.keyCode >= 96 && e.keyCode <=105)
                    this_.val('Numpad'+String(parseInt(e.keyCode)-96))
                
                switch(e.keyCode){
                    case 8: 
                        this_.val('Back'); 
                    break;
                    case 9: 
                        this_.val('Tab'); 
                    break;
                    case 13:
                        if(e.originalEvent.location === KeyboardEvent.DOM_KEY_LOCATION_NUMPAD){
                            this_.val('NumpadEnter');
                        }else{
                            this_.val('Enter');
                        }
                    break;
                    case 16: 
                        if(e.originalEvent.location === KeyboardEvent.DOM_KEY_LOCATION_LEFT){
                           this_.val('LShift'); 
                        }else{
                           this_.val('RShift'); 
                        }   
                    break
                    case 17:
                        if(e.originalEvent.location === KeyboardEvent.DOM_KEY_LOCATION_LEFT){
                           this_.val('LControl'); 
                        }else{
                           this_.val('RControl'); 
                        }   
                    break;
                    case 18:
                        if(e.originalEvent.location === KeyboardEvent.DOM_KEY_LOCATION_LEFT){
                           this_.val('LAlt'); 
                        }else{
                           this_.val('RAlt'); 
                        }  
                    break;
                    case 20: 
                        this_.val('CapsLock'); 
                    break;
                    case 32: 
                        this_.val('Space'); 
                    break;
                    case 37: 
                        this_.val('Left'); 
                    break;
                    case 38: 
                        this_.val('Up'); 
                    break;
                    case 39: 
                        this_.val('Right'); 
                    break;
                    case 40: 
                        this_.val('Down'); 
                    break;
                    case 46: 
                        this_.val('Delete'); 
                    break;
                    case 106: 
                        this_.val('Multiply'); 
                    break;
                    case 107: 
                        this_.val('Add'); 
                    break;
                    case 109: 
                        this_.val('Subtract'); 
                    break;
                    case 110: 
                        this_.val('Decimal'); 
                    break;
                    case 111: 
                        this_.val('Divide'); 
                    break;
                    case 186: 
                        this_.val('Colon'); 
                    break;
                    case 187: 
                        this_.val('Plus'); 
                    break;
                    case 188: 
                        this_.val('Comma'); 
                    break;
                    case 189: 
                        this_.val('Minus'); 
                    break;
                    case 190: 
                        this_.val('Period'); 
                    break;
                    case 191: 
                        this_.val('Question'); 
                    break;
                    case 219: 
                        this_.val('LBracket'); 
                    break;
                    case 220: 
                        this_.val('Pipe'); 
                    break;
                    case 221: 
                        this_.val('RBracket'); 
                    break;
                    case 222: 
                        this_.val('Quote'); 
                    break;
                    default:
                        //console.log(e.keyCode);
                }
                this_.blur();
            };

            function mouseHandler(e){
                e.preventDefault();
                e.stopPropagation();
                if(e.type == 'mousewheel'){
                    
                    if(e.originalEvent.wheelDelta > 0){
                        this_.val('MouseWheelUp');
                    }else{
                        this_.val('MouseWheelDown');
                    }
                }
                else if(e.type == 'mousedown'){
                    switch(e.which){
                        case 1:
                            this_.val('MouseLeft');
                        break;
                        case 2:
                            this_.val('MouseMiddle');
                        break;
                        case 3:
                            this_.val('MouseRight');
                        break;
                        default:
                            this_.val('Mouse' + e.which);
                    }
                }
                this_.blur();

                return false;
            };

            var $doc = $(document);
            if(e.type == 'focus'){
                $doc.on('mousedown.rebind', mouseHandler);
                $doc.on('mousewheel.rebind', mouseHandler);
                $doc.on('keydown.rebind', keyHandler);
            }else{
                $doc.off('keydown.rebind');
                $doc.off('mousedown.rebind');
                $doc.off('mousewheel.rebind');
            }
        });
        $('.keybind').on('blur', function(e){
            $('#cancelButton').html('<img class="button">Cancel');
            $('#applyButton').show();
            if(hasGP){
                setButtons();
                $('button img,.tabs img').show();
            }
        });
    });
}

function updateBindLabels(){
    $('#controllerGraphic').children('div').empty();
    for (i = 0; i < binds.length-8; i++) { 
        var bind = document.getElementById(binds[i][0]).value;
        var action = binds[i][1];
        if(document.getElementById(bind)){
            var actionString = action;
            if(document.getElementById(bind).innerHTML.length > 0){
                actionString = ", " + action;
            }
            $("#" + bind).append(actionString);
        }
    }
}

function getCurrentBindString(){
    var currentBinds = "";
    for(var i = 0; i < binds.length-8; i++) {
        if($('#'+binds[i][0]).val()){
            currentBinds += $('#'+binds[i][0]).val() + ",";
        }else{
            currentBinds += ",";
        }
    }
    //console.log(currentBinds.slice(0, -1));
    $("#presetMenu").val(currentBinds.slice(0, -1));
}

function setButtonLists(){
    for(var i = 0; i < binds.length-8; i++) {
        var sel = document.getElementById(binds[i][0]);
        for(var x = 0; x < buttons.length; x++) {
            var opt = document.createElement('option');
            opt.innerHTML = buttons[x];
            opt.value = buttons[x];
            sel.appendChild(opt);
        }
    }
}

function updateSelection(item, sound, move, controller){
    colorIndex = 0;
    if(controller){
        $('.selectedElement').removeClass('selectedElement');
        $(activePage + ' label:visible').eq(item).parent().addClass('selectedElement');
    }
    selectedItem = $(activePage + ' .setting:visible').not('span').eq(itemNumber).attr('id');
    if(move){
        $('#'+selectedItem).parent()[0].scrollIntoView(false);
    }
    if(sound){
        dew.command('Game.PlaySound 0xAFE');
    }
    setInfoBox(selectedItem);
}

function prevPage(){
    if(tabIndex > 0){
        $('.tabs li:visible a').eq(tabIndex-1).click();
    }        
}

function nextPage(){
    var tabLength = $('.tabs li').length-1;
    if(tabIndex < tabLength){
        $('.tabs li:visible a').eq(tabIndex+1).click();
    }        
}

function upNav(){
    if(itemNumber > 0){
        itemNumber--;
        updateSelection(itemNumber, true, true, true);
    }
}

function downNav(){
    if(itemNumber < $(activePage + ' label:visible').length-1){
        itemNumber++;
        updateSelection(itemNumber, true, true, true);
    }           
}

function onControllerConnect(){
    updateSelection(itemNumber, false, true, true);
    $('button img, .tabs img').show();
}

function onControllerDisconnect(){
    $('.selectedItem').removeClass(); 
    $('button img, .tabs img').hide();
}

function leftToggle(){
    if(document.getElementById(selectedItem).computedRole == "combobox"){
        var elementIndex = $('#'+selectedItem+' option:selected').index();
        if(elementIndex > 0){
            var newElement = elementIndex - 1;
            $('#'+selectedItem+' option').eq(newElement).prop('selected', true);
            $('#'+selectedItem).trigger('change');
        }
    }
    if(document.getElementById(selectedItem).computedRole == "slider"){
        if(parseFloat(document.getElementById(selectedItem).value) > document.getElementById(selectedItem).min){
            document.getElementById(selectedItem).stepDown();
            document.querySelector('#'+selectedItem +'Text').value = document.getElementById(selectedItem).value; 
            $('#'+selectedItem).trigger('change');
        }
    }
    if($('#'+selectedItem).hasClass('color')){
        colorIndex = $.inArray($('#'+selectedItem).val(), h3ColorArray);
        if(colorIndex == -1){ colorIndex = 0 };       
        if($('#'+selectedItem).val()==h3ColorArray[colorIndex]){
            if(colorIndex > 0){
                colorIndex--;
            }    
        }
        setColor(h3ColorArray[colorIndex]);
    }
}

function rightToggle(){
    if(document.getElementById(selectedItem).computedRole == "combobox"){
        var elementIndex = $('#'+selectedItem+' option:selected').index();
        var elementLength = $('#'+selectedItem).children('option').length;
        if(elementIndex < elementLength - 1){
            var newElement = elementIndex + 1;
            $('#'+selectedItem+' option').eq(newElement).prop('selected', true);
            $('#'+selectedItem).trigger('change');
        } 
    }
    if(document.getElementById(selectedItem).computedRole == "slider"){
        if(parseFloat(document.getElementById(selectedItem).value) < document.getElementById(selectedItem).max){
            document.getElementById(selectedItem).stepUp();
            document.querySelector('#'+selectedItem +'Text').value = document.getElementById(selectedItem).value;   
            $('#'+selectedItem).trigger('change');
        }
    }
    if($('#'+selectedItem).hasClass('color')){
        colorIndex = $.inArray( $('#'+selectedItem).val(), h3ColorArray);
        if(colorIndex == -1){ colorIndex = 0 };
        if($('#'+selectedItem).val()==h3ColorArray[colorIndex]){
            if(colorIndex < h3ColorArray.length-1){
                colorIndex++;
            }      
        }
        setColor(h3ColorArray[colorIndex]);
    }
}

function toggleSetting(){
    if(document.getElementById(selectedItem).computedRole == "checkbox"){
        if(document.getElementById(selectedItem).checked){
            document.getElementById(selectedItem).checked = false;
        }else{
            document.getElementById(selectedItem).checked = true;
        }
        $('#'+selectedItem).trigger('change'); 
    }       
}

function queueChange(changeBlock){
    $('#cancelButton').html('<img class="button" src="dew://assets/buttons/360_B.png">Cancel');
    $('#applyButton').show();
    if(hasGP){
        setButtons();
        $('button img,.tabs img').show();
    }
    $.grep(changeArray, function(result, index){
        if(result){
            if(result[0] == changeBlock[0]){
                changeArray.splice(index,1);
            };
        }
    });
    changeArray.push(changeBlock);
}

function alertBox(alertText, dismissButton){
    if(dismissButton){
        $('#dismissButton').show();
    }else{
        $('#dismissButton').hide();
    }
    $('#wDescription').text(alertText);
    $('#alertBox').fadeIn(100);
    activePage = activePage+'alertBox';
    dew.command('Game.PlaySound 0x0B02');
}

function hideAlert(sound){
    $('#alertBox').hide();
    activePage = activePage.replace('alertBox', ''); 
    if(sound){
        dew.command('Game.PlaySound 0x0B04');
    }
}

function queueBindChange(changeBlock){
    $('#cancelButton').html('<img class="button">Cancel');
    $('#applyButton').show();
    if(hasGP){
        setButtons();
        $('button img,.tabs img').show();
    }
    $.grep(bindChangeArray, function(result, index){
        if(result){
            if(result[0] == changeBlock[0]){
                bindChangeArray.splice(index,1);
            };
        }
    });
    bindChangeArray.push(changeBlock);
}

function setInfoBox(ID){
    $.grep(settingsToLoad, function(result, index){
        if(result){
            if(result[0] == ID){
                $('#infoHeader').text(result[2]);
                $('#infoText').text(result[3]);
            };
        }
    });    
}

Number.prototype.isFloat = function() {
    return (this % 1 != 0);
}
