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
  ['AIR ASSAULT','air_assault','Based on a proven Ushuaia Armory design, Naphtali hopes to leverage the AIR ASSAULT\'s all-around excellent performance and combat endurance to compete for the UNSC\'s lucrative Spartan recruit armor contract.'],
  ['BREAKER','mercenary','The BREAKER is a no-nonsense Mjolnir variant being marketed as a "tactical supremacy" design, with increased endurance in the field, battle-tested components, and compatibility with the full range of tactical packages now available.'],
  ['CALADCHOLG','gungnir','Not intended for mass production, the CALADCHOLG was designed and manufactured simply to demonstrate a new design for fast-switching Mjolnir muscle fibers, which allow for quick return to a combat posture after sprinting.'],
  ['COMMANDO','hammerhead','Though it has the rights to the design of Chalybs Defense Solutions original Mk V/K armor, Naphtali has adopted the COMMANDO name for a new design fitted with high-capacity energy banks for extended tactical package usage.'],
  ['CYCLOPS','ballista','A product of IMC\'s secretive Io Design Lab, small superconducting loops spaced throughout the CYCLOPS subassembies and a more powerful fusion power pack give a substantial edge in fights that make use of tactical packages.'],
  ['FLENSER','mac','The FLENSER design is so new that final fabrication took place on Anvil Station itself, conveniently bypassing several testing protocols in the process. It remains to be seen how stable the suits new low-power muscle fiber system is, as the first prototype burned itself out after a single War Games match.'],
  ['GUNSLINGER','scanner','The GUNSLINGER design is unusual in that it is being built around the requirements of UEG police forces, with a tweaked smart-link system to increase handgun accuracy.'],
  ['HALBERD','halberd','Cloaked in secrecy, all users of the HALBERD-class armor must sign binding non-disclosure agreements about its performance and the particulars of its new weapon mag-lock arrangement.'],
  ['HOPLITE','hoplite','Initially designed as an alternative to the SPI armor used by SPARTAN-III operatives, the notoriously secretive Watershed group has unexpectedly revealed the HOPLITE as a low-cost Mjolnir variant optimized for pairing with UNSC pistols.'],
  ['INFILTRATOR','demo','INFILTRATOR-class armor is a new class of Mjolnir armor, created to subvert and penetrate security systems at heavily fortified sites which require on-site taps. To facilitate this, the suit has a faster recovery period after evading enemy patrols and sensors with sprint.'],
  ['INQUISITOR','shark','INQUISITOR-class armor does not utilize a fusion reactor, instead relying on an intact Forerunner Sentinel power cell. This allows for incredible flexibility in reallocating power to critical tactical packages.'],
  ['JUGGERNAUT','juggernaut','The JUGGERNAUT\'s design was thought lost during the fall of Reach. Recovered by a corporate scout team in 2553, the prototype helmets were paired with a GEN2 prototype harness and shipped to Anvil Station for integration testing.'],
  ['MIRMILLON','gladiator','Taking form defining function to an extreme, RKD\'s MIRMILLON-class armor optimizes combat flexibility by using best-in-class tactical package components that require less power to charge.'],
  ['OMNI','omni','A surprisingly effective composite of other Acheron Security designs, the OMNI is currently the company\'s cutting-edge Mjolnir suit, and one of the few able to effectively use Material Groups GEN2 prototype sensor suite.'],
  ['ORACLE','oracle','More an integrated experiment than a fully-realized Mjolnir variant, the ORACLE is testing a host of improvements in energy recuperation and power-distrubution subsystems for eventual inclusion in a standalone tactical package.'],
  ['PIONEER','chameleon','Though further refinements are expected, Acheron Security has staked the reputation of its Military Sensors Division on the PIONEER\'s advanced threat detection system.'],
  ['RAMPART','cyclops','The RAMPART armor system is one of several armor system prototypes from Materials Group which are tailored for operation in built-up areas, such as urban areas and space habitats. The RAMPART is particularly well-suited for hunter-killer operations in cramped arcologies.'],
  ['REAPER','silverback','Considered an early work-in-progress by Hannibal\'s design team, the REAPER-class armor still requires substantial refactoring of its interface system and stabilization of the tactical pack power feeds.'],
  ['RENEGADE','renegade','Obstentially a strategic reconnaissance variant, users have noticed the RENEGADE\'s sensor suite is calibrated specifically to track other Mjolnir suits; particularly those featuring emissions cloaking.'],
  ['SCEPTRE','nihard','Though capable of efficiently utilizing the new modular tactical package system, Lethbridge has also modified several SCEPTRE suits to use the alternative plug-in upgrade system championed by Materials Group.'],
  ['SPECTRUM','spectrum','All business in design, the SPECTRUM-class Mjolnir armor is designed to get up close and personal with Covenant forces inside their starships and destroy them using high explosives and pinpoint gunfire.'],
  ['STRIDER','strider','With its extended-range sensors, the STRIDER was paired with prototype suits that lacked their full combat suite during the desperate defense of Concord in 2551. Now finalized, the suit is undergoing final testing at Anvil Station.'],
  ['TREMOR','dutch','TREMOR-variant armor removes several layers of the Mjolnir\'s threat management and safety interlocks, allowing programmable grenades to be set to higher yields.'],
  ['VENATOR','stealth','VENATOR is Lethbridge Industrial\'s flagship GEN2 prototype, incorporating high-performance power systems and hardwired CQB protocols.'],
  ['VOID DANCER','orbital','Hardened against the perils of exoatmospheric operation and equipped with the latest threat assessment firmware, the VOID DANCER is Acheron\'s entry for the ODST\'s Generation After Next armor upgrade competition.'],
  ['WIDOW MAKER','widow_maker','High-threat dynamic combat actuation is the WIDOW MAKER\'s raison d\'être. With hardwired response protocols and quick-release mag-clamps the operator can quickly assess the situation and use the most appropriate weapon at hand.']
];
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
