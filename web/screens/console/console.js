var inputHistory = [];
var selectedHistoryIndex = 0;

var consoleSize = 2;
var commandList = {};

function focusInput() {
    $("#command").focus();
}

function clearInput() {
    $("#command").val("");
    focusInput();
}

function pushInputHistory(command) {
    if (command != inputHistory[inputHistory.length - 1]) {
        inputHistory.push(command);
        selectedHistoryIndex++;
    }
}

function getInputHistory() {
    appendLine("debug-line", "History: {" + inputHistory + "}");
    appendLine("debug-line", "Count: " + inputHistory.length);
}

function selectInputHistoryIndex(direction) {
    if(selectedHistoryIndex + direction <= inputHistory.length && selectedHistoryIndex + direction >= 0) {
        selectedHistoryIndex = selectedHistoryIndex + direction;
        $("#command").val(inputHistory[selectedHistoryIndex]);
    }
}

function resetInputHistoryIndex() {
    selectedHistoryIndex = inputHistory.length;
}

function clearConsole() {
    $("#output-box").empty();
}

function showConsoleHelp() {
    commandList = {
        "Clear" : "Clear the Console's input and output areas",
        "Console.Help" : "Display this help text for the Console related commands",
        "Console.Reload" : "Reload (resets) the Console, useful if something goes wrong and you can no longer properly use the console",
        "Console.History" : "Display the commands in the Console's command history, you can navigate the command history using the arrow up/down keys",
        "Console.ToggleSize" : "Toggle between different sizes of the Console's output box",
        "Console.Size {int}" : "Set the Console's output box size manually. Options: 1, 2, 3 or 4. Setting it to 0 will do the same as Console.ToggleSize",
        "Console.Transparency {int}" : "Set the Console's output box background opacity. Range: 0 - 100. Default is 75",
        "Console.Opacity {int}" : "Set the Console's overall opacity. Range: 0 - 100. Do not set below 40. Default is 100",
        "Help" : "Display the game's help text"
    }
    $.each(commandList, function(key, value) {
        appendLine("", key + " - " + value);
    });
}

function reloadConsole() {
    location.reload();
}

function setConsoleSize(size) {
    if (isNaN(size)) {
        appendLine("debug-line", "Parameter is not a number! Options: 0, 1, 2, 3 or 4");
        return;
    }
    if (size === 0) {
        size = (consoleSize % 4) + 1;
    }
    consoleSize = size;
    $(".console").css({ "height": consoleSize * 25 + "%" });
}

function setConsoleTransparency(percentage) {
    if (isNaN(percentage)) {
        appendLine("debug-line", "Parameter is not a number! Range 0 - 100");
    }
    percentage = percentage / 100;
    $(".console .box.transparent").css({"background-color": "rgba(64, 64, 64, " + percentage + ")"});
}

function setConsoleOpacity(percentage) {
    if (isNaN(percentage)) {
        appendLine("debug-line", "Parameter is not a number! Range 0 - 100");
    }
    percentage = percentage / 100;
    $("body").css({"opacity": percentage});
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
    if (!!command.trim()) {
        appendLine("command-line", "> " + command);
        switch (command.toLowerCase()) {
            case "clear":
                clearConsole();
                break;
            case "console.help":
                showConsoleHelp();
                break;
            case "console.reload":
                reloadConsole();
                break;
            case "console.history":
                getInputHistory();
                break;
            case "console.togglesize":
                setConsoleSize(0);
                break;
            default:
                if (command.toLowerCase().indexOf('console.size') === 0){
                    var commandValue = command.split(' ');
                    setConsoleSize(parseInt(commandValue[1]));
                    break;
                }
                else if (command.toLowerCase().indexOf('console.transparency') === 0){
                    var commandValue = command.split(' ');
                    setConsoleTransparency(parseInt(commandValue[1]));
                    break;
                }
                else if (command.toLowerCase().indexOf('console.opacity') === 0){
                    var commandValue = command.split(' ');
                    setConsoleOpacity(parseInt(commandValue[1]));
                    break;
                }
                dew.command(command, {}, function (output) {
                    if (output !== "") {
                        appendLine("", output);
                    }
                    scrollToBottom();
                }, function (error) {
                    appendLine("debug-line", error.message);
                });
                break;
        }
        pushInputHistory(command);
        resetInputHistoryIndex();
    }
}

$(window).load(function () {
    dew.getVersion(function (version) {
        $("#version").text(version);
    });

    focusInput();
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
        else if (e.keyCode === 38) {
            // Run the command when enter is pressed
            selectInputHistoryIndex(-1);
            e.preventDefault();
        }
        else if (e.keyCode === 40) {
            // Run the command when enter is pressed
            selectInputHistoryIndex(1);
        }
    });

    $("#button-help").click(function () {
        runCommand("Console.Help");
        runCommand("Help");
        focusInput();
    });

    $("#button-clear").click(function () {
        clearConsole();
        clearInput();
        focusInput();
    });

    $("#button-size").click(function () {
        setConsoleSize(0);
        focusInput();
    });

    $("#button-hide").click(function () {
        dew.hide();
    });

    setConsoleSize(consoleSize);
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