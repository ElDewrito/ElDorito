var buttons = ["A","B","X","Y","Back"];
var votingType = "voting";
var controllerType;
var hasGP = false;
var interval = 0;
var seconds_left;
var isHost;
var axisThreshold = .5;
var stickTicks = { left: 0, right: 0, up: 0, down: 0 };
var repGP;
var lastHeldUpdated = 0;
var itemNumber = 1;

$("html").on("keydown", function(e) {
    if (e.which == 113){
        dew.hide();
    }
    if(e.which == 84 || e.which == 89){
        var teamChat = false;
        if(e.which == 89){ teamChat = true };
        dew.show("chat", {'captureInput': true, 'teamChat': teamChat});
    }
    if(e.keyCode == 192 || e.keyCode == 112 || e.keyCode == 223){
        dew.show("console");
    }
});

$("html").on("keyup", function(e) {
    if (e.which == 44) {
        dew.command('Game.TakeScreenshot');  
    }
});

function vote(number) {
    dew.command("server.SubmitVote " + number).then(function(output) {}).catch(function(error) {});
    if(hasGP){
        $(".votingOption").removeClass("selected");
        $('.votingOption').eq(number-1).addClass("selected");
    }
}

dew.on("show", function(event) {
    dew.getSessionInfo().then(function(i){
        isHost = i.isHost;
    });
    dew.command("Settings.Gamepad", {}).then(function (res) {
        if (res == 1)
            dew.captureInput(true);
        else {
            dew.captureInput(false);
            dew.capturePointer(true);
        }
    });
    initGamepad();
    itemNumber = 1;
});

dew.on("hide", function(event) {
    clearInterval(interval);
    if(repGP){
        window.clearInterval(repGP);
        repGP = null;
    }
});

dew.on("Winner", function(event) {
    clearInterval(interval);
    $("#" + event.data.Winner).addClass('winner');
});
dew.on("VetoOptionsUpdated", function(event) {
    clearInterval(interval);
	var message = "";
	if(event.data.vetoOption.canveto){
		message = "VETO COUNTDOWN " 
        votingType = "veto";
	}
	else{
		message = "GAME STARTING IN: " 
        votingType = "ended";
	}
	$(".container").html("");
	   $("<a></a>", {
		  "class": "boxclose",
            "id": "boxclose",
            text: "x"
        })
        .appendTo($(".container"));
    $("<h5></h5>", {
            "id": "title"
        })
        .html(message + "<span id='timer_span'></span>")
        .appendTo($(".container"));

		var entry = event.data.vetoOption;

        if (entry.mapname != '') {
            $("<div></div>", {
                    "class": "votingOption",
                    "id": "1"
                })
                .html("<p>" +  entry.typename + " on " + entry.mapname + "</p><img src='dew://assets/maps/small/" + entry.image + ".png'><span id='voteTally1" + "'  class='voteTally'></span> ")
                .appendTo($(".container"));

        }
    
    seconds_left = event.data.timeRemaining; //event.data[0].voteTime;
    interval = setInterval(function() {
        document.getElementById('timer_span').innerHTML = " - " + --seconds_left;

        if (seconds_left <= 0) {
            document.getElementById('timer_span').innerHTML = "";
            clearInterval(interval);
        }
    }, 1000);
    $('#boxclose').click(function(){
        capturedInput = false;
        if(isHost){
            dew.command("server.CancelVote").then(function() {});
        }
        dew.hide();
    });
    if(votingType == "veto"){
        $(".votingOption").click(function() {
            $(".votingOption").removeClass("selected");
            $(this).addClass("selected");
            vote($(this).attr('id'));
        });  
        $(".votingOption").hover(
            function() {
                $( this ).addClass( "selected" );
            }, function() {
                $( this ).removeClass("selected");
            }
        );
    }
});
dew.on("VotingOptionsUpdated", function(event) {
    votingType = "voting";
    clearInterval(interval);
	
    $(".container").html("");
	   $("<a></a>", {
		  "class": "boxclose",
            "id": "boxclose",
            text: "x"
        })
        .appendTo($(".container"));
    $("<h5></h5>", {
            "id": "title"
        })
        .html("VOTE FOR GAME AND MAP <span id='timer_span'></span>")
        .appendTo($(".container"));

    event.data.votingOptions.forEach(function(entry, i) {

        if (entry.mapname == "Revote") {
            $("<div></div>", {
                    "class": "revoteOption votingOption",
                    "id": entry.index
                })
                .html("<h5> NONE OF THE ABOVE </h5><span id='voteTally" + entry.index + "'  class='voteTally'></span> ")
                .appendTo($(".container"));
        } else if (entry.mapname != '') {
            $("<div></div>", {
                    "class": "votingOption",
                    "id": entry.index
                })
                .html("<p>" + entry.index + ". " + entry.typename + " on " + entry.mapname + "</p><img src='dew://assets/maps/small/" + entry.image + ".png'><span id='voteTally" + entry.index + "'  class='voteTally'></span> ")
                .appendTo($(".container"));

        }
    });

    seconds_left = event.data.timeRemaining; //event.data[0].voteTime;
    interval = setInterval(function() {
        document.getElementById('timer_span').innerHTML = " - " + --seconds_left;

        if (seconds_left <= 0) {
            document.getElementById('timer_span').innerHTML = "";
            clearInterval(interval);
        }
    }, 1000);
    $('#boxclose').click(function(){
        capturedInput = false;
        if(isHost){
            dew.command("server.CancelVote").then(function() {});
        }
        dew.hide();
   });
    $(".votingOption").click(function() {
        $(".votingOption").removeClass("selected");
        $(this).addClass("selected");
        vote($(this).attr('id'));
    });    
    $(".votingOption").hover(
        function() {
            $( this ).addClass( "selected" );
        }, function() {
            $( this ).removeClass("selected");
        }
    );
});

dew.on("VoteCountsUpdated", function(event) {
    event.data.forEach(function(entry, i) {
        if (entry.Count == 0)
            $("#voteTally" + entry.OptionIndex).text("");
        else
            $("#voteTally" + entry.OptionIndex).text(entry.Count);
    });
});

function onControllerConnect(){
    dew.command('Game.IconSet', {}).then(function(response){
        controllerType = response;
        $("#boxclose").html("<img class='button' src='dew://assets/buttons/"+response+"_B.png'>");  
        $('.playerStats img').eq(0).attr('src','dew://assets/buttons/' + response + '_LB.png');
        $('.playerStats img').eq(1).attr('src','dew://assets/buttons/' + response + '_RB.png');
    });
    if(votingType != 'veto'){
        updateSelection(1);    
    }
}

function onControllerDisconnect(){
    $('.selected').removeClass(); 
    $("#boxclose").text("x"); 
}

dew.on('controllerinput', function(e){       
    if(hasGP){
        if(votingType != 'veto'){
            if(e.data.A == 1){
                vote(itemNumber);
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
        }else{
            if(e.data.X == 1){
                vote(1);
            }            
        }
        if(e.data.B == 1){
            dew.hide();
        }
    }
});

function initGamepad(){
    dew.command('Settings.Gamepad', {}).then(function(result){
        if(result == 1){
            onControllerConnect();
            hasGP = true;
            if(!repGP){
                repGP = window.setInterval(checkGamepad,1000/60);
            }
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
};

function updateSelection(item){
    $('.selected').removeClass('selected');
    $('#'+item).addClass('selected');
    dew.command('Game.PlaySound 0xAFE');
}

function upNav(){
    if(itemNumber > 1){
        itemNumber--;
        updateSelection(itemNumber);
    }
}

function downNav(){
    if(itemNumber < $('.votingOption').length){
        itemNumber++;
        updateSelection(itemNumber);
    }           
}