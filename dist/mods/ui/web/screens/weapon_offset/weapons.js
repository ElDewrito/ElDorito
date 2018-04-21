var selectedItem;
var itemNumber = 0;
var tabIndex = 0;
var hasGP = false;
var axisThreshold = .5;
var stickTicks = { left: 0, right: 0, up: 0, down: 0 };
var repGP;
var lastHeldUpdated = 0;

$(document).keyup(function (e) {
    if (e.keyCode === 27) {
        closeWindow();
    }
});
$(document).keydown(function (e) {
    if(e.keyCode == 192 || e.keyCode == 112 || e.keyCode == 223){
        dew.show("console");
    }
});

function closeWindow() {
    dew.hide();
}

$(document).ready(function() {
    initGamepad();   
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
    $('#loadButton').off('click').on('click', function(){
        dew.command('Weapon.JSON.Load '+ $('#wOffsetConfig').val(), {}).then(function(){ 
            displayOffset($('#weapList').val());
        });
    });
    $('#saveButton').off('click').on('click', function(){
        dew.command('Weapon.JSON.Save');
    });
    $('#resetButton').off('click').on('click', function(){
        dew.command('Weapon.Offset.Reset '+$('#weapList').val(), {}).then(function(){ 
            resetEquipped();
        });
    });
    $('#closeButton').off('click').on('click', function(){
         closeWindow();
    });
    $('#wOffsetConfig').off('change').on('change', function(){
        dew.command('Weapon.JSON.file ' + $('#wOffsetConfig').val());
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
    $('#weapList').off('change').on('change', function(){
        displayOffset($('#weapList').val());
    });
    $('#weaponI, #weaponJ, #weaponK').on('input change', function(){
        dew.command('Weapon.Offset ' + $('#weapList').val() + ' ' +$('#weaponIText').val() + ' ' +$('#weaponJText').val() + ' ' +$('#weaponKText').val());
    });
});

function displayOffset(weapon){
    dew.command('Weapon.Offset ' + weapon, {}).then(function(response){
        var offsets = response.split($('#weapList').val() + ', I: ')[1];
        $('#weaponI').val(offsets.split(',')[0] * -1);
        $('#weaponIText').val(parseFloat(offsets.split(',')[0]).toFixed(3));
        $('#weaponJ').val(offsets.split(', J: ')[1].split(', K: ')[0] * -1);
        $('#weaponJText').val(parseFloat(offsets.split(', J: ')[1].split(', K: ')[0]).toFixed(3));
        $('#weaponK').val(offsets.split(', K: ')[1] * -1);
        $('#weaponKText').val(parseFloat(offsets.split(', K: ')[1]).toFixed(3));
    });   
}

function resetEquipped(){
    dew.command('Weapon.Equipped json', {}).then(function(response){
        var weaponInfo = JSON.parse(response);
        $('#weapList').val(weaponInfo.Name);
        $('#weaponI').val(weaponInfo.FirstPersonWeaponOffset[0] * -1);
        $('#weaponIText').val(parseFloat(weaponInfo.FirstPersonWeaponOffset[0]).toFixed(3));
        $('#weaponJ').val(weaponInfo.FirstPersonWeaponOffset[1] * -1);
        $('#weaponJText').val(parseFloat(weaponInfo.FirstPersonWeaponOffset[1]).toFixed(3));
        $('#weaponK').val(weaponInfo.FirstPersonWeaponOffset[2] * -1);
        $('#weaponKText').val(parseFloat(weaponInfo.FirstPersonWeaponOffset[2]).toFixed(3));
    });
}

function initGamepad(){
    dew.command('Settings.Gamepad', {}).then(function(result){
        if(result == 1){
            hasGP = true;
            if(!repGP){
                repGP = window.setInterval(checkGamepad,1000/60);
            }
            onControllerConnect();
        }else{
            onControllerDisconnect();
            hasGP = false;
            if(repGP){
                window.clearInterval(repGP);
                repGP = null;
            }
        }
    });
}

function checkGamepad(){
    var shouldUpdateHeld = false;
    if(Date.now() - lastHeldUpdated > 100) {
        shouldUpdateHeld = true;
        lastHeldUpdated = Date.now();
    }
    if(stickTicks.up == 1 || (shouldUpdateHeld && stickTicks.up > 25)){
        upNav();
    }
    if(stickTicks.down == 1 || (shouldUpdateHeld && stickTicks.down > 25)){
        downNav();
    }
    if(stickTicks.left == 1 || (shouldUpdateHeld && stickTicks.left > 25)){
        leftToggle();
    }
    if(stickTicks.right == 1 || (shouldUpdateHeld && stickTicks.right > 25)){
        rightToggle();
    }
};

function updateSelection(item){
    $('.selectedElement').removeClass();
    $("#weaponOffsets span").eq(item).addClass('selectedElement');
    selectedItem = $('#weaponOffsets span').eq(itemNumber).find('.setting').attr('id');
}

function upNav(){
    if(itemNumber > 0){
        itemNumber--;
        updateSelection(itemNumber);
    }
}

function downNav(){
    if(itemNumber < $('#weaponOffsets span').length-1){
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
        $("#buttonBar img").eq(3).attr("src","dew://assets/buttons/"+controllerType+"_B.png");
        $('#buttonBar img').show();
    });
}

function onControllerDisconnect(){
    $('.selected').removeClass(); 
    $('button img').hide();    
}

dew.on("show", function(e){
    dew.command('Weapon.JSON.File', {}).then(function(response){
        $('#wOffsetConfig').val(response);
    });
    dew.command('Weapon.List', {}).then(function(response){
        $('#weapList').empty();
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
    dew.command('Weapon.Equipped json', {}).then(function(response){
        var weaponInfo = JSON.parse(response);
        $('#weapList').val(weaponInfo.Name);
        $('#weaponI').val(weaponInfo.FirstPersonWeaponOffset[0] * -1);
        $('#weaponIText').val(parseFloat(weaponInfo.FirstPersonWeaponOffset[0]).toFixed(3));
        $('#weaponJ').val(weaponInfo.FirstPersonWeaponOffset[1] * -1);
        $('#weaponJText').val(parseFloat(weaponInfo.FirstPersonWeaponOffset[1]).toFixed(3));
        $('#weaponK').val(weaponInfo.FirstPersonWeaponOffset[2] * -1);
        $('#weaponKText').val(parseFloat(weaponInfo.FirstPersonWeaponOffset[2]).toFixed(3));
    });
});

dew.on('controllerinput', function(e){       
    if(hasGP){
        if(e.data.Y == 1){
            $('#resetButton').click();
        }
        if(e.data.B == 1){
            closeWindow();  
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
       if(e.data.AxisLeftX > axisThreshold){
            stickTicks.right++;
        }else{
            stickTicks.right = 0;
        }
        if(e.data.AxisLeftX < -axisThreshold){
            stickTicks.left++;
        }else{
            stickTicks.left = 0;
        }
        if(e.data.AxisLeftY > axisThreshold){
            stickTicks.up++;
        }else{
            stickTicks.up = 0;
        }
        if(e.data.AxisLeftY < -axisThreshold){
            stickTicks.down++;
        }else{
            stickTicks.down = 0;
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