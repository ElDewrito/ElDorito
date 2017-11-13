var activePage;
var itemNumber = 0;
var tabIndex = 0;
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
    ['playerName', 'Player.Name','Name','Your name.', 2],
    ['serviceTag', 'Player.ServiceTag','Service Tag','Your Service Tag.', 3],
    ['armorHelmet', 'Player.Armor.Helmet','Helmet','The thing that goes on your head.', 2],
    ['armorChest', 'Player.Armor.Chest','Body','From arm to arm.', 3],
    ['armorRightShoulder', 'Player.Armor.RightShoulder','Right Shoulder','Right there on that shoulder.', 4],
    ['armorLeftShoulder', 'Player.Armor.LeftShoulder','Left Shoulder','The only shoulder that\'s left.', 5],
    ['gender', 'Player.Gender','Gender','Gender Desc', 6],
    ['colorsPrimary', 'Player.Colors.Primary','Primary Color','The primary armor color will serve you in individual combat but will be overwritten in team scenarios.',0],
    ['colorsSecondary', 'Player.Colors.Secondary','Secondary Color','The secondary armor color accents your primary color and will be overwritten in team scenarios.',1],
    ['colorsVisor', 'Player.Colors.Visor','Visor Color','Adjust the tint of your Spartan\'s visor.',2],
    ['colorsLights', 'Player.Colors.Lights','Light Color','Like Christmas, but more subtle.',3]
];
var armorShoulderList = [
    ['Mark VI','base','Supplemental Armor, Pauldron, Mjolnir: This standard-issue shoulder armor for the Mjolnir Mark VI Powered Assault Armor has been in use since October 2552. It is compatible with all Mjolnir variants.'],
    ['CQB','mp_cobra','Developed at Beweglichrüstungsysteme of Essen and tested at the Special Warfare Center in Songnam, the Mjolnir/C variant focuses on improving combat survivability and mobility.'],
    ['EOD','mp_regulator','Developed at the UNSC Damascus Materials Testing Facility on Chi Ceti 4, the Mjolnir/EOD variant was designed specifically to reduce the number of grabbing edges on the armor, decreasing the likelihood of dismemberment.'],
    ['EVA','mp_intruder','Developed at the UNSC Low/Zero Gravity Testing Facility on Ganymede, the Mjolnir/V variant focuses on increasing exoatmospheric endurance and improving mobility in zero gravity.'],
    ['Recon','mp_ninja','In developing the Mjolnir VI/R variant, the goal was to increase the armor\'s overall stealth capabilities with little or no loss of endurance. This was achieved by relying on several tried-and-true methods.'],
    ['Scout','mp_scout','The SCOUT and RECON projects were run as independent parallel projects intended to develop a single variant of the Mjolnir Powered Assault Armor with stealth capabilities.'],
    ['Security','mp_marathon','The Mjolnir Mark V(m) Powered Assault Armor was originally manufactured in 2528 and recently upgraded to be compatible with all current-issue armor variants.'],
    ['Hayabusa','mp_ryu','The critical innovation brought about by RKD\'s involvement in the development of power armor is in the use of advanced materials-reducing the weight of current generation armor by nearly a third.']
];
var armorHelmetList = [
    ['Mark VI','base','Integrated Communications Helmet, Mjolnir: This standard-issue helmet for the Mjolnir Mark VI Powered Assault Armor has been in use since October 2552. It is compatible with all Mjolnir variants.'],
    ['Mark V','mp_markv',' Originally issued in August 2542, all extant Mark V helmets have been upgraded with current-issue internal components and software.'],
    ['CQB','mp_cobra','The Mjolnir/C variant was developed and tested at UNSC facilities in Essen, Deutschland, and Songnam, Hanguk, respectively, integrating feedback gathered from the Jericho VII Theater.'],
    ['EOD','mp_regulator','The Mjolnir/EOD variant was created at UNSC facilities on Chi Ceti 4. The helmet was designed specifically to channel the pressure wave around the user\'s head, significantly reducing the likelihood of decapitation.'],
    ['EVA','mp_intruder','The Mjolnir/V variant was developed and tested at UNSC facilities in Lister, Aigburth on Ganymede, integrating feedback gathered from the Summa Deep Space Incident.'],
    ['Recon','mp_ninja','The Mjolnir VI/R variant was developed concurrently with the Mjolnir Mark VI Powered Assault Armor. The goal was to increase stealth capability without impacting endurance.'],
    ['Rogue','mp_rogue','The Mark VI[A] helmet was the first of the "privatized" variants. With the fall of the Outer Colonies the UNSC called upon private industry to manufacture previously classified war materiel.'],
    ['Scout','mp_scout','As with the RECON variant the goal was to improve the armor\'s stealth capabilities with no impact on endurance; however, the SCOUT variant relies more heavily on advanced materials.'],
    ['Security','mp_marathon','The Mjolnir V(m) variant was manufactured at the Misrah Armories Facility on Mars in 2528. It has been upgraded and modified to be compatible with all current-issue armor variants.'],
    ['Hayabusa','mp_ryu','The critical innovation brought about by RKD\'s involvement in the development of power armor is in the use of advanced materials-reducing the weight of current generation armor by nearly a third.'],
    ['ODST','mp_odst','Many technologies initially developed for Project: MJOLNIR have gained widespread adoption; the use of CTCs for body armor and helmet-integrated neural interfaces being the most visible.']
];
var armorChestList = [
    ['Mark VI','base','Mjolnir Mark VI Powered Assault Armor: This is the standard issue Powered Assault Armor for Spartans as of October 2552. It is compatible with all certified helmet and pauldron variants.'],
    ['Bungie','mp_bungie','Forged in the flames of passion and perseverance. Go forth and represent.'],
    ['CQB','mp_cobra','The Mjolnir/C variant was developed and tested at UNSC facilities in Essen, Deutschland, and Songnam, Hanguk, respectively, integrating feedback gathered from the Jericho VII Theater.'],
    ['EOD','mp_regulator','This variant was designed specifically to protect Spartans during operations involving the handling of explosive ordnance (e.g., clearing/planting land mines, demolishing enemy structures/materiel).'],
    ['EVA','mp_intruder','In developing the Mjolnir Mark VI Powered Assault Armor/V variant, emphasis was placed on increasing exoatmospheric endurance and improving mobility in zero gravity without the use of thrusters.'],
    ['Recon','mp_ninja','In developing the Mjolnir Mark VI Powered Assault Armor/R, the emphasis was to increase stealth capability, specifically by reducing its IR signature, reflective surfaces, and Cherenkov radiation emission.'],
    ['Scout','mp_scout','The Mjolnir VI/S variant was developed and tested alongside the RECON variant at the ONI\'s Ordnance Testing Facility (B5D) at Swanbourne, Perth, Western Australia.'],
    ['Hayabusa','mp_ryu','In late 2536, RKD-an Earth-based think tank-presented the UNSC Ordnance Committee with its answer to the self-contained powered armor problem: Project: HAYABUSA.'],
    ['Katana','mp_katana','To correctly use the sword, one must make it an extension of one\'s body. This is for the understanding of those intending to be warriors.']
];
var subPages = [];
var colorPicker;
var genderList = [
    ['Male','male','Male desc'],
    ['Female','female','Female desc']    
];

$(document).ready(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            if(activePage.endsWith('inputBox')){
                dismissButton();
            }else{
                if(activePage!=location.hash){
                    exitSubform();
                }else{
                    cancelButton();
                }
            }
        }
        if (e.keyCode == 44) {
            dew.command('Game.TakeScreenshot');
        }
    });
    $(document).keydown(function(e){
        if(e.keyCode == 192 || e.keyCode == 223){
            dew.show('console');
        }
        if(e.keyCode == 37){ //Left
            leftNav();
        }
        if(e.keyCode == 38){ //Up
            upNav();
        }
        if(e.keyCode == 39){ //Right
            rightNav();
        }
        if(e.keyCode == 40){ //Down
            downNav();
        }
        if(!activePage.endsWith('inputBox')){
            if(e.keyCode == 32 || e.keyCode == 13){ //Space and Enter
                selectElement();
            }
        }else{
            if(e.keyCode == 13){ //Enter to submit inputBox
                $('#inputBox #okButton').click();
            }
        }
    });
    setRadioList('armorHelmet', armorHelmetList, true);
    setRadioList('armorChest', armorChestList, true);
    setRadioList('armorRightShoulder', armorShoulderList, true);
    setRadioList('armorLeftShoulder', armorShoulderList, true);
    setRadioList('gender', genderList, true);
    setRadioList('colorsPrimary', h3ColorArray);
    setRadioList('colorsSecondary', h3ColorArray);
    setRadioList('colorsVisor', h3ColorArray);
    setRadioList('colorsLights', h3ColorArray);
    $('.tabs li a').click(function(e){
        $('.tabs li').removeClass('selected');
        $(this).parent().addClass('selected');
        window.location.href = e.target.href;
        activePage = e.target.hash;
        itemNumber = 0;
        $(e).ready(function(){
            updateSelection(itemNumber, false, true);
            tabIndex = $('.tabs li:visible a').index($("a[href='"+activePage+"']"));
        });
        $('#infoHeader, #infoText').text('');
        if($(activePage + ' form:visible')){
            $.grep(settingsToLoad, function(result, index){
                if(result){
                    if(result[0] == $(activePage + ' form:visible').attr('id')){
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
        $(this).parent().parent().parent().find('.chosenElement').removeClass('chosenElement');
        $(this).parent().parent().addClass('chosenElement');
        $.grep(settingsToLoad, function(result){
            if(result[0] == e.target.name){
                dew.command(result[1]+' '+e.target.value);
                $('#infoBox #infoText').text(result[3]);
            };
            $('#infoBox #infoHeader').text(e.target.computedName);
        });
        $('#infoBox #infoText').text($(this).attr('desc'));
    });
    $('#colorsPrimaryText, #colorsSecondaryText,#colorsVisorText,#colorsLightsText').on('click', function(e){
        $('.colorForm').hide();
        colorPicker = dew.makeColorPicker(document.querySelector('#colorPicker'));
        var whichColor = $(this);
        $(this).prev()[0].checked = true;
        var currentVal = ColorUtil.hexToHsv($(this).val().split('#')[1]);
        activePage = location.hash+" #colorPicker";
        $('#colorPicker').show();
        $('#infoBox #infoHeader').text($(this).val());
        colorPicker.setColor(currentVal);
        colorPicker.on('select', function(color) {
            var currentSelection = ColorUtil.hsvToRgb(color.h, color.s, color.v);
            whichColor.val('#'+leftPad(ColorUtil.rgbToHex(currentSelection[0],currentSelection[1],currentSelection[2]),6,'0'));
            whichColor.trigger('change');
            $('#infoBox #infoHeader').text(whichColor.val());
        });
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
                    exitSubform();
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
                rightNav();
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
                    updateSelection(itemNumber, true, true);
                }
            }
            if(e.data.RightTrigger != 0){
                if(itemNumber < $(activePage + ' label:visible').length-1){
                    itemNumber = $(activePage + ' label:visible').length-1;
                    updateSelection(itemNumber, true, true);
                }
            }
            if(activePage && activePage.endsWith(' #colorPicker')){
                colorPicker.controllerInput(e.data);
            }else{
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
            updateSelection(itemNumber, false, false);
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
        if($('#inputBox #pName').is(':visible')){
            dew.command('Player.Name "'+$('#inputBox #pName').val()+'"');
        }else if($('#inputBox #sTag').is(':visible')){
            dew.command('Player.ServiceTag "'+$('#inputBox #sTag').val().toUpperCase()+'"');
        }
        hideInputBox(true);
    });
    $('#inputBox #dismissButton').on('click', function(){
        hideInputBox(true);
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
        rightNav();
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
    $('.armorForm, .colorForm').hide();
    $('#infoHeader, #infoText').text('');
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
            dew.hide();
        }
    });
    setControlValues();
    updateSelection(0,false,true);
    adjustBiped();
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
    hideInputBox(false);
    dew.command('Game.HideH3UI 0');
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
    $('#buttonContainer span:eq(0) div:eq(0)').click();
}

function setControlValues(){
    dew.getCommands().then(function (commands){
        for(i = 0; i < commands.length; i++){
            var setValue = commands[i].value;
            $.grep(settingsToLoad, function(result){
                if(result[1] == commands[i].name){
                    if($('#'+result[0]).is('form')){
                        $('#'+result[0]+' :radio[value=""]').attr('checked',true);
                        $('#'+result[0]+' :radio[value="'+setValue+'"]').attr('checked',true);
                        if( $('#'+result[0]+' :radio[value="'+setValue+'"]').length){
                            $('#'+result[0]+' :radio[value="'+setValue+'"]').parent().parent().addClass('chosenElement');
                        }else{
                            $('#'+result[0]+' :radio[value=""]').parent().parent().addClass('chosenElement');
                        }
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

function setRadioList(ElementID, ArrayVar,hasImage){
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
        if(hasImage){
            var img = document.createElement("img");
            img.setAttribute('src', 'dew://assets/armor/'+ElementID+'/'+ArrayVar[i][1]+'.png');
            span.appendChild(img);
        }
        sel.appendChild(span);
    }
}

function randomArmor(){
    var armorArray = ['armorHelmet','armorChest','armorRightShoulder','armorLeftShoulder'];
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
    var colorArray = ['colorsPrimary','colorsSecondary','colorsLights','colorsVisor'];
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

function updateSelection(item, sound, move){
    if(item > -1){
        $(activePage+' .selectedElement').eq(0).removeClass('selectedElement');
        $(activePage + ' label:visible').eq(item).parent().addClass('selectedElement');
        if($(activePage+' .selectedElement').length && move){
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
    if(activePage.startsWith('#page2 #color')){
        if(itemNumber > 3){
            itemNumber-=3;
            updateSelection(itemNumber, true, true);
        }else if(itemNumber > 0){
            itemNumber = 0;
            updateSelection(itemNumber, true, true);
        }        
    }else{
        if(itemNumber > 0){
            itemNumber--;
            updateSelection(itemNumber, true, true);
        }
    }
}

function downNav(){
    if(activePage.startsWith('#page2 #color')){
        if(itemNumber < $(activePage + ' label:visible').length-3){
            if(itemNumber == 0){
                itemNumber+=1;
            }else{
                itemNumber+=3;
            }
            updateSelection(itemNumber, true, true);
        }
    }else{
        if((activePage.split(' ').length < 2 && itemNumber < 3 && activePage == '#page2') || (activePage.split(' ').length < 2 && itemNumber < 6 && activePage == '#page1') ||  (activePage.split(' ').length > 1 && itemNumber < $(activePage + ' label:visible').length-1)){
            itemNumber++;
            updateSelection(itemNumber, true, true);
        }
    }
}

function onControllerConnect(){
    updateSelection(itemNumber, false, true);
    $('button img, .tabs img').show();
}

function onControllerDisconnect(){
    $('.selectedElement').removeClass('selectedElement');
    $('button img, .tabs img').hide();
}

function inputBox(type){
    $('#inputBox .textInput').hide();
    if(type=='playerName'){
       $('#pName').show();
       $('#inputBox .header').text('Player Name');
        dew.command('Player.Name', {}).then(function(response) {
            $('#inputBox #pName').val(response);
            $('#inputBox').fadeIn(100);
            activePage = activePage+'inputBox';
            $('#dismissButton').show();
            $('#pName').focus();
        });
    }else if(type=='serviceTag'){
       $('#sTag').show();
       $('#inputBox .header').text('Service Tag');
        dew.command('Player.ServiceTag', {}).then(function(response) {
            $('#inputBox #sTag').val(response);
            $('#inputBox').fadeIn(100);
            activePage = activePage+'inputBox';
            $('#dismissButton').show();
            $('#sTag').focus();
        });
    }
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
    itemNumber = $('#'+showMe+' span').index($('#'+showMe+' input:checked').parent().parent());
    updateSelection(itemNumber, false, true);
    $('#infoBox #infoHeader').text($('#'+showMe+' input:checked').parent()[0].innerText);
    $('#infoBox #infoText').text($('#'+showMe+' input:checked').attr('desc'));
}

function colorShow(showMe, element){
    activePage = '#page2 #'+showMe;
    $('.baseNav').removeClass('selectedElement');
    $(activePage+' .selectedElement').removeClass('selectedElement');
    element.addClass('selectedElement');
    $('.colorForm').hide();
    $('#'+showMe).css('display', 'grid')
    $('#infoBox').show();
    $.grep(settingsToLoad, function(result, index){
        if(result){
            if(result[0] == showMe){
                $('#infoBox #infoText').text(result[3]);
            }
        }
    });
    $('#infoBox #infoHeader').text($('#'+showMe+' input:checked').parent()[0].innerText);
    itemNumber = $('#'+showMe+' span').index($('#'+showMe+' input:checked').parent().parent());
    updateSelection(itemNumber, false, true);
}

function leftNav(){
    if(activePage.startsWith('#page2 #color') && itemNumber % 3 != 1){
         itemNumber--;
         updateSelection(itemNumber, true, true);
    }
}

function rightNav(){
    if(activePage.startsWith('#page2 #color')){
        if(itemNumber % 3 != 0){
             itemNumber++;
            updateSelection(itemNumber, true, true);
        }
    }
}

function selectElement(){
    if(activePage == location.hash){
        $(activePage+' .selectedElement').click();
    }else if($(activePage + ' form:visible')){
        $(activePage+' .selectedElement').find('input').click();
    }
    dew.command('Game.PlaySound 0x0B00');
}

function exitSubform(){
    if($(activePage + ' form:visible') && activePage != location.hash){
        $(activePage+' .selectedElement').removeClass('selectedElement');
        activePage = location.hash;
        itemNumber = $(activePage+' span').has('.setting').index($('span:has(.selectedElement)'));
        colorPicker = null;
        dew.command('Game.PlaySound 0x0B04');
    }
}

function leftPad(val, size, ch) {
    var result = String(val);
    if(!ch) {
        ch = " ";
    }
    while (result.length < size) {
        result = ch + result;
    }
    return result;
}

function getAspectRatio(){
    function gcd (a, b) {
        return (b == 0) ? a : gcd (b, a%b);
    }
    var w = screen.width;
    var h = screen.height;
    var r = gcd (w, h);
    return w/r+":"+h/r;
}

function adjustBiped(){
    if(getAspectRatio() == '4:3' || getAspectRatio() == '5:4' ){
        dew.command('Player.Armor.SetUiModelPosition 74.058 -101.826 11.65'); //moved to the left
    }else{
        dew.command('Player.Armor.SetUiModelPosition 74.108 -101.926 11.65'); //default
    }    
}