var settingsArray = { 'Settings.Gamepad': '0' , 'Game.IconSet': '360', 'Game.SkipTitleSplash':'0'};

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
        hideScreen();
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
                $('#announcementBox').show();
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

dew.on('controllerinput', function(e){       
    if(settingsArray['Settings.Gamepad'] == 1){
        if(e.data.Start == 1){
            hideScreen();   
        }
    }
});

function hideScreen(){
    $( "body" ).fadeOut( 500, function() {
        dew.hide();
    });
}

function closeAnnounce(){
    $('#announcementBox').hide();
    dew.command('Game.FirstRun 0', {}).then(function(){
        dew.command('writeconfig');
    });
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