var activePage;
var itemNumber = 0;
var tabIndex = 0;
var commandValues = [];
var hasGP = false;
var axisThreshold = .5;
var stickTicks = { left: 0, right: 0, up: 0, down: 0 };
var repGP;
var lastHeldUpdated = 0;

var h3ColorArray = [
    ['Steel','#626262'],
    ['Silver','#B0B0B0'],
    ['White','#DEDEDE'],
    ['Red','#9B3332'],
    ['Mauve','#DB6766'],
    ['Salmon','#EE807F'],
    ['Orange','#DB8B00'],
    ['Coral','#F8AE58'],
    ['Peach','#FECB9C'],
    ['Gold','#CCAE2C'],
    ['Yellow','#F3BC2B'],
    ['Pale','#FDD879'],
    ['Sage','#57741A'],
    ['Green','#90A560'],
    ['Olive','#D8EFA7'],
    ['Teal','#31787E'],
    ['Aqua','#4ABBC1'],
    ['Cyan','#91EDEC'],
    ['Blue','#325992'],
    ['Cobalt','#5588DB'],
    ['Sapphire','#97B5F5'],
    ['Violet','#553E8F'],
    ['Orchid','#9175E3'],
    ['Lavender','#C4B4FD'],
    ['Crimson','#830147'],
    ['Ruby Wine','#D23C83'],
    ['Pink','#FC8BB9'],
    ['Brown','#513714'],
    ['Tan','#AC8A6E'],
    ['Khaki','#E0BEA2']
];
var settingsToLoad = [
    ['armorHelmet', 'Player.Armor.Helmet','Helmet','The thing that goes on your head', 0], 
    ['armorChest', 'Player.Armor.Chest','Chest','From arm to arm', 1],
    ['armorShoulders', 'Player.Armor.Shoulders','Shoulders','The bendy bit at the top', 2], 
    ['armorArms', 'Player.Armor.Arms','Arms','With these wide open', 3], 
    ['armorLegs', 'Player.Armor.Legs','Legs','Never skip leg day', 4], 
    ['colorsPrimary', 'Player.Colors.Primary','Primary Color','Your main color',0], 
    ['colorsSecondary', 'Player.Colors.Secondary','Secondary Color','Your accent color',1], 
    ['colorsVisor', 'Player.Colors.Visor','Visor Color','Tint the world',2],
    ['colorsLights', 'Player.Colors.Lights','Light Color','Like Christmas, but more subtle',3],
    ['colorsHolo', 'Player.Colors.Holo','Holo Color','When armor and Vegas colide',4]
];
var armorList = [
    ['Air Assault','air_assault','Description'],
    ['Ballista','ballista','Description'],
    ['Chameleon','chameleon','Description'],
    ['Cyclops','cyclops','Description'],
    ['Demo','demo','Description'],
    ['Dutch','dutch','Description'],
    ['Gladiator','gladiator','Description'],
    ['Gungnir','gungnir','Description'],
    ['Halberd','halberd','Description'],
    ['Hammerhead','hammerhead','Description'],
    ['Hoplite','hoplite','Description'],
    ['Juggernaut','juggernaut','Description'],
    ['Mac','mac','Description'],
    ['Mercenary','mercenary','Description'],
    ['Nihard','nihard','Description'],
    ['Omni','omni','Description'],
    ['Oracle','oracle','Description'],
    ['Orbital','orbital','Description'],
    ['Renegade','renegade','Description'],
    ['Scanner','scanner','Description'],
    ['Shark','shark','Description'],
    ['Silverback','silverback','Description'],
    ['Spectrum','spectrum','Description'],
    ['Stealth','stealth','Description'],
    ['Strider','strider','Description'],
    ['Widow Maker','widow_maker','Description']
];
var subPages = [];

$(document).ready(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            cancelButton();
        }
    });
    $(document).keydown(function(e){
        if(e.keyCode == 192 || e.keyCode == 223){
            dew.show('console');
        }
    });
    setRadioList('armorHelmet', armorList);
    setRadioList('armorChest', armorList);
    setRadioList('armorShoulders', armorList);
    setRadioList('armorArms', armorList);
    setRadioList('armorLegs', armorList);
    setRadioList('colorsPrimary', h3ColorArray);
    setRadioList('colorsSecondary', h3ColorArray);
    setRadioList('colorsVisor', h3ColorArray);
    setRadioList('colorsLights', h3ColorArray);
    setRadioList('colorsHolo', h3ColorArray);
    $('.tabs li a').click(function(e){
        $('.tabs li').removeClass('selected');
        $(this).parent().addClass('selected');
        window.location.href = e.target.href;
        activePage = e.target.hash;
        itemNumber = 0;
        $(e).ready(function(){
            if(hasGP){
                updateSelection(itemNumber, false);
            }
            tabIndex = $('.tabs li:visible a').index($("a[href='"+activePage+"']"));
        });
        $('#infoHeader, #infoText').text('');
        if($(activePage + ' form:visible')){
            $.grep(settingsToLoad, function(result, index){
                if(result){
                    if(result[0] == $(activePage + ' form:visible').attr('id')){
                        $('#infoBox #infoHeader').text(result[2]);
                        $('#infoBox #infoText').text(result[3]);
                        $('.baseNav').removeClass('selectedElement');
                        itemNumber = result[4];
                        $(activePage+' .baseNav').eq(result[4]).addClass('selectedElement');
                    }
                }
            });
        }
        dew.command('Game.PlaySound 0x0B00');
    });
    $('.colorForm input, .armorForm input').on('change click', function(e){
        $.grep(settingsToLoad, function(result){
            if(result[0] == e.target.name){
                dew.command(result[1]+' '+e.target.value);
                $('#infoBox #infoText').text(result[2]);
            };
            $('#infoBox #infoHeader').text(e.target.computedName);
        });
        $('#infoBox #infoText').text($(this).attr('desc'));
    });
    $('#colorsPrimaryText, #colorsSecondaryText,#colorsVisorText,#colorsLightsText,#colorsHoloText').on('click', function(e){
        $(this).prev()[0].checked = true;
    });
    $('.colorForm, .armorForm').submit(function() {
        return false;
    });
    $('#applyButton').on('click', function(e){
        applyButton();
    });
    $('#cancelButton').on('click', function(e){
        cancelButton();
    });
    setControlValues();
    dew.on('controllerinput', function(e){    
        if(hasGP){    
            if(e.data.A == 1){
                if(activePage.endsWith('inputBox')){
                    dew.command('Player.Name '+$('#inputBox input').val());
                    dismissButton();
                }else{
                    selectElement();
                }
            }
            if(e.data.B == 1){
                if(activePage.endsWith('inputBox')){
                    dismissButton();
                }else if(activePage!=location.hash){
                    leftNav();
                }else{
                    cancelButton();
                }
            }
            if(e.data.X == 1){
                if(activePage.startsWith('#page1')){
                    randomArmor();
                }else if(activePage.startsWith('#page2')){
                    randomColors();
                }
            }
            if(e.data.Y == 1){
                inputBox();
            }
            if(e.data.Up == 1){
                upNav();
            }
            if(e.data.Down == 1){
                downNav();
            }
            if(e.data.Left == 1){
                leftNav();
            }
            if(e.data.Right == 1){
                selectElement();
            }
            if(e.data.LeftBumper == 1){
                prevPage();
            }
            if(e.data.RightBumper == 1){
                nextPage();
            }
            if(e.data.LeftTrigger != 0){
                if(itemNumber > 0){
                    itemNumber = 0;
                    updateSelection(itemNumber, true);
                }
            }
            if(e.data.RightTrigger != 0){
                if(itemNumber < $(activePage + ' label:visible').length-1){
                    itemNumber = $(activePage + ' label:visible').length-1;
                    updateSelection(itemNumber, true);
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
            if(e.data.AxisRightX != 0){
                if(e.data.AxisRightX > axisThreshold){
                    rotateBiped('right');
                }
                if(e.data.AxisRightX < -axisThreshold){
                    rotateBiped('left');
                };
            }
        }
    });
    var clicking = false;
    var currentPos = {x: null, y: null};
    $('#playerWindow').mousedown(function(){
        clicking = true;
    });
    $(document).mouseup(function(){
        clicking = false;
    })
    $('#playerWindow').mousemove(function(event){
        if(clicking){
            currentPos.x = event.clientX;
            currentPos.y = event.clientY;
            var xDiff = (currentPos.x + 90);
            dew.command('Player.Armor.SetUiModelRotation ' + xDiff);
        }
    });
    $('.baseNav').mouseover(function(){
       activePage = location.hash;
    });
    $('.armorForm, .colorForm').mouseover(function(){
       activePage = location.hash+' #'+$(this).attr('id'); 
    });
    $('span').has('.setting').mouseover(function(){
        if(hasGP){
            itemNumber = $(activePage+' span').has('.setting').index($(this));
            updateSelection(itemNumber, false); 
        }else{
            $(this).addClass('selectedElement');
        }
    });
    $('span').has('.setting').mouseout(function(){
        if(!hasGP){
            $(this).removeClass('selectedElement');
        }
    });
    $('#inputBox #okButton').on('click', function(){
        dew.command('Player.Name '+$('#inputBox input').val());
        hideInputBox(true);
    });
    $('#inputBox #dismissButton').on('click', function(){
        hideInputBox(true);
    });
    dew.command('Player.Name', {}).then(function(response) {
        $('#inputBox #pName').val(response);
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
        leftNav();
    }
    if(stickTicks.right == 1 || (shouldUpdateHeld && stickTicks.right > 25)){
        selectElement();
    }
};

function setButtons(){
    dew.command('Game.IconSet', {}).then(function(response){
        $('#randomArmor img').attr('src','dew://assets/buttons/' + response + '_X.png');
        $('#randomColors img').attr('src','dew://assets/buttons/' + response + '_X.png');
        $('#applyButton img').attr('src','dew://assets/buttons/' + response + '_Start.png');
        $('#cancelButton img').attr('src','dew://assets/buttons/' + response + '_B.png');
        $('#dismissButton img').attr('src','dew://assets/buttons/' + response + '_B.png');
        $('#namePrompt img').attr('src','dew://assets/buttons/' + response + '_Y.png');
        $('#okButton img').attr('src','dew://assets/buttons/' + response + '_A.png');
        $('.tabs img').eq(0).attr('src','dew://assets/buttons/' + response + '_LB.png');
        $('.tabs img').eq(1).attr('src','dew://assets/buttons/' + response + '_RB.png');
    });
}

var bipedRotate = 270;
dew.on('show', function(e){
    $('#settingsWindow').hide();
    $('#blueHeader, #blueFooter,#blackLayer').hide();
    bipedRotate = 270;
    dew.getSessionInfo().then(function(i){
        if(i.mapName == "mainmenu"){
            $('#blackLayer').fadeIn(200, function() {
                dew.command('Player.Armor.Update');
                dew.command('Player.Armor.SetUiModelRotation 270');
                dew.command('game.hideh3ui 1');
                dew.command('Game.ScenarioScript settings_cam');
                dew.command('Game.ScreenEffectRange 0 0');
                $('#settingsWindow').show();
                $('#blueHeader, #blueFooter, #blackLayer').show();
                initActive();
                initGamepad();
            }).fadeOut(200);
        }else{
            $('#settingsWindow').show();
            initActive();
            initGamepad();
        }
    });
    setControlValues();
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
});

function rotateBiped(direction){
    var rotateAmount = 2;
    if(direction == "right"){
        bipedRotate+=rotateAmount;
    }else{
        bipedRotate-=rotateAmount;
    }
    dew.command('Player.Armor.SetUiModelRotation '+bipedRotate);
}

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
                    if($('#'+result[0]).is('form')){
                        $('#'+result[0]+' :radio[value=""]').attr('checked',true);
                        $('#'+result[0]+' :radio[value="'+setValue+'"]').attr('checked',true);
                        $('#'+result[0]+' :radio[value="'+setValue+'"]').parent().parent().addClass('selectedElement');
                        $('#'+result[0]+'Text').val(setValue);
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

function cancelButton(){
    itemNumber = 0;
    dew.command('writeconfig');
    effectReset();
}

function dismissButton(){
    hideInputBox(false);
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
                dew.command('Game.ScenarioScript leave_settings');
                dew.command('Game.ScreenEffectRange 0 1E+19');
                dew.command('Player.Armor.SetUiModelRotation 270');
                dew.command('game.hideh3ui 0');
                $('#settingsWindow').hide();
                $('#blueHeader').hide();
                $('#blueFooter').hide();
                $('#blackLayer').fadeOut(200, function(){
                    dew.hide();
                    $('#settingsWindow').show();
                    $('#blueHeader').show();
                    $('#blueFooter').show();
                    exiting = false;
                });
            });
        }else{
            dew.hide();
            exiting = false;
        }
    })
}

function setRadioList(ElementID, ArrayVar){
    var sel = document.getElementById(ElementID);
    for(var i = 0; i < ArrayVar.length; i++){
        var span = document.createElement("span");
        var label = document.createElement("label");
        var radio = document.createElement("input");
        radio.setAttribute('type', 'radio');
        radio.setAttribute('name', ElementID);
        radio.setAttribute('class', 'setting');
        if(ArrayVar[i][2]){
            radio.setAttribute('desc', ArrayVar[i][2]);
        }
        radio.value = ArrayVar[i][1];
        label.appendChild(radio);
        label.appendChild(document.createTextNode(ArrayVar[i][0]));
        span.appendChild(label);
        sel.appendChild(span);
    }
}

function randomArmor(){
    var armorArray = ['armorHelmet','armorChest','armorShoulders','armourArms','armourLegs'];
    for(var i = 0; i < armorArray.length; i++) {
        var $options = $('#'+armorArray[i]).find('input'),
            random = ~~(Math.random() * $options.length);
        $options.eq(random).prop('checked', true);    
        $.grep(settingsToLoad, function(result){
            if(result[0] == armorArray[i]){
                dew.command(result[1] + ' ' + $('#'+armorArray[i]+' input:checked').val());
            };
        });
    }
}

function randomColors(){
    var colorArray = ['colorsPrimary','colorsSecondary','colorsLights','colorsVisor', 'colorsHolo'];
    for(var i = 0; i < colorArray.length; i++) {
        var randomColor = '#'+Math.floor(Math.random()*16777215).toString(16).toUpperCase();
        $('#'+colorArray[i]+'Text').val(randomColor);
        $('#'+colorArray[i]).val(randomColor);
        $('#'+colorArray[i]+' input').eq(0).prop('checked', true);
        $.grep(settingsToLoad, function(result){
            if(result[0] == colorArray[i]){
                dew.command(result[1] + ' ' + randomColor);
            };
        });
    }   
}

function updateSelection(item, sound){
    if(item > -1){
        $(activePage+' .selectedElement').removeClass('selectedElement');
        $(activePage + ' label:visible').eq(item).parent().addClass('selectedElement');
        if($(activePage+' .selectedElement').length){
            $(activePage+' .selectedElement')[0].scrollIntoView(false);
        }
        if(sound){
            dew.command('Game.PlaySound 0xAFE');
        }
    }
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
        updateSelection(itemNumber, true);
    }
}

function downNav(){
    if(itemNumber < $(activePage + ' label:visible').length-1){
        itemNumber++;
        updateSelection(itemNumber, true);
    }           
}

function onControllerConnect(){
    updateSelection(itemNumber, false);
    $('button img, .tabs img').show();
}

function onControllerDisconnect(){
    $('.selectedElement').removeClass(); 
    $('button img, .tabs img').hide();
}

function inputBox(){
    $('#inputBox').fadeIn(100);
    activePage = activePage+'inputBox';
    $('#dismissButton').show();
}

function hideInputBox(sound,condition){
    $('#inputBox').hide();
    activePage = activePage.replace('inputBox', ''); 
    if(sound){
        if(condition=='ok'){
            dew.command('Game.PlaySound 0x0B00');              
        }else if(condition=='dismiss'){
            dew.command('Game.PlaySound 0x0B04');
        }
    }
}

function armorShow(showMe, element){
    activePage = '#page1 #'+showMe;
    $('.baseNav').removeClass('selectedElement');
    $(activePage+' .selectedElement').removeClass('selectedElement');
    element.addClass('selectedElement');
    $('.armorForm').hide();
    $('#'+showMe).show();
    $('#infoBox').show();
    $.grep(settingsToLoad, function(result, index){
        if(result){
            if(result[0] == showMe){
                $('#infoBox #infoHeader').text(result[2]);
                $('#infoBox #infoText').text(result[3]);
            }
        }
    });
    if(hasGP){
        itemNumber = 0;
        updateSelection(0, false);
    }
}

function colorShow(showMe, element){
    activePage = '#page2 #'+showMe;
    $('.baseNav').removeClass('selectedElement');
    $(activePage+' .selectedElement').removeClass('selectedElement');
    element.addClass('selectedElement');
    $('.colorForm').hide();
    $('#'+showMe).show();
    $('#infoBox').show();
    $('#infoBox #infoHeader').text($('#'+showMe+' input:checked').parent().text());
    $.grep(settingsToLoad, function(result, index){
        if(result){
            if(result[0] == showMe){
                $('#infoBox #infoHeader').text(result[2]);
                $('#infoBox #infoText').text(result[3]);
            }
        }
    });
    if(hasGP){
        itemNumber = 0;
        updateSelection(0, false);
    }
}

function leftNav(){
    if($(activePage + ' form:visible') && activePage != location.hash){
        $(activePage+' .selectedElement').removeClass('selectedElement');
        activePage = location.hash;
        itemNumber = $(activePage+' span').has('.setting').index($('span:has(.selectedElement)'));
        dew.command('Game.PlaySound 0x0B04');
    }
}

function selectElement(){
    if($(activePage + ' form:visible')){
        $(activePage+' .selectedElement').find('input').click();
    }
    if(activePage == location.hash){
        $(activePage+' .selectedElement').click();
    }
    dew.command('Game.PlaySound 0x0B00');  
}
