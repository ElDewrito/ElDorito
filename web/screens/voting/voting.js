$("html").on("keydown", function(e) {
    if (e.which == 113)
        dew.hide();
});

var interval = 0;
var seconds_left;

function vote(number) {
    dew.command("server.SubmitVote " + number).then(function(output) {}).catch(function(error) {});
}

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
                .html("<p>" + entry.index + ". " + entry.typename + " on " + entry.mapname + "</p><img src='images/" + entry.image + ".jpg'><span id='voteTally" + entry.index + "'  class='voteTally'></span>  ")
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
           dew.command("server.CancelVote").then(function(output) {}).catch(function(error) {});

          dew.hide();
   });
    $(".votingOption").click(function() {

        $(".votingOption").removeClass("selected");
        $(this).addClass("selected");
        vote($(this).attr('id'));
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