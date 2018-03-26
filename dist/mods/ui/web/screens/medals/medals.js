var fadeTime = 800;
var animTime = 18;
var removeTime = 4500;
var medalsPath = 'medals://';
var playQueue = [];
var eventJSONCache;
var playVol;
var suppressRepeat = false;
var medalWidth = '3.5vw';
var leftPos = '6.78vw';
var bottomPos = '32vh';
var transform = 'skew(0,-2.75deg)'; 
var juggleEvent = 0;
var juggleDelay = 3000;

var settingsArray = { 'Game.CefMedals': '1', 'Game.MedalPack': 'default', 'Game.SuppressJuggling': '0', 'Settings.MasterVolume': '50', 'Settings.SfxVolume': '100' };

dew.on("mpevent", function (event) {
    if(settingsArray['Game.CefMedals'] == 1){
        medalsPath = "medals://" + settingsArray['Game.MedalPack'] + "/";
        if(!eventJSONCache){
            $.getJSON(medalsPath+'events.json', function(json) {
                eventJSONCache = json;
                parseEvent(event);
            });
        }else{
            parseEvent(event);
        }
    }
});

function parseEvent(event){
    if(settingsArray['Game.SuppressJuggling'] == 1){
        suppressRepeat = true;
    } else {
        suppressRepeat = false;
    }
    var sfxVol = settingsArray['Settings.MasterVolume']/100;
    var mstrVol = settingsArray['Settings.SfxVolume']/100;
    playVol = sfxVol * mstrVol * 0.3;
    var medal = event.data.name;
    if(suppressRepeat && ( medal.startsWith('ctf_event_flag_') || medal.startsWith('assault_event_bomb_') || medal.startsWith('oddball_event_ball') || medal.startsWith('king_event_hill_c'))){
        juggleEvent++;
        setTimeout(function(){
            if(juggleEvent > 0){ juggleEvent--; }
        }, juggleDelay);
    } 
    if(juggleEvent > 2 && ((medal.startsWith('oddball_event_ball') && (medal != 'oddball_event_ball_spawned' && medal != 'oddball_event_ball_reset')) || (medal.startsWith('ctf_event_flag_') && medal != 'ctf_event_flag_captured')||(medal.startsWith('assault_event_bomb_') && medal != 'assault_event_bomb_placed_on_enemy_post') || medal.startsWith('king_event_hill_c'))){
        return
    }
    doMedal(event.data.name, event.data.audience);       
}

$.fn.pulse = function() { 
    var i = 0.5, x = 0, medal = this.selector;
    function pulseLoop(medal) { 
        setTimeout(function () {  
            $(medal).css({'transform': 'scale('+ i +','+ i +')', 'opacity': x });
            i+=0.1, x+=0.4;
            if (i < 1.5) { 
                pulseLoop(medal);
            } else if (i = 1.5) {
                $(medal).css({'transform' : 'scale(1.2,1.2)'});   
                setTimeout(function () {  
                    $(medal).css({'transform' : 'scale(1,1)'});             
                }, animTime)
            }  
        }, animTime)
    }
    pulseLoop(medal);
};

function queue_audio(audio){
    playQueue.push(medalsPath + 'audio/' + audio);
    playQueue.splice(4, Infinity);
    if(!isPlaying){
        play(playQueue[0]);	
    }
}

var isPlaying = false;
function play(audio){
    isPlaying = true;
    var audioElement = new Audio(audio);
    audioElement.play();
    audioElement.volume = playVol;
    audioElement.onended = function(){
        advanceQueue();
    }
    audioElement.onerror = function(){ 
        advanceQueue();
    }; 
    function advanceQueue(){
        isPlaying = false;
        playQueue.splice(0, 1);
        if(playQueue.length > 0){
            play(playQueue[0]);	
        }
    }
}

var medalNum = 0;
function display_medal(medal){
    dew.show();
    $('#medalBox').css({
        left:leftPos,
        bottom: bottomPos, 
        transform: transform
    });
    var currentMedalNum = medalNum;
    $('<img />', { 
        id: currentMedalNum,
        src: medalsPath + 'images/' + medal,
        css: {
            width: medalWidth
        }
    }).prependTo($('#medalBox'));
    $("#"+currentMedalNum).pulse();
    setTimeout(function(){
        $("#"+currentMedalNum).fadeOut(fadeTime, function() { 
            $("#"+currentMedalNum).remove(); 
            if(!$('#medalBox img').length){
                dew.hide();  
            }
        });
    }, removeTime);
    medalNum++;
}

function doMedal(eventString, audience){
    //console.log(eventString+', '+audience);
    if(eventJSONCache[eventString]){
        switch(audience){
            case 0:
                if(eventJSONCache[eventString].hasOwnProperty('cause_player')){
                    if(eventJSONCache[eventString].cause_player.hasOwnProperty('image')){
                        if(typeof eventJSONCache[eventString].cause_player.image === 'string'){
                            display_medal(eventJSONCache[eventString].cause_player.image);
                        }
                        else{
                            var items = eventJSONCache[eventString].cause_player.image;
                            display_medal(items[Math.floor(Math.random()*items.length)]);
                        }
                    }
                    if(eventJSONCache[eventString].cause_player.hasOwnProperty('sound')){
                        if(typeof eventJSONCache[eventString].cause_player.sound === 'string'){
                            queue_audio(eventJSONCache[eventString].cause_player.sound); 
                        }
                        else{
                            var items = eventJSONCache[eventString].cause_player.sound;
                            queue_audio(items[Math.floor(Math.random()*items.length)]);
                        }
                    }
                }
                break;
            case 1:
                if(eventJSONCache[eventString].hasOwnProperty('cause_team')){
                    if(eventJSONCache[eventString].cause_team.hasOwnProperty('image')){
                        if(typeof eventJSONCache[eventString].cause_team.image === 'string'){
                            display_medal(eventJSONCache[eventString].cause_team.image);
                        }
                        else{
                            var items = eventJSONCache[eventString].cause_team.image;
                            display_medal(items[Math.floor(Math.random()*items.length)]);
                        }
                    }
                    if(eventJSONCache[eventString].cause_team.hasOwnProperty('sound')){
                        if(typeof eventJSONCache[eventString].cause_team.sound === 'string'){
                            queue_audio(eventJSONCache[eventString].cause_team.sound);   
                        }
                        else{
                            var items = eventJSONCache[eventString].cause_team.sound;
                            queue_audio(items[Math.floor(Math.random()*items.length)]);
                        }
                    }
                }
                break;
            case 2:
                if(eventJSONCache[eventString].hasOwnProperty('effect_player')){
                    if(eventJSONCache[eventString].effect_player.hasOwnProperty('image')){
                        if(typeof eventJSONCache[eventString].effect_player.image === 'string'){
                            display_medal(eventJSONCache[eventString].effect_player.image);
                        }
                        else{
                            var items = eventJSONCache[eventString].effect_player.image;
                            display_medal(items[Math.floor(Math.random()*items.length)]);
                        }
                    }
                    if(eventJSONCache[eventString].effect_player.hasOwnProperty('sound')){
                        if(typeof eventJSONCache[eventString].effect_player.sound === 'string'){
                            queue_audio(eventJSONCache[eventString].effect_player.sound);    
                        }
                        else{
                            var items = eventJSONCache[eventString].effect_player.sound;
                            queue_audio(items[Math.floor(Math.random()*items.length)]);
                        }
                    }
                }
                break;
            case 3:
                if(eventJSONCache[eventString].hasOwnProperty('effect_team')){
                    if(eventJSONCache[eventString].effect_team.hasOwnProperty('image')){
                        if(typeof eventJSONCache[eventString].effect_team.image === 'string'){
                            display_medal(eventJSONCache[eventString].effect_team.image);
                        }
                        else{
                            var items = eventJSONCache[eventString].effect_team.image;
                            display_medal(items[Math.floor(Math.random()*items.length)]);
                        }
                    }
                    if(eventJSONCache[eventString].effect_team.hasOwnProperty('sound')){
                        if(typeof eventJSONCache[eventString].effect_team.sound === 'string'){
                            queue_audio(eventJSONCache[eventString].effect_team.sound);  
                        }
                        else{
                            var items = eventJSONCache[eventString].effect_team.sound;
                            queue_audio(items[Math.floor(Math.random()*items.length)]);
                        }
                    }
                }
                break;
            case 4:
                if(eventJSONCache[eventString].hasOwnProperty('image')){
                    if(typeof eventJSONCache[eventString].image === 'string'){
                        display_medal(eventJSONCache[eventString].image);
                    }
                    else{
                        var items = eventJSONCache[eventString].image;
                        display_medal(items[Math.floor(Math.random()*items.length)]);
                    }
                }
                if(eventJSONCache[eventString].hasOwnProperty('sound')){
                    if(typeof eventJSONCache[eventString].sound === 'string'){
                        queue_audio(eventJSONCache[eventString].sound);  
                    }
                    else{
                        var items = eventJSONCache[eventString].sound;
                        queue_audio(items[Math.floor(Math.random()*items.length)]);
                    }
                }
                break;
        }
    }
}

dew.on("variable_update", function(e){
    for(i = 0; i < e.data.length; i++){
        if(e.data[i].name in settingsArray){
            settingsArray[e.data[i].name] = e.data[i].value;
            if(e.data[i].name == "Game.MedalPack"){
                eventJSONCache = null;
            }
        }
    }
});

$(document).ready(function(){
    loadSettings(0);
})

function loadSettings(i){
	if (i != Object.keys(settingsArray).length) {
		dew.command(Object.keys(settingsArray)[i], {}).then(function(response) {
			settingsArray[Object.keys(settingsArray)[i]] = response;
			i++;
			loadSettings(i);
		});
	}
}