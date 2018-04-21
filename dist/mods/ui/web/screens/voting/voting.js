var votingType = "voting";
var interval = 0;
var seconds_left;
var isHost;
var axisThreshold = .5;
var stickTicks = { left: 0, right: 0, up: 0, down: 0 };
var repGP;
var lastHeldUpdated = 0;
var itemNumber = 1;

var settingsArray = { 'Settings.Gamepad': '0' , 'Game.IconSet': '360'};

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
            vote(1);
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
        vote(1);
    });
    loadSettings(0);
});

function hideScreen(){
    if(isHost){
        dew.command("server.CancelVote");
    }else{
        dew.gameaction(11);	
    }
    dew.hide(); 
}

function vote(number) {
    dew.command("server.SubmitVote " + number);
    if(votingType != "veto"){
        var WinnerChosen = document.getElementsByClassName('winner');
        if (WinnerChosen.length <= 0) {
            $('.myVote').removeClass('myVote');
            $('.votingOption').eq(number-1).addClass("myVote");
        }
    }else{
        if($('.vetoBox:contains("Map")').length){
            $('.vetoBox').html('<img class="button">Voted to veto');
        }else{
            $('.vetoBox').html('<img class="button">Veto Map and Game'); 
        }
        if(settingsArray['Settings.Gamepad'] == 1){
            $(".vetoBox img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_X.png');
            $('.button').show();
        }
    }
}

dew.on("show", function(event) {
    dew.getSessionInfo().then(function(i){
        isHost = i.isHost;
        if(isHost){
            $('#closeButton').html('<img class="button">Close');
        }else{
            $('#closeButton').html('<img class="button">Leave');
        }
        if(settingsArray['Settings.Gamepad'] == 1){
            $("#closeButton img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_B.png');
            $('.button').show();
        }
    });
    itemNumber = 1;
    initGamepad();
});

dew.on("hide", function(event) {
    clearInterval(interval);
    if(repGP){
        window.clearInterval(repGP);
        repGP = null;
    }
});

dew.on("Winner", function(event) {
    $('#votingDesc').html('Game starts in: <span id="timer"></span>');
    clearInterval(interval);
    seconds_left = event.data.timeUntilGameStart + 1;
    interval = setInterval(function() {
        document.getElementById('timer').innerHTML = --seconds_left + " seconds";
        if (seconds_left <= 0) {
            document.getElementById('timer').innerHTML = "";
            clearInterval(interval);
        }
    }, 1000);
    $("#" + event.data.Winner).addClass('winner');
	$(".votingOption").removeClass("selected");
});

dew.on("VetoOptionsUpdated", function(event) {
    $("#votingOptions").empty();
    if(event.data.vetoOption.canveto){
        votingType = "veto";
        $('#vetoButton, #vetoCount').show();
        $('#votingDesc').html('Voting round ends in: <span id="timer"></span>');
        $('#description').text('A new game and map will be chosen with a majority veto.');
        $("<div class='vetoBox'><img class='button'>Veto Map and Game</div>").appendTo($("#votingOptions"));
        if(settingsArray['Settings.Gamepad'] == 1){
            $(".vetoBox img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_X.png');
            $('.button').show();
        }
    }else{
        votingType = "ended";
        $('#vetoButton, #vetoCount').hide();
        $('#votingDesc').html('Game starts in: <span id="timer"></span>');
        $('#description').text('Your game will be starting shortly.');
    }
    clearInterval(interval);

    $('.top').css('height', '294px');
     
    seconds_left = event.data.timeRemaining -1;
    interval = setInterval(function() {
        document.getElementById('timer').innerHTML = --seconds_left + " seconds";
        if (seconds_left <= 0) {
            document.getElementById('timer').innerHTML = "";
            clearInterval(interval);
        }
    }, 1000);
    
    
    
    dew.command('Server.ListPlayersJSON').then(function(res){
        $('#voteCount').html('<span id="1"><span class="selector">0</span> of '+JSON.parse(res).length+'</span> ('+event.data.votesNeededToPass + ' needed)');  
    });
});

dew.on("VotingOptionsUpdated", function(event) {
    votingType = "voting";
    clearInterval(interval);

    $('.top').css('height', '530px');
    $('#description').text('Vote for game and map...');
    $('#vetoButton, #vetoCount').hide();
    $('#votingDesc').html('Voting round ends in: <span id="timer"></span>');

    $('#votingOptions').empty();
    event.data.votingOptions.forEach(function(entry, i) {
        if (entry.mapname == "Revote") {
            $("<div>", {
                    "class": "votingOption none",
                    "id": entry.index
                })
                .html('NONE OF THE ABOVE<span class="selector">0</span><span class="votebox" style="top: 11px;"></span><span class="vote" style="top: 23px;"></span>')
                .appendTo($("#votingOptions"));
        } else if (entry.mapname != '') {
            $("<div>", {
                    "class": "votingOption",
                    "id": entry.index
                })
                .html('<img class="mapImage" src="dew://assets/maps/small/'+entry.image+'.png"><span class="gameType">'+entry.typename+'</span><span class="mapName">'+entry.mapname+'</span><span class="selector">0</span><span class="votebox"></span><span class="vote"></span>')
                .appendTo($("#votingOptions"));
        }
    });

    itemNumber = 1;
    updateSelection(itemNumber, false);
    
    seconds_left = event.data.timeRemaining-1;
    interval = setInterval(function() {
        document.getElementById('timer').innerHTML = --seconds_left + " seconds";
        if (seconds_left <= 0) {
            document.getElementById('timer').innerHTML = "";
            clearInterval(interval);
        }
    }, 1000);

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

dew.on('controllerinput', function(e){       
    if(settingsArray['Settings.Gamepad'] == 1){
        if(e.data.A == 1){
            vote(itemNumber);
        }
        if(e.data.Select == 1){
            dew.show('scoreboard',{'locked':true});
        }
        if(e.data.Start == 1){
            dew.show('settings');
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
            vote(1);
        }            
        if(e.data.B == 1){
            hideScreen();
        }
    }
});

function initGamepad(){
    if(settingsArray['Settings.Gamepad'] == 1){
        $("#scoreboardButton img").attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_Back.png');  
        $("#settingsButton img").attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_Start.png');  
        $("#vetoButton img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_X.png');
        $("#closeButton img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_B.png');
        $(".vetoBox img").attr('src','dew://assets/buttons/' + settingsArray['Game.IconSet'] + '_X.png');
        $(".button").show();
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