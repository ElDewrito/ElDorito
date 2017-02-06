dew.on("show", function (event) {
    $("#title").text(event.data.title);
    $("#description").text(event.data.description);
    $("#text").val(event.data.defaultValue);
    $(".dialog").show();
    $("#text").focus();
});

dew.on("hide", function (event) {
    $(".dialog").hide();
});

$(window).load(function () {
    $(".dialog").hide();

    $("html").on("submit", "form", function (event) {
        event.preventDefault();
        var value = $("#text").val().trim();
        if (value === "") {
            return;
        }
        dew.submitVirtualKeyboard(value).then(() => dew.hide());
    });

    $("html").on("keydown", function (event) {
        if (event.keyCode === 13 /* Enter */) {
            $("form").submit();
        } else if (event.keyCode === 27 /* Escape */) {
            dew.cancelVirtualKeyboard().then(() => dew.hide());
        }
    });
});