$(document).keydown(function (e) {
    // Hide when a key is pressed
    dew.hide();
});

function setTitle(text) {
    $("#title").text(text);
}

function setMessage(text) {
    $("#message").text(text);
}

dew.on("show", function (event) {
    switch (event.screen) {
        case "browser":
            setTitle("Failed to load the server browser!");
            setMessage("Check your internet connection and make sure that Game.MenuURL points to a valid URL.");
            break;
        default:
            dew.hide();
            break;
    }
});