var hasGP = false;

$(document).ready(function() {
    $('#exit').on('click', function(e){
        dew.command("game.exit");
    });
    
    $('#cancel').on('click', function(e){
        dew.hide();
    });
    
    $(document).keydown(function(e) {
            if (e.keyCode === 13 || e.keyCode === 32 || e.keyCode === 89) //Enter, Space, Y
                    dew.command("game.exit");
    });
    $(document).keyup(function(e) {
        if (e.keyCode === 27 || e.keyCode === 66 || e.keyCode === 78) //Escape, B, N
            dew.hide();
    });
});

function onControllerConnect() {
    dew.command('Game.IconSet', {}).then(function(response) {
        $("#exit").append("<img class='button' src='dew://assets/buttons/" + response + "_Y.png'>");
        $("#cancel").append("<img class='button' src='dew://assets/buttons/" + response + "_B.png'>");
    });
};

dew.on("show", function(event) {
    dew.command('Settings.Gamepad', {}).then(function(result) {
        if (result == 1) {
            onControllerConnect();
            hasGP = true;
        } else { 
            hasGP = false; 
        }
    });
});

dew.on('controllerinput', function(e) {
    if (hasGP) {
        if (e.data.Y == 1) { dew.command('game.exit')};
		if (e.data.B == 1) { dew.hide(); };
    }
});