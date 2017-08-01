var selectedItem;
var itemNumber = 0;
var tabIndex = 0;
var hasGP = false;

$(document).keyup(function (e) {
    if (e.keyCode === 27) {
        dew.hide();
    }
});
$(document).keydown(function (e) {
    if(e.keyCode == 192 || e.keyCode == 112 || e.keyCode == 223){
        dew.show("console");
    }
});

function closeBrowser() {
    dew.hide();
}

$(document).ready(function() {
    if(hasGP){
        updateSelection(itemNumber);
    }   
    $('#xSens, #xSensText').on('change input', function(e){
        dew.command('Input.ControllerSensitivityX '+$(this).val());
        $('#xSens, #xSensText').val($(this).val());
    });
    $('#ySens, #ySensText').on('change input', function(e){
        dew.command('Input.ControllerSensitivityY '+$(this).val());
        $('#ySens, #ySensText').val($(this).val());
    });
});

function updateSelection(item){
    $('.selected').removeClass();
    $("#controllerSensitivity label").eq(item).addClass('selected');
    selectedItem = $('#controllerSensitivity').children().not('label,center,br,.tinySetting,.header,p').eq(itemNumber).attr('id');
}

function upNav(){
    if(itemNumber > 0){
        itemNumber--;
        updateSelection(itemNumber);
    }
}

function downNav(){
    if(itemNumber < $('#controllerSensitivity label').length-1){
        itemNumber++;
        updateSelection(itemNumber);
    }           
}

function leftToggle(){
    if(document.getElementById(selectedItem).computedRole == "combobox"){
        var elementIndex = $('#'+selectedItem+' option:selected').index();
        if(elementIndex > 0){
            var newElement = elementIndex - 1;
            $('#'+selectedItem+' option').eq(newElement).prop('selected', true);
        }
    }
    if(document.getElementById(selectedItem).computedRole == "slider"){
        document.getElementById(selectedItem).stepDown();
        document.querySelector('#'+selectedItem +'Text').value = document.getElementById(selectedItem).value * -1;
        updateSetting($('#'+selectedItem).attr('name'), $('#'+selectedItem).val());
    }   
    if(document.getElementById(selectedItem).computedRole == "combobox" || document.getElementById(selectedItem).computedRole == "slider"){
        $('#'+selectedItem).trigger('change');
        dew.command('Game.PlaySound 0x0B00');  
    }    
}

function rightToggle(){
    if(document.getElementById(selectedItem).computedRole == "combobox"){
        var elementIndex = $('#'+selectedItem+' option:selected').index();
        var elementLength = $('#'+selectedItem).children('option').length;
        if(elementIndex < elementLength){
            var newElement = elementIndex + 1;
            $('#'+selectedItem+' option').eq(newElement).prop('selected', true);

        }
    }
    if(document.getElementById(selectedItem).computedRole == "slider"){
        document.getElementById(selectedItem).stepUp();
        document.querySelector('#'+selectedItem +'Text').value = document.getElementById(selectedItem).value * -1;
        updateSetting($('#'+selectedItem).attr('name'), $('#'+selectedItem).val());
    }        
    if(document.getElementById(selectedItem).computedRole == "combobox" || document.getElementById(selectedItem).computedRole == "slider"){
        $('#'+selectedItem).trigger('change');
        dew.command('Game.PlaySound 0x0B00');  
    }
}

function onControllerConnect(){
    updateSelection(itemNumber); 
}

function onControllerDisconnect(){
    $('.selected').removeClass();     
}

dew.on("show", function(e){
    dew.command('Settings.Gamepad', {}).then(function(result){
        if(result == 1){
            onControllerConnect();
            hasGP = true;
        }else{
            onControllerDisconnect();
            hasGP = false;
        }
    });
    dew.command('Input.ControllerSensitivityX', {}).then(function(result){
        $('#xSens, #xSensText').val(parseFloat(result));
    });
    dew.command('Input.ControllerSensitivityY', {}).then(function(result){
        $('#ySens, #ySensText').val(parseFloat(result));
    });
});

dew.on('controllerinput', function(e){       
    if(hasGP){
        if(e.data.B == 1){
            dew.hide();  
        }
        if(e.data.Up == 1){
            upNav();  
        }
        if(e.data.Down == 1){
            downNav();
        }
        if(e.data.Left == 1){
            leftToggle()
        }
        if(e.data.Right == 1){
            rightToggle(); 
        }
    }
});