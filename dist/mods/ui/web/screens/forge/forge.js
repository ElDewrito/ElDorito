var settingsToLoad = [['fCloneDepth', 'Forge.CloneDepth'], ['fCloneMultiplier', 'Forge.CloneMultiplier'], ['fRotationSnap', 'Forge.RotationSnap'], ['fRotationSens', 'Forge.RotationSensitivity']];

$(document).keyup(function (e) {
    if (e.keyCode === 27) {
        $('#playerBreakdown').hide();
        dew.hide();
    }
});
$(document).keydown(function (e) {
    if(e.keyCode == 84 || e.keyCode == 89){
        var teamChat = false;
        if(e.keyCode == 89){ teamChat = true };
        dew.show("chat", {'captureInput': true, 'teamChat': teamChat});
    }
    if(e.keyCode == 192 || e.keyCode == 112){
        dew.show("console");
    }
});

function closeBrowser() {
    dew.hide();
}

$(document).ready(function() {
    loadSettings(0);     
    $('#forgeControls input').on('change', function(){
        updateSetting(this.name, this.value);
    });    
});

function loadSettings(i) {
	if (i != settingsToLoad.length) {
		dew.command(settingsToLoad[i][1], {}).then(function(response) {
            response = parseFloat(response);
            if ($("input[name='"+settingsToLoad[i][0]+"']").is(':checkbox')){
                if (response == "1"){
                    $("input[name='"+settingsToLoad[i][0]+"']").prop('checked', true);
                }                
            }else{
                $("input[name='"+settingsToLoad[i][0]+"']").val(response);
            }
            $("select[name='"+settingsToLoad[i][0]+"']").val(response);
            $("textarea[name='"+settingsToLoad[i][0]+"']").val(response);
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
    dew.command(settingsToLoad[arrayInArray(setting, settingsToLoad)][1] + " \"" + value + "\"", {}).then(function(response){
        //dew.command("writeconfig");
    });
}

function arrayInArray(needle, haystack) {
    for(i=0; i<haystack.length; i++) {
        if (haystack[i].indexOf(needle) > -1){
            return i
        }
    }
}