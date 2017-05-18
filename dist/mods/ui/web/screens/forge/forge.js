var settingsToLoad = [['fCloneDepth', 'Forge.CloneDepth'], ['fCloneMultiplier', 'Forge.CloneMultiplier']];

$("html").on("keydown", function(e) {
    if (e.which == 113){
        dew.hide();
    }
    if(e.which == 84 || e.which == 89){
        var teamChat = false;
        if(e.which == 89){ teamChat = true };
        dew.show("chat", {'captureInput': true, 'teamChat': teamChat});
    }
    if(e.which == 192 || e.which == 112){
        dew.show("console");
    }
});

function closeBrowser() {
    dew.hide();
}

$(document).ready(function() {
        loadSettings(0);        
});

function loadSettings(i) {
	if (i != settingsToLoad.length) {
		dew.command(settingsToLoad[i][1], {}).then(function(response) {
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