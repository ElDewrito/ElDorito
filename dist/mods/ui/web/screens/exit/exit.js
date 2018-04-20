var settingsArray = { 'Settings.Gamepad': '0' , 'Game.IconSet': '360'};

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

$(document).ready(function() {
    $('#exit').off('click').on('click', function(e){
        dew.command("game.exit");
    });
    
    $('#cancel').off('click').on('click', function(e){
        dew.command('Game.PlaySound 0xb01');
        dew.hide();
    });
    
    $(document).keydown(function(e) {
        if (e.keyCode === 13 || e.keyCode === 32 || e.keyCode === 89) //Enter, Space, Y
            dew.command("game.exit");
    });
    $(document).keyup(function(e) {
        if (e.keyCode === 27 || e.keyCode === 66 || e.keyCode === 78){ //Escape, B, N
            dew.command('Game.PlaySound 0xb01');
            dew.hide();
		}
    });
    loadSettings(0);
});

dew.on("show", function(event) {
    if(settingsArray['Settings.Gamepad'] == 1){
        $('#exit .button').attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_Y.png');
        $('#cancel .button').attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_B.png');
        $('.button').show();   
    }else{
        $('.button').hide();   
    }
});

dew.on('controllerinput', function(e) {
    if(settingsArray['Settings.Gamepad'] == 1){
        if (e.data.Y == 1) { dew.command('game.exit')};
		if (e.data.B == 1) { dew.hide(); };
    }
});