$("html").on("keydown", function(e) {
    if (e.which == 113){
        dew.hide();
    }
    if(e.which == 84 || e.which == 89){
        var teamChat = false;
        if(e.which == 89){ teamChat = true };
        dew.show("chat", {'captureInput': true, 'teamChat': teamChat});
    }
    if(e.which == 192 || e.which == 112){
        dew.show("console");
    }
});

var interval = 0;
var seconds_left;
var isHost;

function vote(number) {
    dew.command("server.SubmitVote " + number).then(function(output) {}).catch(function(error) {});
}

dew.on("show", function(event) {
    dew.captureInput(true);
    dew.getSessionInfo().then(function(i){
        isHost = i.isHost;
    });
});

dew.on("hide", function(event) {
    clearInterval(interval);
});


dew.on("Winner", function(event) {
    clearInterval(interval);
    $("#" + event.data.Winner).addClass('winner');
});

dew.on("VotingOptionsUpdated", function(event) {
    clearInterval(interval);
	
    $(".container").html("");
	   $("<a></a>", {
		  "class": "boxclose",
            "id": "boxclose"
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
                .html("<h5> NONE OF THE ABOVE </h5><span id='voteTally" + entry.index + "'  class='voteTally'></span>  ")
                .appendTo($(".container"));
        } else if (entry.mapname != '') {
            $("<div></div>", {
                    "class": "votingOption",
                    "id": entry.index
                })
                .html("<p>" + entry.index + ". " + entry.typename + " on " + entry.mapname + "</p><img src='dew://assets/maps/small/" + entry.image + ".png'><span id='voteTally" + entry.index + "'  class='voteTally'></span>  ")
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
        if(isHost){
            dew.command("server.CancelVote").then(function() {});
            dew.hide();
        } else {
            dew.captureInput(false);
            $("#boxclose").remove();
        }
   });
    $(".votingOption").click(function() {

        $(".votingOption").removeClass("selected");
        $(this).addClass("selected");
        vote($(this).attr('id'));
    });
    $(".clickCatcher").click(function(){
            dew.captureInput(false);
            $("#boxclose").remove();        
    });
    
});

dew.on("VoteCountsUpdated", function(event) {
    event.data.forEach(function(entry, i) {
        if (entry.Count == 0)
            $("#voteTally" + entry.OptionIndex).text("");
        else
            $("#voteTally" + entry.OptionIndex).text(entry.Count);

    });
});