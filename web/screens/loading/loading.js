var mapName = "";

function updateProgress(progress) {
    $("#text").text("Loading " + mapName + "... (" + Math.round(progress) + "%)");
}

dew.on("show", function (event) {
    mapName = event.data.map || "";
    updateProgress(0);

    // Remote name and message are internal variables
    // (the public ones are for the local player's server, not the server we're connected to)
    dew.command("Server.NameClient", { internal: true }, function (name) {
        $("#server").text(name.substr(0, 512));
    });
    dew.command("Server.MessageClient", { internal: true }, function (message) {
        $("#message").text(message.substr(0, 512));
    });
});

dew.on("hide", function (event) {
    // Reset strings so that stale values don't briefly show when the screen is shown again
    $("#text").text("");
    $("#server").text("");
    $("#message").text("");
});

dew.on("loadprogress", function (event) {
    var progress = event.data.currentBytes / event.data.totalBytes * 100;
    updateProgress(progress);
});