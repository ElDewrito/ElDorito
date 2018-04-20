var settingsArray = { 'Settings.Gamepad': '0' , 'Game.IconSet': '360', 'Game.SkipTitleSplash':'0'};
var announcementShown = false;

dew.on("variable_update", function(e){
    for(i = 0; i < e.data.length; i++){
        if(e.data[i].name in settingsArray){
            settingsArray[e.data[i].name] = e.data[i].value;
        }
    }
});


function loadSettings(i){
	if (i != Object.keys(settingsArray).length) {
		dew.command(Object.keys(settingsArray)[i], {}).then(function(response) {
			settingsArray[Object.keys(settingsArray)[i]] = response;
			i++;
			loadSettings(i);
		});
	}
}

$(document).ready(function(){
        loadSettings(0);
});

$("html").on("keydown", function(e) {
    if (e.which == 13){
        if(!announcementShown)
            hideScreen();
		else
			closeAnnounce();
    } 
    if(e.keyCode == 192 || e.keyCode == 112 || e.keyCode == 223){
        dew.show("console");
    }
});

dew.on("show", function(e){
    if(settingsArray['Game.SkipTitleSplash'] == 1){
        dew.captureInput(false);
        $(".genericLoader").hide();
        $("#blackLayer").fadeOut(2000, function() {
            dew.hide();
        });
    }else{
        $("#blackLayer").hide();
        $.getJSON( "https://raw.githubusercontent.com/ElDewrito/ElDorito/master/currentRelease.json", function(data) {
            dew.getVersion().then(function (version) {
                if(parseVersion(data.release[0].version) != parseVersion(version)) {
                    dew.show('alert',{"title":"Update Available!", "body":"There is a newer version of ElDewrito available.|r|n|r|nWould you like to launch the updater?", "type":"update"});
                }
            });
        }); 
        dew.command('Game.FirstRun', {}).then(function(result){
            if(result == 1){
                $.getJSON("http://scooterpsu.github.io/announcements.json", function(data) {
                    if(data.announcements.length){
                        for(var i = 0; i < data.announcements.length; i++){
                            $('#announcementBox').append(
                                $('<div>',{
                                    class: 'announcement',
                                    "css" : {
                                        "background-image" : "url('"+data.announcements[i].background+"')"
                                    },
                                }).append($('<p>',{
                                    class: 'announceTitle',
                                    text: data.announcements[i].title
                                }).append($('<p>',{
                                    class: 'announcesubTitle',
                                    text: data.announcements[i].subtitle
                                })).append($('<p>',{
                                    class: 'announceContent',
                                    html: data.announcements[i].content
                                }))));
                        }
                        announcementShown = true;
                        $('#announcementBox').show();
                        $('#announcementBG').show();
                    }
                });
             }
        });
       if(settingsArray['Settings.Gamepad'] == 1){
            $('#dpad').attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_Dpad.png');
            $("#dpad").show();
            $( "#up, #down, #left, #right" ).hide();
            $(".instructionText img").attr("src","dew://assets/buttons/"+settingsArray['Game.IconSet']+"_Start.png");
            $("#esc").attr("src","dew://assets/buttons/"+settingsArray['Game.IconSet']+"_B.png");
            $("#enter").attr("src","dew://assets/buttons/"+settingsArray['Game.IconSet']+"_A.png");
        }else{
            $("#dpad").hide();
			$("#closeAnnounceButton").attr("src","dew://assets/buttons/Keyboard_White_Enter.png");
            $( "#up, #down, #left, #right" ).show();
            $(".instructionText img").attr("src","dew://assets/buttons/Keyboard_White_Enter.png");
            $("#esc").attr("src","dew://assets/buttons/Keyboard_White_Esc.png");
            $("#enter").attr("src","dew://assets/buttons/Keyboard_White_Enter.png");
        }
    }
});

dew.on('serverconnect', function(e){
    hideScreen();
});

dew.on('controllerinput', function (e) {
    if (!settingsArray['Settings.Gamepad'])
        return;

    if (announcementShown) {
        if (e.data.A == 1) {
            closeAnnounce();
        }
    } else {
        if (e.data.Start == 1) {
            hideScreen();
        }
    }
});

function hideScreen(){
    $( "body" ).fadeOut( 500, function() {
        dew.hide();
    });
	dew.command('Game.PlaySound 0x0B00');
}

function closeAnnounce(){
    $('.announcement').last().remove();
    if(!$('.announcement:visible').length){
        $('#announcementBox').hide();
        $('#announcementBG').hide();
        announcementShown = false;
        dew.command('Game.FirstRun 0', {}).then(function(){
            dew.command('writeconfig');
        });
    }
	dew.command('Game.PlaySound 0x0B00');
}

function parseVersion(str) { 
    var result = 0;
    var suffixPos = str.indexOf('-');
    if(suffixPos != -1)
        str = str.substr(0, suffixPos);
    
    var parts = str.split('.');
    for(var i = 0; i < parts.length && i < 4; i++) {
        result |= (parseInt(parts[i]) << (24-(i*8)));
    }  
    return result;
}