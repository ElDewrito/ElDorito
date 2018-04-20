var votingType = "voting";
var interval = 0;
var isHost;
var axisThreshold = .5;
var stickTicks = { left: 0, right: 0, up: 0, down: 0 };
var repGP;
var lastHeldUpdated = 0;
var itemNumber = 1;

var settingsArray = { 'Settings.Gamepad': '0' , 'Game.IconSet': '360'};
var compactMode = false;

dew.on("variable_update", function(e){
    for(i = 0; i < e.data.length; i++){
        if(e.data[i].name in settingsArray){
            settingsArray[e.data[i].name] = e.data[i].value;
        }
    }
});

function loadSettings(i){
	if (i != Object.keys(settingsArray).length) {
		dew.command(Object.keys(settingsArray)[i], {}).then(function(response) {
			settingsArray[Object.keys(settingsArray)[i]] = response;
			i++;
			loadSettings(i);
		});
	}
}

$(document).ready(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            hideScreen();
        }
    });
    $(document).keydown(function(e){
        if(e.keyCode == 192 || e.keyCode == 112 || e.keyCode == 223){
            dew.show('console');
        }
        if(e.keyCode == 38){ //Up
            upNav();
        }
        if(e.keyCode == 40){ //Down
            downNav();
        }
        if(e.keyCode == 13||e.keyCode == 32){ //Enter or Space to vote
            vote(itemNumber);
        }
        if(e.keyCode == 84 || e.keyCode == 89){ //Chat
            var teamChat = false;
            if(e.keyCode == 89){ teamChat = true };
            dew.show("chat", {'captureInput': true, 'teamChat': teamChat});
        }
        if(e.keyCode == 88 && votingType == "veto"){ //X to veto
            veto();
        }
        if(e.keyCode == 36){ //Home
            dew.show('settings');
        }
        if(e.keyCode == 9){ //Tab
            dew.show('scoreboard', {'locked':'true'});
        }
    });
    $('#scoreboardButton').off('click').on('click', function(){
        dew.show('scoreboard', {'locked':'true'});
    });
    $('#settingsButton').off('click').on('click', function(){
        dew.show('settings');
    });
    $('#closeButton').off('click').on('click', function(){
        hideScreen();
    });
    $('#vetoButton').off('click').on('click', function(){
        veto();
    });

    $('#showButton').off('click').on('click', function() {
        dew.command('Game.PlaySound 0xb02');
        compactMode = false;
        onShow();
        
    });
    loadSettings(0);
});

var isThrottled = false;
var throttleDuration = 1000;
function veto(){
    if (isThrottled) { return; }
    isThrottled = true;
    setTimeout(function () { isThrottled = false; }, throttleDuration);
    vote(1);
}

function hideScreen(){
    if(!compactMode) {
        dew.command('Game.PlaySound 0xb01');
    }
    compactMode = true;
    onShow();
}

function setTimer(amount, voting){
    clearInterval(interval);
    interval = setInterval(function() {
        var timerElement =   document.getElementById(compactMode ? 'timerCompact' : 'timer');
        if(voting){
            timerElement.innerHTML = " Voting ends in..." + --amount ;
        }else{
            timerElement.innerHTML = " Game starts in..." + --amount ;    
        }
        if (amount <= 0) {
            timerElement.innerHTML = "";
            clearInterval(interval);
        }
    }, 1000);
}

function vote(number) {
    dew.command("server.SubmitVote " + number);
	var WinnerChosen = document.getElementsByClassName('winner');
    if(votingType != "veto"){
        if (WinnerChosen.length <= 0) {
            $('.myVote').removeClass('myVote');
            $('.votingOption').eq(number-1).addClass("myVote");
        }
    }else{
        if($('.vetoBox:contains("Map")').length){
            $('.vetoBox').html('<a onclick="veto();"><img class="button">Voted to veto</a>');
        }else{
            $('.vetoBox').html('<a onclick="veto();"><img class="button">Veto Map and Game</a>'); 
        }
        $(".vetoBox img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_X.png');
    }
    if (WinnerChosen.length <= 0) {
        dew.command('Game.PlaySound 0x0B00');
    }
}

dew.on("show", function(event) {
    dew.getSessionInfo().then(function(i){
        isHost = i.isHost;
    });
    itemNumber = 1;
    initGamepad();


    if(event.data.userInvoked && compactMode && !event.data.compact) {
        dew.command('Game.PlaySound 0xb02');
    }
    compactMode = event.data.compact;
    onShow();
});

dew.on("hide", function(event) {
    if(repGP){
        window.clearInterval(repGP);
        repGP = null;
    }
});

dew.on("Winner", function(event) {
    $('#votingDesc').html('Your game will be starting shortly. <span id="timer"></span>');
    $("#" + event.data.Winner).addClass('winner');
	$(".votingOption").removeClass("selected");
    setTimer(event.data.timeUntilGameStart, false);  
});

dew.on("VetoOptionsUpdated", function(event) {
    $("#votingOptions").empty();
    var voting;
    if(event.data.vetoOption.canveto){
        votingType = "veto";
        voting = true;
        $('#vetoButton, #vetoCount').show();
        $('#votingDesc').html('Vote to veto game and map... <span id="timer"></span>');
        $("<div class='vetoBox'><a onclick='veto()'><img class='button'>Veto Map and Game</a></div>").appendTo($("#votingOptions"));
        $(".vetoBox img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_X.png');
    }else{
        votingType = "ended";
        voting = false;
        $('#vetoButton, #vetoCount').hide();
        $('#votingDesc').html('Your game will be starting shortly. <span id="timer"></span>');
    }

    $('.top').css('height', '294px');
    
    dew.command('Server.ListPlayersJSON').then(function(res){
        $('#voteCount').html('<span id="1"><span class="selector">0</span> of '+JSON.parse(res).length+'</span> ('+event.data.votesNeededToPass + ' needed)');
    });
    setTimer(event.data.timeRemaining - 1, voting);     
});

dew.on("VotingOptionsUpdated", function(event) {
    votingType = "voting";

    $('.top').css('height', '530px');
    $('#vetoButton, #vetoCount').hide();
    $('#votingDesc').html('Vote for game and map... <span id="timer"></span>');

    $('#votingOptions').empty();
    event.data.votingOptions.forEach(function(entry, i) {
        if (entry.mapname == "Revote") {
            $("<div>", {
                    "class": "votingOption none",
                    "id": entry.index
                })
                .html('<span class="votebox"></span>None of the above<span class="selector">0</span>')
                .appendTo($("#votingOptions"));
        } else if (entry.mapname != '') {
            $("<div>", {
                    "class": "votingOption",
                    "id": entry.index
                })
                .html('<span class="votebox"></span><img class="mapImage" src="dew://assets/maps/small/'+entry.image+'.png"><span class="gameType">'+entry.typename+'</span><span class="mapName">'+entry.mapname+'</span><span class="selector">0</span>')
                .appendTo($("#votingOptions"));
        }
    });

    itemNumber = 1;
    updateSelection(itemNumber, false);
    
    $(".votingOption").off('click').on('click', function() {
        $(".votingOption").removeClass("selected");
		var WinnerChosen = document.getElementsByClassName('winner');
		if (WinnerChosen.length <= 0) {
			$(this).addClass( "selected" );
		}
        vote($(this).attr('id'));
    });    
    
    $(".votingOption").off('mouseover').on('mouseover', function(){
        if (!$('.winner').length) {
            itemNumber = $('.votingOption').index($(this))+1;
            updateSelection(itemNumber, false);
        }
    });
    setTimer(event.data.timeRemaining - 1, true); 
});

dew.on("VoteCountsUpdated", function(event) {
    event.data.voteCounts.forEach(function(entry, i) {
        if (entry.Count == 0){
            $('#' + entry.OptionIndex + ' .selector').text(0);
        }else{
            $('#' + entry.OptionIndex + ' .selector').text(entry.Count);
        }
    });
});

dew.on('VoteEnded', function(event) {
    clearInterval(interval);
    compactMode = false;
});

dew.on('controllerinput', function(e){       
    if(settingsArray['Settings.Gamepad'] == 1){
        if(e.data.A == 1){
            vote(itemNumber);
        }
        if(e.data.Select == 1){
            dew.show('scoreboard',{'locked':true});
        }
        if(e.data.Up == 1){
            upNav();
        }
        if(e.data.Down == 1){
            downNav();
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
        if(e.data.X == 1 && votingType == "veto"){
            veto();
        }            
        if(e.data.B == 1){
            hideScreen();
        }
    }
});

function initGamepad(){
    $("#scoreboardButton img").attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_Back.png');  
    $("#settingsButton img").attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_Start.png');  
    $("#vetoButton img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_X.png');
    $("#closeButton img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_B.png');
    $(".vetoBox img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_X.png');
    if(settingsArray['Settings.Gamepad'] == 1){
        if(!repGP){
            repGP = window.setInterval(checkGamepad,1000/60);
        }
    }else{
        if(repGP){
            window.clearInterval(repGP);
            repGP = null;
        }
    }
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
};

function updateSelection(item, sound){
    $('.selected').removeClass('selected');
    var WinnerChosen = document.getElementsByClassName('winner');
    if (WinnerChosen.length <= 0) {
        $('#'+item).addClass('selected');
        if(sound){
            dew.command('Game.PlaySound 0xAFE');
        }
    }
}

function upNav(){
    if(itemNumber > 1){
        itemNumber--;
        updateSelection(itemNumber, true);
    }
}

function downNav(){
    if(itemNumber < $('.votingOption').length){
        itemNumber++;
        updateSelection(itemNumber, true);
    }           
}

function onShow() {
    if(compactMode) {
        $('#window').fadeOut('fast');
        $('.window-compact').fadeIn('fast');
        dew.captureInput(false);
        dew.capturePointer(true);
    } else {
        $('#window').fadeIn('fast');
        $('.window-compact').fadeOut('fast');
        dew.captureInput(true);
        dew.capturePointer(true);
    }
}

function cancelVote() {
    if(isHost) {
        dew.command("server.CancelVote");
    }
}