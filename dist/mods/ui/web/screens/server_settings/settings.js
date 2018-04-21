var activePage = "#page1";
var selectedItem;
var itemNumber = 0;
var tabIndex = 0;
var changeArray = [];
var commandValues = [];
var vetoEnabled;
var unlimitedSprint;
var hasGP = false;
var axisThreshold = .5;
var stickTicks = { left: 0, right: 0, up: 0, down: 0 };
var repGP;
var lastHeldUpdated = 0;

var settingsToLoad = [
    ['sName', 'Server.Name', 'Server Name', 'Desc'],
	['sMatchCountdown', 'Server.CountdownLobby', 'Match Start Countdown', 'Desc'], 
    ['sCountdown', 'Server.Countdown', 'Round Start Countdown', 'Desc'], 
    ['sMaxPlayers', 'Server.MaxPlayers', 'Max Players', 'Desc'], 
    ['sNumberTeams', 'Server.NumberOfTeams', 'The desired number of teams', 'Desc'], 
    ['sShouldAnnounce', 'Server.ShouldAnnounce', 'Display Server in Browser', 'Desc'],
    ['sDualWieldEnabled', 'Server.DualWieldEnabled', 'Dual Wielding', 'Desc'], 
    ['sAssassinationEnabled', 'Server.AssassinationEnabled', 'Assassinations', 'Desc'], 
    ['sPass', 'Server.Password', 'Password', 'Desc'], 
    ['sMessage', 'Server.Message', 'Join Message', 'Desc'],  
    ['sTeamShufflingEnabled','Server.TeamShuffleEnabled', 'Team Shuffle', 'Desc'],
    ['sMapVotingTime','Server.MapVotingTime', 'Voting Countdown', 'Desc'],
    ['sNumOfRevotes','Server.NumberOfRevotesAllowed', 'Revotes', 'Desc'],
    ['sNumberOfVotingOptions','Server.NumberOfVotingOptions', 'Voting Options', 'Desc'],
    ['sVotingDuplicationLevel','Server.VotingDuplicationLevel', 'Voting Duplication', 'Desc'],
    ['sTimeBetweenVoteEndAndGameStart','Server.TimeBetweenVoteEndAndGameStart', 'Winner/Start Time', 'Desc'],
    ['sNumOfVetoes','Server.NumberOfVetoVotes', 'Max Veto Votes Per Round', 'Desc'],
    ['sVetoVoteTime','Server.VetoVoteTime', 'Veto Countdown', 'Desc'],
    ['sVetoWinningShowTime','Server.VetoWinningOptionShownTime', 'Winning Vote Display Time', 'Desc'],
    ['sVetoPassPercentage','Server.VetoVotePassPercentage', 'Vote Pass Percentage', 'Desc'],
    ['sVetoSystemSelectionType','Server.VetoSystemSelectionType', 'Veto System Selection Type', 'Desc'],
    ['sUPNP','UPnP.Enabled', 'Universal Plug and Play (UPnP)', 'Desc'],
    ['sNetworkMode', '', 'Network Mode', 'Desc'],
    ['sVotingStyle', '', 'Voting Mode', 'Desc']
];

var bindChangeArray = [];
var subPages = [];

$(document).ready(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            cancelButton();
        }
        if (e.keyCode == 44) {
            dew.command('Game.TakeScreenshot');  
        }
    });
    $(document).keydown(function(e){
        if(e.keyCode == 192 || e.keyCode == 223){
            dew.show('console');
        }
    });
    location.hash = activePage;
    $('.tinySetting').on('change', function(){
        var newID = $(this).attr('id');
        if(newID.endsWith('Text')){
            newID = newID.slice(0, -4);
        }
        $('#'+newID).val($(this).val());
    });
    $('input[type=range]').on('input', function(){
        var newID = $(this).attr('id') + 'Text';
        $('#'+newID).val($(this).val());
    });
    $('#settingsWindow input:not(#lookSensitivity,#lookSensitivityText), #settingsWindow select,#settingsWindow textarea').on('change', function(e){
        var elementID = e.target.id;
        if($('#'+elementID).hasClass('tinySetting') && e.target.id.endsWith('Text')){
            elementID = elementID.slice(0, -4);
        }
        var newValue = e.target.value;
        if(e.target.computedRole == 'checkbox'){
            if($('#'+elementID).is(':checked')){
                newValue = '1';
            }else{
                newValue = '0';
            }
        }
        $.grep(settingsToLoad, function(result){
            if(result[0] == elementID){
                if(result[1].length){
                    dew.command(result[1]+' "'+newValue+'"');
                }
            };
        });
        dew.command('Game.PlaySound 0x0B00');
    });
    $('#applyButton').on('click', function(e){
        applyButton();
    });
    $('#cancelButton').on('click', function(e){
        cancelButton();
    });
    $('#sVotingStyle').on('change', function(){
        updateVotingStyle(this.value);
        if(hasGP){
            if(itemNumber > $(activePage + ' label:visible').length-1){
                itemNumber = $(activePage + ' label:visible').length-1;
            }
            updateSelection(itemNumber, true, true);
        }
    });
    $('#sSprint').on('change', function(){
        updateSprint(this.value);
    });
    setControlValues();
    
    dew.command('Server.VotingEnabled', {}).then(function(x){
        dew.command('Server.VetoSystemEnabled', {}).then(function(y){
            if(x == '0' && y == '1'){
                $('#sVotingStyle').val('2');
                $('#voting').hide();
                $('#veto').show();
            }else if(x == '1' && y == '0'){
                $('#sVotingStyle').val('1');
                $('#voting').show();
                $('#veto').hide();
            }else{
                $('#sVotingStyle').val('0');
                $('#voting, #veto').hide();
            }
        });       
    });
    dew.command('Server.SprintEnabled', {}).then(function(a){
        dew.command('Server.UnlimitedSprint', {}).then(function(b){
            if(a == '1' && b == '1'){
                $('#sSprint').val('2');
            }else if(a == '1' && b == '0'){
                $('#sSprint').val('1');
            }else{
                $('#sSprint').val('0');
            }
        });       
    });
    
    dew.on('controllerinput', function(e){    
        if(hasGP){    
            if(e.data.A == 1){
                if($('#'+selectedItem).prev()[0].computedRole == 'button'){
                    $('#'+selectedItem).prev().click();
                }else if(activePage.endsWith('alertBox')){
                    hideAlert(true);
                }else{    
                    toggleSetting();
                }
            }
            if(e.data.B == 1){
                if(activePage.endsWith('alertBox')){
                    dismissButton();
                }else{
                    cancelButton();
                }
            }
            if(e.data.X == 1){
                if($('#'+selectedItem).prev()[0].computedRole == 'button'){
                    $('#'+selectedItem).prev().click();
                }
            }
            if(e.data.Up == 1){
                upNav();
            }
            if(e.data.Down == 1){
                downNav();
            }
            if(e.data.Left == 1){
                leftToggle();
            }
            if(e.data.Right == 1){
                rightToggle();
            }
            if(e.data.LeftBumper == 1){
                prevPage();
            }
            if(e.data.RightBumper == 1){
                nextPage();
            }
            if(e.data.Start == 1){
                applyButton();
                hideAlert(true);
            }
            if(e.data.LeftTrigger != 0){
                if(itemNumber > 0){
                    itemNumber = 0;
                    updateSelection(itemNumber, true, true);
                }
            }
            if(e.data.RightTrigger != 0){
                if(itemNumber < $(activePage + ' label:visible').length-1){
                    itemNumber = $(activePage + ' label:visible').length-1;
                    updateSelection(itemNumber, true, true);
                }
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
    var clicking = false;
    var currentPos = {x: null, y: null};

    $('span').has('.setting').mouseover(function(){
        if(hasGP){
            itemNumber = $(activePage+' span').has('.setting').index($(this));
            updateSelection(itemNumber, false, false);
        }
    });

    $('#okButton').on('click', function(){
        hideAlert(true);
    });
    $('#dismissButton').on('click', function(){
        dismissButton();
    });
    $('span:has(.setting)').hover(
        function(){
            $(this).addClass('selectedElement');
            setInfoBox($(this).find('.setting').attr('id'));
        }, function(){
            $(this).removeClass('selectedElement');
        }
    );
    $('#sNetworkMode').on('change', function(){
        dew.command('Server.Mode '+$(this).val());
    });
});

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

function setButtons(){
    dew.command('Game.IconSet', {}).then(function(response){
        $('#randomArmor img').attr('src','dew://assets/buttons/' + response + '_X.png');
        $('#randomColors img').attr('src','dew://assets/buttons/' + response + '_Y.png');
        $('#applyButton img').attr('src','dew://assets/buttons/' + response + '_Start.png');
        $('#cancelButton img').attr('src','dew://assets/buttons/' + response + '_B.png');
        $('#dismissButton img').attr('src','dew://assets/buttons/' + response + '_B.png');
        $('#okButton img').attr('src','dew://assets/buttons/' + response + '_A.png');
        $('.tabs img').eq(0).attr('src','dew://assets/buttons/' + response + '_LB.png');
        $('.tabs img').eq(1).attr('src','dew://assets/buttons/' + response + '_RB.png');
    });
}

dew.on('show', function(e){
    $('#settingsWindow').hide();
    $('#blackLayer').hide();
    dew.getSessionInfo().then(function(i){
        if(i.mapName == "mainmenu"){
            $('#blackLayer').fadeIn(200, function() {
                dew.command('game.hideh3ui 1');
                $('#settingsWindow').show();
                $('#blackLayer').show();
                //initActive();
                initGamepad();
            }).fadeOut(200);
        } else {
            $('#settingsWindow').show();
            //initActive();
            initGamepad();
        }
    });
    setControlValues();
    dew.command('Server.VotingEnabled', {}).then(function(x){
        dew.command('Server.VetoSystemEnabled', {}).then(function(y){
            if(x == '0' && y == '1'){
                $('#sVotingStyle').val('2');
                $('#voting').hide();
                $('#veto').show();
            }else if(x == '1' && y == '0'){
                $('#sVotingStyle').val('1');
                $('#voting').show();
                $('#veto').hide();
            }else{
                $('#sVotingStyle').val('0');
                $('#voting, #veto').hide();
            }
        });       
    });
    dew.command('Server.SprintEnabled', {}).then(function(a){
        dew.command('Server.UnlimitedSprint', {}).then(function(b){
            if(a == '1' && b == '1'){
                $('#sSprint').val('2');
            }else if(a == '1' && b == '0'){
                $('#sSprint').val('1');
            }else{
                $('#sSprint').val('0');
            }
        });       
    });
    
    dew.command('Server.Mode', {}).then(function(response){
        $('#sNetworkMode').val(response);
    });
});

function initGamepad(){
    dew.command('Settings.Gamepad', {}).then(function(result){
        if(result == 1){
            hasGP = true;
            if(!repGP){
                repGP = window.setInterval(checkGamepad,1000/60);
            }
            onControllerConnect();
            setButtons();
            $('button img,.tabs img').show();
        }else{
            onControllerDisconnect();
            hasGP = false;
            if(repGP){
                window.clearInterval(repGP);
                repGP = null;
            }
            $('button img,.tabs img').hide();
        }
    });
}

dew.on('hide', function(e){
    if(repGP){
        window.clearInterval(repGP);
        repGP = null;
    }
    hideAlert(false);
    dew.command('Game.HideH3UI 0');
});

function initActive(){
    tabIndex = 0;
    $('.selected').removeClass('selected');
    $('.tabs li:visible').eq(0).addClass('selected');
    location.hash = $('.selected a')[0].hash;
    activePage = window.location.hash;
}

function setControlValues(){
    commandValues = [];
    dew.getCommands().then(function (commands){
        for(i = 0; i < commands.length; i++){
            var setValue = commands[i].value;
            $.grep(settingsToLoad, function(result){
                if(result[1] == commands[i].name){
                    commandValues.push([result[0],commands[i].name,commands[i].value]);
                        if($('#'+result[0]).is(':checkbox')){
                            if(setValue == '1'){
                                $('#'+result[0]).prop('checked', true);
                            }else{
                                $('#'+result[0]).prop('checked', false);                               
                            }
                        }else{
                            if($('#'+result[0]).hasClass('tinySetting')){
                                setValue = parseFloat(setValue);
                            }
                            $('#'+result[0]).val(setValue);
                        }
                        $('#'+result[0]).val(setValue);
                        if($('#'+result[0]).hasClass('hasTiny')){
                            $('#'+result[0]+'Text').val(setValue);
                        }

                };
            });
        }
    })
}

function switchPage(pageHash){
    itemNumber = 0;
    location.href=pageHash;
    activePage=pageHash;    
    if(hasGP){
        updateSelection(0, true, true);
    }
    if(subPages.indexOf(pageHash) != -1){
        $('#cancelButton').html('<img class="button">Back');
    }else{
        if(changeArray.length || bindChangeArray.length){
            $('#cancelButton').html('<img class="button">Cancel');
        }else{
            $('#cancelButton').html('<img class="button">Close');
        }
    }
    initGamepad();
}

function editControls(which){
    if(which == 0){
        switchPage('#page5');
    }else{
        switchPage('#page8');
    }
}

function applySettings(i){
    if(i != changeArray.length){
        dew.command(changeArray[i][0] + ' "' + changeArray[i][1] + '"', {}).then(function(){
			i++;
			applySettings(i);            
        }).catch(function (error){
            console.log(error);
            i++;
			applySettings(i);  
        });
    }else{
        changeArray = [];
        dew.command('writeconfig');
        dew.command('VoIP.Update');
        if(subPages.indexOf(activePage) != -1){
            $('#cancelButton').html('<img class="button">Back');
        }else{
            $('#cancelButton').html('<img class="button">Close');
        }
        $('#applyButton').hide();
        initGamepad();
    }
}

function applyBindChanges(i){
    if(i != bindChangeArray.length){    
        if (bindChangeArray[i][1] == "Back") { bindChangeArray[i][1] = "Select"; }
        if (bindChangeArray[i][1]) { bindChangeArray[i][1] = "\"" + bindChangeArray[i][1] + "\""; }
        dew.command("Input.ControllerAction \"" + bindChangeArray[i][0] + "\" " + bindChangeArray[i][1], {}).then(function(){
            i++;
            applyBindChanges(i);            
        }).catch(function (error){
            console.log(error);
            i++;
            applyBindChanges(i);  
        });
    }else{
        bindChangeArray = [];
        dew.command('writeconfig');
        if(subPages.indexOf(activePage) != -1){
            $('#cancelButton').html('<img class="button">Back');
        }else{
            $('#cancelButton').html('<img class="button">Close');
        }
        initGamepad();
    }
}


function applyButton(){
    if(window.location.hash == '#page5'){
        applyBinds();
        switchPage('#page2'); 
        if(!changeArray.length && !bindChangeArray.length){
            $('#applyButton').hide();
        }
    }else if(window.location.hash == '#page4'){
        applySettings(0);
        switchPage('#page3');     
    }else if(window.location.hash == '#page11'){ 
        switchPage('#page8');        
    }else{
        if(changeArray.length || bindChangeArray.length){
            applySettings(0); 
            applyBindChanges(0);              
            setButtons();            
        }else{
            effectReset();
        }
    }
}

function cancelButton(){
    effectReset();
    dew.command('writeconfig');
}

function dismissButton(){
    hideAlert(false);
    resetInstants();    
    itemNumber = 0;
    effectReset();
    setControlValues();
    changeArray = [];
    if(bindChangeArray.length){
        initializeBindings();
        bindChangeArray = [];
    }
    $('#applyButton').hide();
}

exiting = false;
function effectReset(){
    // Prevent escape spamming
    if(exiting)
        return;
    exiting = true;

    dew.command('Game.PlaySound 0x0B04');
    dew.getSessionInfo().then(function(i){
        if(i.mapName == "mainmenu"){
            $('#blackLayer').fadeIn(200, function(){
                dew.command('game.hideh3ui 0');
                $('#settingsWindow').hide();
                $('#blackLayer').fadeOut(200, function(){
                    dew.hide();
                    $('#settingsWindow').show();
                    exiting = false;
                });
            });
        }else{
            dew.hide();
            exiting = false;
        }
    })
}

function setOptionList(ElementID, ArrayVar){
    var sel = document.getElementById(ElementID);
    for(var i = 0; i < ArrayVar.length; i++){
        var opt = document.createElement('option');
        opt.innerHTML = ArrayVar[i][0];
        opt.value = ArrayVar[i][1];
        sel.appendChild(opt);
    }
}

function updateVotingStyle(value){
    if(value == "0"){
        dew.command('Server.VotingEnabled 0');
        dew.command('Server.VetoSystemEnabled 0');
        $('#voting, #veto').hide();
    }else if(value == "1"){
        dew.command('Server.VotingEnabled 1');
        dew.command('Server.VetoSystemEnabled 0');
        $('#veto').hide();
        $('#voting').show();
    }else{
        dew.command('Server.VotingEnabled 0');
        dew.command('Server.VetoSystemEnabled 1');
        $('#voting').hide();
        $('#veto').show();
    }
}

function updateSprint(value){
    if(value == "0"){
        dew.command('Server.SprintEnabled 0');
        dew.command('Server.UnlimitedSprint 0');
    }else if(value == "1"){
        dew.command('Server.SprintEnabled 1');
        dew.command('Server.UnlimitedSprint 0');
    }else{
        dew.command('Server.SprintEnabled 1');
        dew.command('Server.UnlimitedSprint 1');
    }
}

function updateSelection(item, sound, move){
    colorIndex = 0;
    $('.selectedElement').removeClass('selectedElement');
    $(activePage + ' label:visible').eq(item).parent().addClass('selectedElement');
    selectedItem = $(activePage + ' .setting:visible').not('span').eq(itemNumber).attr('id');
    if(move){
        $('#'+selectedItem).parent()[0].scrollIntoView(false);
    }
    if(sound){
        dew.command('Game.PlaySound 0xAFE');
    }
    setInfoBox(selectedItem);
}

function prevPage(){
    if(tabIndex > 0){
        $('.tabs li:visible a').eq(tabIndex-1).click();
    }        
}

function nextPage(){
    var tabLength = $('.tabs li').length-1;
    if(tabIndex < tabLength){
        $('.tabs li:visible a').eq(tabIndex+1).click();
    }        
}

function upNav(){
    if(itemNumber > 0){
        itemNumber--;
        updateSelection(itemNumber, true, true);
    }
}

function downNav(){
    if(itemNumber < $(activePage + ' label:visible').length-1){
        itemNumber++;
        updateSelection(itemNumber, true, true);
    }           
}

function onControllerConnect(){
    updateSelection(itemNumber, false, true);
    $('button img, .tabs img').show();
}

function onControllerDisconnect(){
    $('.selectedItem').removeClass(); 
    $('button img, .tabs img').hide();
}

function resetInstants(){
    for(var i = 0; i < $('.instant').length; i++) {
        var elementID = $('.instant').eq(i).attr('id');
        $.grep(commandValues, function(result){
            if(result[0] == elementID){
                dew.command(result[1] + ' "' + result[2]+'"');
            };
        });
    }
}

function leftToggle(){
    if(document.getElementById(selectedItem).computedRole == "combobox"){
        var elementIndex = $('#'+selectedItem+' option:selected').index();
        if(elementIndex > 0){
            var newElement = elementIndex - 1;
            $('#'+selectedItem+' option').eq(newElement).prop('selected', true);
            $('#'+selectedItem).trigger('change');
        }
    }
    if(document.getElementById(selectedItem).computedRole == "slider"){
        if(parseInt(document.getElementById(selectedItem).value) > parseInt(document.getElementById(selectedItem).min)){
            document.getElementById(selectedItem).stepDown();
            document.querySelector('#'+selectedItem +'Text').value = document.getElementById(selectedItem).value; 
            $('#'+selectedItem).trigger('change');
        }
    }
    if($('#'+selectedItem).hasClass('color')){
        colorIndex = $.inArray($('#'+selectedItem).val(), h3ColorArray);
        if(colorIndex == -1){ colorIndex = 0 };       
        if($('#'+selectedItem).val()==h3ColorArray[colorIndex]){
            if(colorIndex > 0){
                colorIndex--;
            }    
        }
        setColor(h3ColorArray[colorIndex]);
    }
}

function rightToggle(){
    if(document.getElementById(selectedItem).computedRole == "combobox"){
        var elementIndex = $('#'+selectedItem+' option:selected').index();
        var elementLength = $('#'+selectedItem).children('option').length;
        if(elementIndex < elementLength - 1){
            var newElement = elementIndex + 1;
            $('#'+selectedItem+' option').eq(newElement).prop('selected', true);
            $('#'+selectedItem).trigger('change');
        } 
    }
    if(document.getElementById(selectedItem).computedRole == "slider"){
        if(parseInt(document.getElementById(selectedItem).value) < parseInt(document.getElementById(selectedItem).max)){
            document.getElementById(selectedItem).stepUp();
            document.querySelector('#'+selectedItem +'Text').value = document.getElementById(selectedItem).value;   
            $('#'+selectedItem).trigger('change');
        }
    }
    if($('#'+selectedItem).hasClass('color')){
        colorIndex = $.inArray( $('#'+selectedItem).val(), h3ColorArray);
        if(colorIndex == -1){ colorIndex = 0 };
        if($('#'+selectedItem).val()==h3ColorArray[colorIndex]){
            if(colorIndex < h3ColorArray.length-1){
                colorIndex++;
            }      
        }
        setColor(h3ColorArray[colorIndex]);
    }
}

function toggleSetting(){
    if(document.getElementById(selectedItem).computedRole == "checkbox"){
        if(document.getElementById(selectedItem).checked){
            document.getElementById(selectedItem).checked = false;
        }else{
            document.getElementById(selectedItem).checked = true;
        }
        $('#'+selectedItem).trigger('change'); 
    }       
}

function queueChange(changeBlock){
    $('#cancelButton').html('<img class="button">Cancel');
    $('#applyButton').show();
    if(hasGP){
        setButtons();
        $('button img,.tabs img').show();
    }
    $.grep(changeArray, function(result, index){
        if(result){
            if(result[0] == changeBlock[0]){
                changeArray.splice(index,1);
            };
        }
    });
    changeArray.push(changeBlock);
}

function alertBox(alertText, dismissButton){
    if(dismissButton){
        $('#dismissButton').show();
    }else{
        $('#dismissButton').hide();
    }
    $('#wDescription').text(alertText);
    $('#alertBox').fadeIn(100);
    activePage = activePage+'alertBox';
    dew.command('Game.PlaySound 0x0B02');
}

function hideAlert(sound){
    $('#alertBox').hide();
    activePage = activePage.replace('alertBox', ''); 
    if(sound){
        dew.command('Game.PlaySound 0x0B04');
    }
}

function setInfoBox(ID){
    $.grep(settingsToLoad, function(result, index){
        if(result){
            if(result[0] == ID){
                $('#infoHeader').text(result[2]);
                $('#infoText').text(result[3]);
            };
        }
    });    
}