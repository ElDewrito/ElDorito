$(window).load(function () {
    $(document).keydown(function (e) {
        if (e.keyCode === 27) {
            // Hide when escape is pressed
            dew.hide();
        }
    });

    dew.on("chat", function (e) {
        console.log(e);
        //Make sure to escape chat messages
        $("#chat").append("<p style='color: " + e.data.color + "'>" + $("<div>").text("<" + e.data.sender + "> " + e.data.message).html() + "</p>");
    });

    $("html").on("keyup", "#chatBox", function (e) {
        if (e.keyCode === 13) {
            //Send global chat message
            dew.sendChat($("#chatBox").val(), false);
            //For team messages:
            //dew.sendChat($("#chatBox").val(), true);
            $("#chatBox").val("");
        }
    });
});
