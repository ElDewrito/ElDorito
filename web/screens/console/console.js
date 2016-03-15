function focusInput() {
    $("#command").focus();
}

function clearInput() {
    $("#command").val("");
}

function clearConsole() {
    $("#output-box").empty();
}

function isScrolledToBottom(e) {
    return (e.prop("scrollHeight") - e.scrollTop() - e.outerHeight() < 1);
}

function scrollToBottom() {
    var box = $("#output-box");
    box.scrollTop(box.prop("scrollHeight"));
}

function appendLine(cssClass, line) {
    if (line === "") {
        line = "\n";
    }
    var atBottom = isScrolledToBottom($("#output-box"));
    $("<pre></pre>", {
            "class": cssClass
        })
        .text(line)
        .appendTo($("#output-box"));
    if (atBottom) {
        scrollToBottom();
    }
}

function runCommand(command) {
    switch (command.toLowerCase()) {
        case "clear":
            clearConsole();
            break;
        default:
            dew.command(command, {}, function (output) {
                appendLine("command-line", "> " + command);
                if (output !== "") {
                    appendLine("", output);
                }
                scrollToBottom();
            });
            break;
    }
}

$(window).load(function () {
    dew.getVersion(function (version) {
        $("#version").text(version);
    });

    $("#input-box").click(function () {
        focusInput();
    });

    $(document).keydown(function (e) {
        if (e.keyCode === 27) {
            // Hide when escape is pressed
            dew.hide();
        }
    });

    $("#command").keydown(function (e) {
        if (e.keyCode === 13) {
            // Run the command when enter is pressed
            runCommand($("#command").val());
            clearInput();
        }
    });
});

dew.on("show", function (e) {
    focusInput();
});

dew.on("hide", function (e) {
    clearInput();
    scrollToBottom();
});

dew.on("console", function (e) {
    appendLine("", e.data.line);
});