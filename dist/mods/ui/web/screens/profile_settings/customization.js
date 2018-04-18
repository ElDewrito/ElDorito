var activePage = "";
var itemNumber = 0;
var tabIndex = 0;
var hasGP = false;
var axisThreshold = .5;
var stickTicks = { left: 0, right: 0, up: 0, down: 0 };
var repGP;
var lastHeldUpdated = 0;
var changeEmblemAPI = '/api/changeemblem';
var getEmblemAPI = '/api/getemblem';
var apiServer = 'new.halostats.click';
var emblemGeneratorAPI = "/emblem/emblem.php";
var hasValidConnection = true;
var lastEmblem = "";
var emblemToggle = 1;
var needApply = false;

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
    ['playerName', 'Player.Name','Name','', 2],
    ['serviceTag', 'Player.ServiceTag','Service Tag','', 3],
    ['armorHelmet', 'Player.Armor.Helmet','Helmet','The thing that goes on your head.', 2],
    ['armorChest', 'Player.Armor.Chest','Body','From arm to arm.', 3],
    ['armorRightShoulder', 'Player.Armor.RightShoulder','Right Shoulder','Right there on that shoulder.', 4],
    ['armorLeftShoulder', 'Player.Armor.LeftShoulder','Left Shoulder','The only shoulder that\'s left.', 5],
    ['gender', 'Player.Gender','Gender','', 6],
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
var colorPicker;
var genderList = [
    ['Male','male','State your gender. This will not be displayed to other players, but combat effects will be tailored to your gender.'],
    ['Female','female','State your gender. This will not be displayed to other players, but combat effects will be tailored to your gender.']    
];

$(document).ready(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            if(activePage.endsWith('inputBox')){
                dismissButton();
            }else{
                if(activePage.includes(" ")){
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
        if(e.keyCode == 37 && !(activePage.startsWith('#page2 #color') || activePage.startsWith('#page3 #color'))){ //Left
            leftNav();
        }
        if(e.keyCode == 38 && !(activePage.startsWith('#page2 #color') || activePage.startsWith('#page3 #color'))){ //Up
            upNav();
        }
        if(e.keyCode == 39 && !(activePage.startsWith('#page2 #color') || activePage.startsWith('#page3 #color'))){ //Right
            rightNav();
        }
        if(e.keyCode == 40 && !(activePage.startsWith('#page2 #color') || activePage.startsWith('#page3 #color'))){ //Down
            downNav();
        }
        if(!activePage.endsWith('inputBox')){
            if(e.keyCode == 32 || e.keyCode == 13){ //Space and Enter
                selectElement();
            }
            if(e.keyCode == 81){//Q
                prevPage();
            }
            if(e.keyCode == 69){//E
                nextPage();
            }
        }else{
            if(e.keyCode == 13){ //Enter to submit inputBox
                $('#inputBox #okButton').click();
            }
        }
    });
	$("#randomColors").hide();
	SetupEmblems(false, true, true);
		
    setRadioList('armorHelmet', armorHelmetList, true);
    setRadioList('armorChest', armorChestList, true);
    setRadioList('armorRightShoulder', armorShoulderList, true);
    setRadioList('armorLeftShoulder', armorShoulderList, true);
    setRadioList('gender', genderList, true);
    setRadioList('colorsPrimary', h3ColorArray);
    setRadioList('colorsSecondary', h3ColorArray);
    setRadioList('colorsVisor', h3ColorArray);
    setRadioList('colorsLights', h3ColorArray);
    $('.tabs li a').off('click').on('click', function(e){
        $('.tabs li').removeClass('selected');
        $(this).parent().addClass('selected');
        window.location.href = $(this).attr('href');
        activePage = $(this).prop('hash');
        itemNumber = 0;
        $(e).ready(function(){
            updateSelection(itemNumber, false, true);
            tabIndex = $('.tabs li:visible a').index($("a[href='"+activePage+"']"));
        });
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
    $('.colorForm input, .armorForm input').off('click').on('change click', function(e){
        $(this).parent().parent().parent().find('.chosenElement').removeClass('chosenElement');
        $(this).parent().parent().addClass('chosenElement');
        $.grep(settingsToLoad, function(result){
            if(result[0] == e.target.name){
                dew.command(result[1]+' '+e.target.value);
                $(location.hash+' #infoBox #infoText').text(result[3]);
            };
            $(location.hash+' #infoBox #infoHeader').text(e.target.computedName);
        });
        $(location.hash+' #infoBox #infoText').text($(this).attr('desc'));
		setUrl(false);
    });
    $('#colorsPrimaryText, #colorsSecondaryText,#colorsVisorText,#colorsLightsText').off('click').on('click', function(e){
        $('.colorForm').hide();
        colorPicker = dew.makeColorPicker(document.querySelector('#colorPicker'));
        var whichColor = $(this);
        $(this).prev()[0].checked = true;
        var currentVal = ColorUtil.hexToHsv($(this).val().split('#')[1]);
        activePage = location.hash+" #colorPicker";
        $('#colorPicker').show();
        $(location.hash+' #infoBox #infoHeader').text($(this).val());
        colorPicker.setColor(currentVal);
        colorPicker.on('select', function(color) {
            var currentSelection = ColorUtil.hsvToRgb(color.h, color.s, color.v);
            whichColor.val('#'+leftPad(ColorUtil.rgbToHex(currentSelection[0],currentSelection[1],currentSelection[2]),6,'0'));
            whichColor.trigger('change');
            $(location.hash+' #infoBox #infoHeader').text(whichColor.val());
        });
    });
    $('.colorForm, .armorForm').submit(function() {
        return false;
    });
    $('#applyButton').off('click').on('click', function(e){
        applyButton();
    });
    $('#cancelButton').off('click').on('click', function(e){
        cancelButton();
    });
    setControlValues();
	var hasPressed = false;
    dew.on('controllerinput', function(e){
        if(hasGP){
            if(e.data.A == 1){
                if(activePage.endsWith('inputBox')){
                    dew.command('Player.Name '+$('#inputBox input').val());
                    dew.notify("settings-update", [['Player.Name',$('#inputBox input').val()]]);
                    dismissButton();
                }else{
                    selectElement();
                }
            }
            if(e.data.B == 1){
                if(activePage.endsWith('inputBox')){
                    dismissButton();
                }else if(activePage.includes(" ")){
                    exitSubform();
                }else{
                    cancelButton();
                }
            }
            if(e.data.X == 1){
                if(activePage.startsWith('#page3')){
					toggleIcon();
				}
            }
			if(e.data.Y == 1){
				if(activePage.startsWith('#page1')){
                    randomArmor();
                }else if(activePage.startsWith('#page2')){
                    randomColors();
                }else if(activePage.startsWith('#page3')){
					randomEmblem();
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
				if(activePage.startsWith('#page3')){
					if(!hasPressed){
						if(itemNumber - 5 >= 0) {
							itemNumber -= 5;
							updateSelection(itemNumber, true, true);
						}else{
							itemNumber = 0;
							updateSelection(itemNumber, true, true);
						}
						hasPressed = true;
					}
				}else
                if(itemNumber > 0){
                    itemNumber = 0;
                    updateSelection(itemNumber, true, true);
                }
            }else
            if(e.data.RightTrigger != 0){
				if(activePage.startsWith('#page3')){
					if(!hasPressed){
						if(itemNumber+5 <= $(activePage + ' label:visible').length-1){
							itemNumber += 5;
							updateSelection(itemNumber, true, true);
						}else{
							itemNumber = $(activePage + ' label:visible').length-1;
							updateSelection(itemNumber, true, true);
						}
						hasPressed = true;
					}
				}else
                if(itemNumber < $(activePage + ' label:visible').length-1){
                    itemNumber = $(activePage + ' label:visible').length-1;
                    updateSelection(itemNumber, true, true);
                }
            }else{
				hasPressed = false;
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
        itemNumber = $(activePage+' span').has('.setting').index($(this));
        updateSelection(itemNumber, false, false);
    });
    $('span').has('.setting').mouseout(function(){
        if(!hasGP){
            $(this).removeClass('selectedElement');
        }
    }); 
    $('#inputBox #okButton').off('click').on('click', function(){
        if($('#inputBox #pName').is(':visible')){
            dew.command('Player.Name "'+$('#inputBox #pName').val()+'"');
            dew.notify("settings-update", [['Player.Name',$('#inputBox #pName').val()]]);
			if(hasValidConnection){
				SetupEmblems(false, false, false, function(){
					ApplyEmblem(false);
				},true);
			}
        }else if($('#inputBox #sTag').is(':visible')){
            dew.command('Player.ServiceTag "'+$('#inputBox #sTag').val().toUpperCase()+'"');
        }
        hideInputBox(true);
    });
    $('#inputBox #dismissButton').off('click').on('click', function(){
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
        $('#randomArmor img').attr('src','dew://assets/buttons/' + response + '_Y.png');
        $('#randomColors img').attr('src','dew://assets/buttons/' + response + '_Y.png');
        $('#applyButton img').attr('src','dew://assets/buttons/' + response + '_Start.png');
        $('#cancelButton img').attr('src','dew://assets/buttons/' + response + '_B.png');
		$('#toggleIconButton img').attr('src','dew://assets/buttons/'+ response + '_X.png');
		$('#randomEmblem img').attr('src','dew://assets/buttons/'+ response + '_Y.png');
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
    $('.armorForm, .colorForm, .emblemForm, .emblemColorForm').hide();
    $('#infoHeader, #infoText').text('');
    $('#infoBox').hide();
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
	$("#toggleButton").hide();
}

function cancelButton(){
    itemNumber = 0;
    dew.command('writeconfig');
    effectReset();
	page1();
	if(needApply){
		ApplyEmblem(false);
	}
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
		$options.eq(random).click();
		itemNumber = random;
		updateSelection(itemNumber,false,true, '#page1 #' + armorArray[i]);
        $.grep(settingsToLoad, function(result){
            if(result[0] == armorArray[i]){
                dew.command(result[1] + ' ' + $('#'+armorArray[i]+' input:checked').val());
            };
        });
    }
}

function randomEmblem(){
    var emblemArray = ['emblemIcon','emblemBackgroundImage',];
    for(var i = 0; i < emblemArray.length; i++) {
        var $options = $('#'+emblemArray[i]).find('input'),
            random = ~~(Math.random() * $options.length);
        $options.eq(random).prop('checked', true);
		$options.eq(random).click();
		itemNumber = random;
		updateSelection(itemNumber,false,true, '#page3 #' + emblemArray[i]);
    }
	var colorArray = ['colorsEmblemPrimary','colorsEmblemSecondary','colorsEmblemImage'];
    for(var i = 0; i < colorArray.length; i++) {
        var random = ~~(Math.random() * 30);
		$('#'+colorArray[i]+' input').eq(random).prop('checked', true);
		$('#'+colorArray[i]+' input').eq(random).click();
		itemNumber = random;
		updateSelection(itemNumber,false,true, '#page3 #' + colorArray[i]);
		
    }
   	setUrl(false);
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

function updateSelection(item, sound, move, direct){
	var elem = (direct ? direct : activePage);

    if(item > -1){
        $(elem+' .selectedElement').eq(0).removeClass('selectedElement');
        $(elem + ' label:visible').eq(item).parent().addClass('selectedElement');
        if($(elem+' .selectedElement').length && move){
            $(elem+' .selectedElement')[0].scrollIntoView(false);
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
	if(activePage.startsWith('#page3 #color')){
		if(itemNumber >= 3){
            itemNumber-=3;
            updateSelection(itemNumber, true, true);
        }  
	}else
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
	if(activePage.startsWith('#page3 #color')){
		if(itemNumber < $(activePage + ' label:visible').length-3){
			itemNumber+=3;
            updateSelection(itemNumber, true, true);
        }
	}else
    if(activePage.startsWith('#page2 #color') ){
        if(itemNumber < $(activePage + ' label:visible').length-3){
            if(itemNumber == 0){
                itemNumber+=1;
            }else{
                itemNumber+=3;
            }
            updateSelection(itemNumber, true, true);
        }
    }else{
        if((activePage.split(' ').length < 2 && itemNumber < 3 && activePage == '#page2') || ((activePage.split(' ').length < 2 && itemNumber < 5 && activePage == '#page3')) || (activePage.split(' ').length < 2 && itemNumber < 6 && activePage == '#page1') ||  (activePage.split(' ').length > 1 && itemNumber < $(activePage + ' label:visible').length-1)){
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
            $('#inputBox #pName').val(response.substring(0, 15));
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
    itemNumber = $('#'+showMe+' span').index($('#'+showMe+' input:checked').parent().parent());
    updateSelection(itemNumber, false, true);
    $(location.hash+' #infoBox #infoHeader').text($('#'+showMe+' input:checked').parent()[0].innerText);
    $(location.hash+' #infoBox #infoText').text($('#'+showMe+' input:checked').attr('desc'));
    $(location.hash+' #infoBox').show();
}

function colorShow(showMe, element){
    activePage = '#page2 #'+showMe;
    $('.baseNav').removeClass('selectedElement');
    $(activePage+' .selectedElement').removeClass('selectedElement');
    element.addClass('selectedElement');
    $('.colorForm').hide();
    $('#'+showMe).css('display', 'grid')
    $.grep(settingsToLoad, function(result, index){
        if(result){
            if(result[0] == showMe){
                $(location.hash+' #infoBox #infoText').text(result[3]);
            }
        }
    });
    $(location.hash+' #infoBox #infoHeader').text($('#'+showMe+' input:checked').parent()[0].innerText);
    $(location.hash+' #infoBox').show();
    itemNumber = $('#'+showMe+' span').index($('#'+showMe+' input:checked').parent().parent());
    updateSelection(itemNumber, false, true);
}

function leftNav(){
	if(activePage.startsWith('#page3 #color') ){
		if(itemNumber % 3 != 0){
			itemNumber--;
			updateSelection(itemNumber, true, true);
		}
	}
    if((activePage.startsWith('#page2 #color') && itemNumber % 3 != 1)){
         itemNumber--;
         updateSelection(itemNumber, true, true);
    }
}

function rightNav(){
	if(activePage.startsWith('#page3 #color')){
		if(itemNumber % 3 != 2){
			itemNumber++;
			updateSelection(itemNumber, true, true);
		}
		
	}
    if(activePage.startsWith('#page2 #color') ){
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
        $(activePage + ' form:visible').hide();
        $(activePage + ' #infoBox').hide();
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

function SetupEmblems(resetEmblemList, setRadiosLists, setEmblem, onFinish, runFinish){
	$("#toggleIconButton").hide();
	$("#randomEmblem").hide();
	
	ping(apiServer, function(response){
		if(response == 'timeout'){
			var elem = document.getElementById("EmblemTabLink").href = "#unavailable";
			hasValidConnection = false;
		}else{
			var elem = document.getElementById("EmblemTabLink").href = "#page3";
						
			var jsonObj = new Object();
			dew.command("Player.Name").then(function (name){jsonObj.playerName = name;});
			dew.command("Player.ServiceTag").then(function (srvtag){jsonObj.serviceTag = srvtag;});
			dew.command("Player.PrintUID").then(function (uid) {jsonObj.uid = uid.substr(14);});
			dew.command("Player.PubKey").then(function (pubkey){jsonObj.publicKey = pubkey});
			dew.command("Player.EncryptGMTTimestamp").then(function (encryptedVal) {
				jsonObj.encryptedTimestamp = encryptedVal;
				
				$.ajax({
				contentType: 'application/json',
				data: JSON.stringify(jsonObj),
				dataType: 'json',
				success: function(data){
					var embList = JSON.parse(data.emblemList);
					if(resetEmblemList){
						setEmblemRadioList('emblemIcon',embList.emblemList, true, true);
						setEmblemRadioList('emblemBackgroundImage', embList.backgroundEmblems, true, true);
						setEmblemColorRadioList('colorsEmblemPrimary', h3ColorArray,true);
						setEmblemColorRadioList('colorsEmblemSecondary', h3ColorArray,true);
						setEmblemColorRadioList('colorsEmblemImage', h3ColorArray,true);
					}
					
					if(setRadiosLists){
						setEmblemRadioList('emblemIcon',embList.emblemList,true, false);
						setEmblemRadioList('emblemBackgroundImage', embList.backgroundEmblems,true, false);
						setEmblemColorRadioList('colorsEmblemPrimary', h3ColorArray);
						setEmblemColorRadioList('colorsEmblemSecondary', h3ColorArray);
						setEmblemColorRadioList('colorsEmblemImage', h3ColorArray);
					}
					
					$('#emblemIcon input, #emblemBackgroundImage input, #colorsEmblemPrimary input, #colorsEmblemSecondary input, #colorsEmblemImage input, #colorsEmblemBackground input').off('click').on('change click', function(e) {
						needApply = true;
					});
					
					$('.emblemForm input, .emblemColorForm input').off('click').on('change click', function(e){
						$(this).parent().parent().parent().find('.selectedElement').removeClass('selectedElement');
						$(this).parent().parent().parent().find('.chosenElement').removeClass('chosenElement');
						$(this).parent().parent().addClass('chosenElement');
						setUrl(false);
					});
									
					$('.emblemForm, .emblemColorForm').submit(function() {
						return false;
					});
					
					$('.emblemForm, .emblemColorForm').unbind("mouseover");
					$('span').has('.setting').unbind("mouseover");
					$('span').has('.setting').unbind("mouseout");
					
					$('.emblemForm, .emblemColorForm').mouseover(function(){
						activePage = location.hash+' #'+$(this).attr('id');
					});
                    $('span').has('.setting').mouseover(function(){
                        itemNumber = $(activePage+' span').has('.setting').index($(this));
                        updateSelection(itemNumber, false, false);
                    });
					
					$('span').has('.setting').mouseout(function(){
						if(!hasGP){
							$(this).removeClass('selectedElement');
						}
					}); 
					
					if(setEmblem){
						setItemValues('emblemIcon', embList.emblemList[getProperIndex(parseInt(getQueryVariable(data.emblem,'fi')),embList.emblemList)][1]);
						setItemValues('emblemBackgroundImage', embList.backgroundEmblems[getProperIndex(parseInt(getQueryVariable(data.emblem,'bi')),embList.backgroundEmblems)][1]);
						setItemValues('colorsEmblemPrimary', h3ColorArray[parseInt(getQueryVariable(data.emblem,'3'))][1]);
						setItemValues('colorsEmblemSecondary', h3ColorArray[parseInt(getQueryVariable(data.emblem,'2'))][1]);
						setItemValues('colorsEmblemImage', h3ColorArray[parseInt(getQueryVariable(data.emblem,'1'))][1]);
						emblemToggle = parseInt(getQueryVariable(data.emblem,'fl'));
						setUrl(true);
					}
					if(runFinish)
					onFinish();
				},
				error: function(){		
					var elem = document.getElementById("EmblemTabLink").href = "#unavailable";
					hasValidConnection = false;
				},
				type: 'POST',
				url: 'http://' + apiServer + getEmblemAPI
				});			
			});
		}
	});
}

function setItemValues(item, value){
	$('#'+item+' :radio[value=""]').attr('checked',true);
	$('#'+item+' :radio[value="'+value+'"]').attr('checked',true);
	if( $('#'+item+' :radio[value="'+value+'"]').length){
		$('#'+item+' :radio[value="'+value+'"]').parent().parent().addClass('chosenElement');
	}else{
		$('#'+item+' :radio[value=""]').parent().parent().addClass('chosenElement');
	}
	$('#'+item+'Text').val(value);

}

function getProperIndex(emblem, array){
	for(var x = 0; x < array.length; x++){
		var value = array[x][1];
		if(value == emblem){
			return x;
		}
	}
	return 0;
}

function getQueryVariable(url, variable) {
    var vars = url.split('&');
    for (var i = 0; i < vars.length; i++) {
        var pair = vars[i].split('=');
        if (decodeURIComponent(pair[0]) == variable) {
            return decodeURIComponent(pair[1]);
        }
    }
}

function setEmblemRadioList(ElementID, Json, hasImage, shouldReset){
    var sel = document.getElementById(ElementID);
	
	if(shouldReset){
		while(sel.firstChild){
			sel.removeChild(sel.firstChild);
		}
	}
	
    for(var i = 0; i < Json.length; i++){
        var span = document.createElement("span");
        var label = document.createElement("label");
        var radio = document.createElement("input");
        radio.setAttribute('type', 'radio');
        radio.setAttribute('name', ElementID);
        radio.setAttribute('class', 'setting');
        if(Json[i][2]){
            radio.setAttribute('desc', Json[i][2]);
        }
        radio.value = Json[i][1];
        label.appendChild(radio);
        label.appendChild(document.createTextNode(Json[i][0]));
        span.appendChild(label);
        if(hasImage){
            var img = document.createElement("img");
            img.setAttribute('src', Json[i][2]);
            span.appendChild(img);
        }
        sel.appendChild(span);
    }
}

function setEmblemColorRadioList(ElementID, ArrayVar, shouldReset){
    var sel = document.getElementById(ElementID);
	
	if(shouldReset){
		while(sel.firstChild){
			sel.removeChild(sel.firstChild);
		}
	}
	
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

function emblemColorShow(showMe, element){
    activePage = '#page3 #'+showMe;
    $('.baseNav').removeClass('selectedElement');
    $(activePage+' .selectedElement').removeClass('selectedElement');
    element.addClass('selectedElement');
    $('.emblemColorForm').hide();
	$('.emblemForm').hide();
    $('#'+showMe).css('display', 'grid')
    itemNumber = $('#'+showMe+' span').index($('#'+showMe+' input:checked').parent().parent());
    updateSelection(itemNumber, false, true);
}

function emblemShow(showMe, element){
    activePage = '#page3 #'+showMe;
    $('.baseNav').removeClass('selectedElement');
    $(activePage+' .selectedElement').removeClass('selectedElement');
    element.addClass('selectedElement');
    $('.emblemForm').hide();
	$('.emblemColorForm').hide();
    $('#'+showMe).show();
    itemNumber = $('#'+showMe+' span').index($('#'+showMe+' input:checked').parent().parent());
    updateSelection(itemNumber, false, true);
}

function setUrl(isLastEmblem){
	var primary = $('#colorsEmblemPrimary span').index($('#colorsEmblemPrimary input:checked').parent().parent());
	var secondary = $('#colorsEmblemSecondary span').index($('#colorsEmblemSecondary input:checked').parent().parent());
	var imageb = $('#colorsEmblemImage span').index($('#colorsEmblemImage input:checked').parent().parent());
	var background = $('#colorsEmblemBackground span').index($('#colorsEmblemBackground input:checked').parent().parent());
	var icon = $('#emblemIcon input:checked').val();
	var backgroundimg = $('#emblemBackgroundImage span').index($('#emblemBackgroundImage input:checked').parent().parent());
	var toggle = emblemToggle;

	var emblemurl =  "?s=100&0=" + (background < 0 ? 0 : background) + 
	"&1=" + (imageb < 0 ? 0 : imageb) + 
	"&2=" + (secondary < 0 ? 0 : secondary) + 
	"&3=" + (primary < 0 ? 0 : primary) + 
	"&fi=" + (icon == undefined ? 0 : icon) + 
	"&bi=" + (backgroundimg < 0 ? 0 : backgroundimg) + 
	"&fl=" + (toggle < 0 ? 0 : toggle) + 
	"&m=1";

	if(isLastEmblem){
		lastEmblem = emblemurl;
	}
	if(lastEmblem == emblemurl){
		needApply = false;
	}
	
	document.getElementById("emblemBox").src = 'http://' + apiServer + emblemGeneratorAPI + emblemurl;
}

function ApplyEmblem(ShowAlert) {
	var primary = $('#colorsEmblemPrimary span').index($('#colorsEmblemPrimary input:checked').parent().parent());
	var secondary = $('#colorsEmblemSecondary span').index($('#colorsEmblemSecondary input:checked').parent().parent());
	var imageb = $('#colorsEmblemImage span').index($('#colorsEmblemImage input:checked').parent().parent());
	var background = $('#colorsEmblemBackground span').index($('#colorsEmblemBackground input:checked').parent().parent());
	var icon = $('#emblemIcon input:checked').val();
	var backgroundimg = $('#emblemBackgroundImage span').index($('#emblemBackgroundImage input:checked').parent().parent());
	var toggle = emblemToggle;
	
	var jsonObj = new Object();
	dew.command("Player.Name").then(function (name){jsonObj.playerName = name;});
	dew.command("Player.ServiceTag").then(function (srvtag){jsonObj.serviceTag = srvtag;});
	dew.command("Player.PrintUID").then(function (uid) {jsonObj.uid = uid.substr(14);});
	dew.command("Player.PubKey").then(function (pubkey){jsonObj.publicKey = pubkey});
	dew.command("Player.EncryptGMTTimestamp").then(function (encryptedVal) {
		jsonObj.encryptedTimestamp = encryptedVal;
		
		var emblemObj = new Object();
		emblemObj.s = 100;
		emblemObj.zero = (background < 0 ? 0 : background);
		emblemObj.one = (imageb < 0 ? 0 : imageb);
		emblemObj.two = (secondary < 0 ? 0 : secondary);
		emblemObj.three = (primary < 0 ? 0 : primary);
		emblemObj.fi = (icon == undefined ? 0 : icon);
		emblemObj.bi = (backgroundimg < 0 ? 0 : backgroundimg);
		emblemObj.fl = (toggle < 0 ? 0 : toggle);
		emblemObj.m = 1;
	
		jsonObj.emblem = emblemObj;
		
		$.ajax({
		contentType: 'application/json',
		data: JSON.stringify(jsonObj),
		success: function(data){
			if(ShowAlert){
				dew.show('alert', {
							icon: 0,
							title: "Success",
							body: "Your emblem has been successfully changed"
				});
			}
			setUrl(true);
		},
		error: function(data){
			if(ShowAlert){
				dew.show('alert', {
							icon: 0,
							title: "Failed",
							body: "Your emblem has failed to change. Please check your internet connection"
				});
			}
		},
		type: 'POST',
		url: 'http://' + apiServer + changeEmblemAPI
		});			
	});
}

function toggleIcon(){
	emblemToggle = 1 - emblemToggle;
	needApply = true;
	setUrl(false);
}

function page3(){
	SetupEmblems(true, false, true);
	$("#toggleIconButton").show();
	$("#randomArmor").hide();
	$("#randomColors").hide();
	$("#randomEmblem").show();
}
function page2(){
	$("#toggleIconButton").hide();
	$("#randomArmor").hide();
	$("#randomColors").show();
	$("#randomEmblem").hide();
	if(needApply){
		ApplyEmblem(false);
	}
}
function page1(){
	$("#toggleIconButton").hide();
	$("#randomArmor").show();
	$("#randomColors").hide();
	$("#randomEmblem").hide();
	if(needApply){
		ApplyEmblem(false);
	}
}

function ping(ip, callback) {
    if (!this.inUse) {
        this.status = 'unchecked';
        this.inUse = true;
        this.callback = callback;
        this.ip = ip;
        var _that = this;
        this.img = new Image();
        this.img.onload = function () {
            _that.inUse = false;
            _that.callback('responded');

        };
        this.img.onerror = function (e) {
            if (_that.inUse) {
                _that.inUse = false;
                _that.callback('responded', e);
            }

        };
        this.start = new Date().getTime();
        this.img.src = "http://" + ip;
        this.timer = setTimeout(function () {
            if (_that.inUse) {
                _that.inUse = false;
                _that.callback('timeout');
            }
        }, 1500);
    }
}