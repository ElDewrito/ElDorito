var fadeTime = 300;
var animTime = 18;
var removeTime = 3500;
var medalsPath = 'medals://';
var playQueue = [];
var eventJson;

$(document).ready(function() {
    dew.command('Game.MedalPack', {}, function(response) {
        medalsPath = medalsPath + response + "/";
        $.getJSON(medalsPath+'events.json', function(json) {
            eventJson = json;
        });
    });
});

dew.on("mpevent", function (event) {
    doMedal(event.data.name, event.data.audience);
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
    audioElement.volume = 0.5;
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
	var currentMedalNum = medalNum;
	$('#medalBox').prepend('<img id="'+ medalNum + '" src="' + medalsPath + 'images/' + medal + '">');
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
    if(eventJson[eventString]){
        if(eventJson[eventString].hasOwnProperty('image')){
            display_medal(eventJson[eventString].image);
        }
        if(eventJson[eventString].hasOwnProperty('sound')){
            queue_audio(eventJson[eventString].sound);		
        }
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
        }
    }
}