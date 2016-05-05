var isTeamChat = false;

$(window).load(function () {
    $(document).keydown(function (e) {
        if (e.keyCode === 27) {
            // Hide when escape is pressed
            dew.hide();
        }
    });

    dew.on("show", function(e) {
        isTeamChat = e.data.teamChat;
    });

    dew.on("chat", function (e) {
        console.log(e);
        //Make sure to escape chat messages
        $("#chat").append("<p style='color: " + e.data.color + "'>" + $("<div>").text("<" + e.data.sender + "> " + e.data.message).html() + "</p>");
    });

    $("html").on("keyup", "#chatBox", function (e) {
        if (e.keyCode === 13) {
            //Send the chat message
            dew.sendChat($("#chatBox").val(), isTeamChat);
            $("#chatBox").val("");
        }
    });
});
