var settingsToLoad = [
    ['wOffsetConfig','Weapon.Config']
];

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
    loadSettings(0);     
    if(hasGP){
        updateSelection(itemNumber);
    }   
    $('input[type=range]').on('input', function(){
        var newID = $(this).attr('id') + 'Text';
        $('#'+newID).val($(this).val() * -1);

    });
    $('.tinySetting').on('change', function(){
        var newID = $(this).attr('id');
        if(newID.endsWith('Text')){
            newID = newID.slice(0, -4);
        }
        $('#'+newID).val($(this).val() * -1);
        $('#'+newID).trigger('change');
    });
    $('#loadButton').on('click', function(){
        dew.command('Weapon.Config '+ $('#wOffsetConfig').val(), {}).then(function(){ 
            dew.command('Weapon.Offset ' + $('#weapList').val(), {}).then(function(response){
                var offsets = response.split($('#weapList').val() + ', I: ')[1];
                $('#weaponI').val(offsets.split(',')[0] * -1);
                $('#weaponIText').val(offsets.split(',')[0]);
                $('#weaponJ').val(offsets.split(', J: ')[1].split(', K: ')[0] * -1);
                $('#weaponJText').val(offsets.split(', J: ')[1].split(', K: ')[0]);
                $('#weaponK').val(offsets.split(', K: ')[1] * -1);
                $('#weaponKText').val(offsets.split(', K: ')[1]);
            });  
        });
    });
    $('#saveButton').on('click', function(){
        dew.command('Weapon.Config.Save');
    });
    $('#resetButton').on('click', function(){
        dew.command('Weapon.Offset.Reset '+$('#weapList').val(), {}).then(function(){ 
            dew.command('Weapon.Offset ' + $('#weapList').val(), {}).then(function(response){
                var offsets = response.split($('#weapList').val() + ', I: ')[1];
                $('#weaponI').val(offsets.split(',')[0] * -1);
                $('#weaponIText').val(offsets.split(',')[0]);
                $('#weaponJ').val(offsets.split(', J: ')[1].split(', K: ')[0] * -1);
                $('#weaponJText').val(offsets.split(', J: ')[1].split(', K: ')[0]);
                $('#weaponK').val(offsets.split(', K: ')[1] * -1);
                $('#weaponKText').val(offsets.split(', K: ')[1]);
            });  
        });
    });
    dew.command('Weapon.List', {}).then(function(response){
        var weaponList = response.split('\n');
        var weaponArray = [];
        for(i = 0; i < weaponList.length; i++){
            var weapName = weaponList[i].split('Name: ')[1]
            weaponArray.push([weapName,weapName]);
        }
        setOptionList('weapList', weaponArray);
    });
    $('#weapList').on('change', function(){
        dew.command('Weapon.Offset ' + $('#weapList').val(), {}).then(function(response){
            var offsets = response.split($('#weapList').val() + ', I: ')[1];
            $('#weaponI').val(offsets.split(',')[0] * -1);
            $('#weaponIText').val(offsets.split(',')[0]);
            $('#weaponJ').val(offsets.split(', J: ')[1].split(', K: ')[0] * -1);
            $('#weaponJText').val(offsets.split(', J: ')[1].split(', K: ')[0]);
            $('#weaponK').val(offsets.split(', K: ')[1] * -1);
            $('#weaponKText').val(offsets.split(', K: ')[1]);
        });       
    });
    $('#weaponI, #weaponJ, #weaponK').on('input change', function(){
        dew.command('Weapon.Offset ' + $('#weapList').val() + ' ' +$('#weaponIText').val() + ' ' +$('#weaponJText').val() + ' ' +$('#weaponKText').val());
    });
});

function loadSettings(i) {
	if (i != settingsToLoad.length) {
		dew.command(settingsToLoad[i][1], {}).then(function(response) {
            if ($("#"+settingsToLoad[i][0]).is(':checkbox')){
                if (response == "1"){
                    $("#"+settingsToLoad[i][0]).prop('checked', true);
                }                
            }else{
                $("#"+settingsToLoad[i][0]).val(response);
            }
            if($('#'+settingsToLoad[i][0]).hasClass('hasTiny')){
                $('#'+settingsToLoad[i][0]+'Text').val(parseFloat(response));
            }
			i++;
			loadSettings(i);
		});
	} else {
		loadedSettings = true;
	}
}

function updateSetting(setting, value){
    if ($("#"+setting).is(':checkbox')){
        if ($("#"+setting).is(':checked')){
            value = "1";
        } else {
            value = "0";
        }
    }
    $.grep(settingsToLoad, function(result, index){
        if(result[0] == setting){
            dew.command(result[1] + " \"" + value + "\"");
        }
    });
}

function updateSelection(item){
    $('.selected').removeClass();
    $("#weaponOffsets label").eq(item).addClass('selected');
    selectedItem = $('#weaponOffsets').children().not('label,center,br,.tinySetting,.header,p').eq(itemNumber).attr('id');
}

function upNav(){
    if(itemNumber > 0){
        itemNumber--;
        updateSelection(itemNumber);
    }
}

function downNav(){
    if(itemNumber < $('#weaponOffsets label').length-1){
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
    dew.command('Game.IconSet', {}).then(function(controllerType){
        $("#buttonBar img").eq(0).attr("src","dew://assets/buttons/"+controllerType+"_Y.png");
        $("#buttonBar img").eq(1).attr("src","dew://assets/buttons/"+controllerType+"_X.png");
        $("#buttonBar img").eq(2).attr("src","dew://assets/buttons/"+controllerType+"_A.png");
        $('#buttonBar img').show();
    });
}

function onControllerDisconnect(){
    $('.selected').removeClass(); 
    $('button img').hide();    
}

dew.on("show", function(e){
    dew.command('Weapon.List', {}).then(function(response){
        var weaponList = response.split('\n');
        var weaponArray = [];
        for(i = 0; i < weaponList.length; i++){
            var weapName = weaponList[i].split('Name: ')[1]
            weaponArray.push([weapName,weapName]);
        }
        setOptionList('weapList', weaponArray);
    });
    dew.command('Settings.Gamepad', {}).then(function(result){
        if(result == 1){
            onControllerConnect();
            hasGP = true;
        }else{
            onControllerDisconnect();
            hasGP = false;
        }
    });
    dew.command('Weapon.JSON', {}).then(function(response){
        var weaponInfo = JSON.parse(response);
        $('#weapList').val(weaponInfo.Name);
        $('#weaponI').val(weaponInfo.FirstPersonWeaponOffset[0] * -1);
        $('#weaponIText').val(weaponInfo.FirstPersonWeaponOffset[0]);
        $('#weaponJ').val(weaponInfo.FirstPersonWeaponOffset[1] * -1);
        $('#weaponJText').val(weaponInfo.FirstPersonWeaponOffset[1]);
        $('#weaponK').val(weaponInfo.FirstPersonWeaponOffset[2] * -1);
        $('#weaponKText').val(weaponInfo.FirstPersonWeaponOffset[2]);
    });
});

dew.on('controllerinput', function(e){       
    if(hasGP){
        if(e.data.Y == 1){
            $('#resetButton').click();
        }
        if(e.data.B == 1){
            dew.hide();  
        }
        if(e.data.X == 1){
            $('#loadButton').click(); 
        }
        if(e.data.A == 1){
            $('#saveButton').click();
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

function setOptionList(ElementID, ArrayVar){
    var sel = document.getElementById(ElementID);
    for(var i = 0; i < ArrayVar.length; i++){
        var opt = document.createElement('option');
        opt.innerHTML = ArrayVar[i][0];
        opt.value = ArrayVar[i][1];
        sel.appendChild(opt);
    }
}
