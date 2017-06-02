var fadeTime = 800;
var animTime = 18;
var removeTime = 4500;
var medalsPath = 'medals://';
var playQueue = [];
var eventJson;
var announcerVolume;
var suppressRepeat = false;
var medalWidth = '3.5vw';
var leftPos = '6.78vw';
var bottomPos = '32vh';
var transform = 'skew(0,-2.75deg)'; 
var juggleEvent = 0;
var juggleDelay = 3000;

dew.on("mpevent", function (event) {
    dew.command('Game.MedalPack', {}).then(function(response) {
        medalsPath = "medals://" + response + "/";
        $.getJSON(medalsPath+'events.json', function(json) {
            eventJson = json;
            if(eventJson['settings']){
                if(eventJson['settings'].hasOwnProperty('medalWidth')){
                    medalWidth = eventJson['settings'].medalWidth;
                }
                if(eventJson['settings'].hasOwnProperty('leftPos')){            
                    leftPos = eventJson['settings'].leftPos;
                }
                if(eventJson['settings'].hasOwnProperty('bottomPos')){
                    bottomPos = eventJson['settings'].bottomPos;
                }   
                if(eventJson['settings'].hasOwnProperty('transform')){
                    transform = eventJson['settings'].transform;
                } 
            }    
            dew.command('Game.SuppressJuggling', {}).then(function(response) {
                if(response == 1){
                    suppressRepeat = true;
                } else {
                    suppressRepeat = false;
                }
            });
            dew.command('Game.AnnouncerVolume', {}).then(function(response) {
                announcerVolume = response/100;
                var medal = event.data.name;
                if(suppressRepeat && ( medal.startsWith('ctf_event_flag_') || medal.startsWith('assault_event_bomb_'))){
                    juggleEvent++;
                    setTimeout(function(){
                        if(juggleEvent > 0){ juggleEvent--; }
                    }, juggleDelay);
                } 
                if(juggleEvent > 2 && ((medal.startsWith('ctf_event_flag_') && medal != 'ctf_event_flag_captured')||(medal.startsWith('assault_event_bomb_') && medal != 'assault_event_bomb_placed_on_enemy_post'))){
                    return
                }
                doMedal(event.data.name, event.data.audience);
            });            
        });
    });
});

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
    if(!isPlaying){
        play(playQueue[0]);	
    }
}

var isPlaying = false;
function play(audio){
    isPlaying = true;
    var audioElement = new Audio(audio);
    audioElement.play();
    audioElement.volume = announcerVolume;
    audioElement.onended = function(){
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
    if(eventJson[eventString]){
        switch(audience){
            case 0:
                if(eventJson[eventString].hasOwnProperty('cause_player')){
                    if(eventJson[eventString].cause_player.hasOwnProperty('image')){
                        display_medal(eventJson[eventString].cause_player.image);
                    }
                    if(eventJson[eventString].cause_player.hasOwnProperty('sound')){
                        queue_audio(eventJson[eventString].cause_player.sound);		
                    }
                }
                break;
            case 1:
                if(eventJson[eventString].hasOwnProperty('cause_team')){
                    if(eventJson[eventString].cause_team.hasOwnProperty('image')){
                        display_medal(eventJson[eventString].cause_team.image);
                    }
                    if(eventJson[eventString].cause_team.hasOwnProperty('sound')){
                        queue_audio(eventJson[eventString].cause_team.sound);		
                    }
                }
                break;
            case 2:
                if(eventJson[eventString].hasOwnProperty('effect_player')){
                    if(eventJson[eventString].effect_player.hasOwnProperty('image')){
                        display_medal(eventJson[eventString].effect_player.image);
                    }
                    if(eventJson[eventString].effect_player.hasOwnProperty('sound')){
                        queue_audio(eventJson[eventString].effect_player.sound);		
                    }	
                }
                break;
            case 3:
                if(eventJson[eventString].hasOwnProperty('effect_team')){
                    if(eventJson[eventString].effect_team.hasOwnProperty('image')){
                        display_medal(eventJson[eventString].effect_team.image);
                    }
                    if(eventJson[eventString].effect_team.hasOwnProperty('sound')){
                        queue_audio(eventJson[eventString].effect_team.sound);		
                    }	
                }
                break;
            case 4:
                if(eventJson[eventString].hasOwnProperty('image')){
                    display_medal(eventJson[eventString].image);
                }
                if(eventJson[eventString].hasOwnProperty('sound')){
                    queue_audio(eventJson[eventString].sound);		
                }
                break;
        }
    }
}