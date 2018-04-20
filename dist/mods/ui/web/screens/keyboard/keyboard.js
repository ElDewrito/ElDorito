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

dew.on("show", function (event) {
    $("#title").text(event.data.title);
    $("#description").text(event.data.description);
    $("#text").val(event.data.defaultValue);
    $(".dialog").show();
    $("#text").focus();
    
    if(event.data.title == "Edit Description"){
        $('#text').attr({'maxlength':'127','rows':'10'});
    }else{
        $('#text').attr({'maxlength':'15','rows':'1'});
    };
    
    if(settingsArray['Settings.Gamepad'] == 1){
        $('#ok .button').attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_A.png');
        $('#cancel .button').attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_B.png');
        $('.button').show();   
    }else{
        $('.button').hide();   
    }
});

dew.on("hide", function (event) {
    $(".dialog").hide();
});

$(document).ready(function(){
    $(".dialog").hide();

    $("html").on("submit", "form", function (event) {
        event.preventDefault();
        var value = $("#text").val().trim();
        if (value === "") {
            return;
        }
        dew.submitVirtualKeyboard(value).then(() => dew.hide());
    });

    $("html").on("keydown", function (event) {
        if (event.keyCode === 13 /* Enter */) {
			dew.command('Game.PlaySound 0xb00');
            event.preventDefault();
            $("form").submit();
        } else if (event.keyCode === 27 /* Escape */) {
			dew.command('Game.PlaySound 0xb01');
            dew.cancelVirtualKeyboard().then(() => dew.hide());
        }
    });
    
    $("#ok").off("click").on("click", function (){
		dew.command('Game.PlaySound 0xb00');
        $("form").submit();
    });
    
    $("#cancel").off("click").on("click", function (){
		dew.command('Game.PlaySound 0xb01');
        dew.cancelVirtualKeyboard().then(() => dew.hide());
    });
    
    loadSettings(0);
});

dew.on('controllerinput', function(e){    
    if(e.data.A == 1){
		dew.command('Game.PlaySound 0xb00');
        $("form").submit();
    }
    if(e.data.B == 1){
		dew.command('Game.PlaySound 0xb01');
        dew.cancelVirtualKeyboard().then(() => dew.hide());
    }
    if(e.data.Start == 1){
		dew.command('Game.PlaySound 0xb00');
        $("form").submit();
    }
});