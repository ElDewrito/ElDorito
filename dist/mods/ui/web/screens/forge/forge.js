var settingsToLoad = [['fCloneDepth', 'Forge.CloneDepth'], ['fCloneMultiplier', 'Forge.CloneMultiplier'], ['fRotationSnap', 'Forge.RotationSnap'], ['fRotationSens', 'Forge.RotationSensitivity']];

var selectedItem;
var itemNumber = 0;
var tabIndex = 0;

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
    if(hasGP){
        updateSelection(itemNumber);
    }    
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

function updateSelection(item){
    $('.selected').removeClass();
    $("#forgeControls label").eq(item).addClass('selected');
    selectedItem = $('#forgeControls').children().not('label,center,br,.tinySetting,.header,p').eq(itemNumber).attr('id');
}

function upNav(){
    if(itemNumber > 0){
        itemNumber--;
        updateSelection(itemNumber);
    }
}

function downNav(){
    if(itemNumber < $('#forgeControls label').length-1){
        itemNumber++;
        updateSelection(itemNumber);
    }           
}

function leftToggle(){
    if($('#'+selectedItem).getInputType() == "select"){
        var elementIndex = $('#'+selectedItem+' option:selected').index();
        if(elementIndex > 0){
            var newElement = elementIndex - 1;
            $('#'+selectedItem+' option').eq(newElement).prop('selected', true);
            if(selectedItem == 'presetMenu'){
                applyBindString($('#'+selectedItem).val());
            } else {
                updateSetting($('#'+selectedItem).attr('name'), $('#'+selectedItem).val());
            }
        }
    }
    if($('#'+selectedItem).getInputType() == "range"){
        document.getElementById(selectedItem).stepDown();
        document.querySelector('#'+selectedItem +'Text').value = document.getElementById(selectedItem).value;
        updateSetting($('#'+selectedItem).attr('name'), $('#'+selectedItem).val());
    }        
}

function rightToggle(){
    if($('#'+selectedItem).getInputType() == "select"){
        var elementIndex = $('#'+selectedItem+' option:selected').index();
        var elementLength = $('#'+selectedItem).children('option').length;
        if(elementIndex < elementLength){
            var newElement = elementIndex + 1;
            $('#'+selectedItem+' option').eq(newElement).prop('selected', true);
            if(selectedItem == 'presetMenu'){
                applyBindString($('#'+selectedItem).val());
            } else {
                updateSetting($('#'+selectedItem).attr('name'), $('#'+selectedItem).val());
            }
        }
    }
    if($('#'+selectedItem).getInputType() == "range"){
        document.getElementById(selectedItem).stepUp();
        document.querySelector('#'+selectedItem +'Text').value = document.getElementById(selectedItem).value;
        updateSetting($('#'+selectedItem).attr('name'), $('#'+selectedItem).val());
    }        
}

$.fn.getInputType = function () {
    return this[0].tagName.toString().toLowerCase() === "input" ?
        $(this[0]).attr("type").toLowerCase() : this[0].tagName.toLowerCase();
};

function onControllerConnect(){
    updateSelection(itemNumber); 
    $('button img').show();
}

function onControllerDisconnect(){
    $('.selected').removeClass(); 
    $('button img').hide();    
}

function buttonAction(i){
    switch (i) {
        case 1: // B
            $('#playerBreakdown').hide();
            dew.hide();
            break;
        case 2: // X
            dew.command('Forge.Canvas');
            break;
        case 3: // Y
            dew.command('Forge.DeleteAll');
            break;
        case 12: // Up
            upNav();
            break;
        case 13: // Down
            downNav();
            break;
        case 14: // Left
            leftToggle();
            break;
        case 15: // Right
            rightToggle();
            break;
        case 4: // LB
            prevPage();
            break;
        case 5: // RB
            nextPage();
            break;
        default:
            //console.log("nothing associated with " + i);
    }  
}

function stickAction(direction, x){
    if(x<2){//left stick
        if(x==0 && direction=="+"){//LS Right
            rightToggle();
        }else if(x==0 && direction=="-"){//LS Left
            leftToggle();
        }else if(x==1 && direction=="+"){//LS Down
            downNav();
        }else if(x==1 && direction=="-"){//LS Up
            upNav();
        }
    }
};