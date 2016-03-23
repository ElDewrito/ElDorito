var inputHistory = [];
var selectedHistoryIndex = 0;

var consoleSize = 2;
var commandList = {};

function focusInput() {
    $("#command").focus();
}

function setInput(command) {
    $("#command").val(command);
    focusInput();
}

function clearInput() {
    $("#command").val("");
    focusInput();
}

function clearConsole() {
    $("#output-box").empty();
}

function reloadConsole() {
    location.reload();
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

function getConsoleHelp() {
    commands = [
        {
            "module": "Console",
            "name": "Clear",
            "shortName": "console_clear",
            "description": "Clear the Console's input and output areas",
            "defaultValue": null,
            "hidden": false,
            "arguments": []
        },
        {
            "module": "Console",
            "name": "Console.Dock",
            "shortName": "console_dock",
            "description": "Toggle Console docking, allows you to drag and resize the console if undocked. Options: 0, 1 or 2. Setting it to 0 will do the same as the Dock button",
            "defaultValue": 1,
            "hidden": false,
            "arguments": [
                "dockmode(int) Options: 0, 1 or 2. 1 = docked, 2 = undocked, 0 toggles between 1 and 2."
            ]
        },
        {
            "module": "Console",
            "name": "Console.History",
            "shortName": "console_history",
            "description": "Display the commands in the Console's command history, you can navigate the command history using the arrow up/down keys",
            "defaultValue": null,
            "hidden": false,
            "arguments": []
        },
        {
            "module": "Console",
            "name": "Console.Invert",
            "shortName": "console_invert",
            "description": "Inverts the Console input box and drag handle",
            "defaultValue": 1,
            "hidden": false,
            "arguments": [
                "inverted(int) Options: 0, 1 or 2. 1 = not inverted, 2 = inverted, 0 toggles between 1 and 2."
            ]
        },
        {
            "module": "Console",
            "name": "Console.Opacity",
            "shortName": "console_opacity",
            "description": "Set the Console's overall opacity. Range: 0 - 100. Do not set below 40",
            "defaultValue": 100,
            "hidden": false,
            "arguments": [
                "opacityPercentage(int) Range: 0 - 100"
            ]
        },
        {
            "module": "Console",
            "name": "Console.Reload",
            "shortName": "console_reload",
            "description": "Reload (resets) the Console, useful if something goes wrong and you can no longer properly use the console",
            "defaultValue": null,
            "hidden": false,
            "arguments": []
        },
        {
            "module": "Console",
            "name": "Console.Size",
            "shortName": "console_size",
            "description": "Set the Console's output box size manually. Options: 1, 2, 3 or 4. Setting it to 0 will do the same as Console.ToggleSize",
            "defaultValue": 2,
            "hidden": false,
            "arguments": [
                "sizeIndex(int) Options: 0, 1, 2, 3 or 4. 1 = 25% height, 2 = 50% height, 3 = 75% height, 4 = 100% height, 0 toggles between 1, 2, 3 and 4."
            ]
        },
        {
            "module": "Console",
            "name": "Console.Transparency",
            "shortName": "console_transparency",
            "description": "Set the Console's background transparency. Range: 0 - 100",
            "defaultValue": 75,
            "hidden": false,
            "arguments": [
                "transparencyPercentage(int) Range: 0 - 100"
            ]
        }
    ]
    return commands;
}

function showHelp(command) {
    $.merge(commandList, getConsoleHelp());
    if (command == null) {
        commandList = commandList.sort(function (a, b) {
            return a.module.localeCompare( b.module );
        });
        $.each(commandList, function (key, value) {
            if (!value.hidden){
                var name = value.name;
                if (value.defaultValue !== null) {
                    name += " " + value.defaultValue;
                }
                appendLine("", "<span class=\"command\" data-command=\"" + name + "\"><b>" + name + "</b></span> - " + value.description, false);
            }
        });
    }
    else {
        commandItem = $.grep(commandList, function(item){
            return item.name.toLowerCase() == command.toLowerCase();
        });
        commandItem = commandItem[0];
        var name = commandItem.name;
        var usage = "";
        var parameters = [];
        if (commandItem.defaultValue !== null) {
            name += " " + commandItem.defaultValue;
        }
        appendLine("", "<span class=\"command\" data-command=\"" + name + "\"><b>" + name + "</b></span> - " + commandItem.description, false);
        $.each(commandItem.arguments, function (key, value) {
            argument = value.split(" ");
            description = value.replace(argument[0] + " ", "");
            usage += "<" + argument[0] + "> ";
            parameters.push({"name": argument[0], "description": description});
        });
        appendLine("", "Usage: " + name + " " + usage);
        $.each(parameters, function (key, value) {
            appendLine("", "  <i>" + value.name + "</i>: " + value.description, false);
        });
    }
}

function setConsoleSize(size) {
    if (isNaN(size)) {
        appendLine("error-line", "Parameter is not an integer! Options: 0, 1, 2, 3 or 4");
        return;
    }
    else if (size != 0 && size != 1 && size != 2 && size != 3 && size != 4) {
        appendLine("error-line", "Parameter is invalid! Options: 0, 1, 2, 3 or 4");
    }
    if (size === 0) {
        size = (consoleSize % 4) + 1;
    }
    consoleSize = size;
    $(".console").css({ "height": consoleSize * 25 + "%" });
}

function dockConsole(toggle) {
    if (isNaN(toggle)) {
        appendLine("error-line", "Parameter is not an integer! Options: 0, 1 or 2");
        return;
    }
    else if (toggle != 0 && toggle != 1 && toggle != 2) {
        appendLine("error-line", "Parameter is invalid! Options: 0, 1 or 2");
        return;
    }
    switch(toggle) {
        case 1:
            $(".console").draggable("disable").resizable("disable");
            setConsoleSize(consoleSize);
            $(".console").css({left: "0px", top: "0px", width: "100%"});
            break;
        case 2:
            $(".console").draggable("enable").resizable("enable");
            $(".console").css({left: "15px", top: "15px", width: "calc(100% - 30px)"});
            break;
        default:
            if ($(".console").draggable("option", "disabled")) {
                dockConsole(2);
            }
            else {
                dockConsole(1);
            }
            break;
    }
}

function invertConsole(toggle) {
    if (isNaN(toggle)) {
        appendLine("error-line", "Parameter is not an integer! Options: 0, 1 or 2");
        return;
    }
    else if (toggle != 0 && toggle != 1 && toggle != 2) {
        appendLine("error-line", "Parameter is invalid! Options: 0, 1 or 2");
        return;
    }
    switch(toggle) {
        case 1:
            $(".console").prepend($(".console .box.titlebar"));
            $(".console .box.output").after($(".console .box.input"));
            break;
        case 2:
            $(".console").prepend($(".console .box.input"));
            $(".console .box.output").after($(".console .box.titlebar"));
            break;
        default:
            if ($(".console").children.first().hasClass("titlebar")) {
                dockConsole(2);
            }
            else {
                dockConsole(1);
            }
            break;
    }
}

function setConsoleTransparency(percentage) {
    if (isNaN(percentage)) {
        appendLine("error-line", "Parameter is not an integer! Range 0 - 100");
        return;
    }
    else if (percentage < 0 || percentage > 100) {
        appendLine("error-line", "Parameter is out of range! Range 0 - 100");
        return;
    }
    percentage = percentage / 100;
    $(".console").css({"background-color": "rgba(64, 64, 64, " + percentage + ")"});
}

function setConsoleOpacity(percentage) {
    if (isNaN(percentage)) {
        appendLine("error-line", "Parameter is not an integer! Range 0 - 100");
        return;
    }
    else if (percentage < 0 || percentage > 100) {
        appendLine("error-line", "Parameter is out of range! Range 0 - 100");
        return;
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

function appendLine(cssClass, line, htmlFilter) {
    if (typeof htmlFilter === 'undefined' || htmlFilter === null) {
        htmlFilter = true;
    }
    if (line === "") {
        line = "\n";
    }
    var atBottom = isScrolledToBottom($("#output-box"));
    if (htmlFilter) {
        $("<pre></pre>", {
                "class": cssClass
            })
            .text(line)
            .appendTo($("#output-box"));
    }
    else {
        $("<pre></pre>", {
                "class": cssClass
            })
            .html(line)
            .appendTo($("#output-box"));
    }
    if (atBottom) {
        scrollToBottom();
    }
}

function runCommand(command) {
    if (!!command.trim()) {
        switch (command.toLowerCase()) {
            case "clear":
                clearConsole();
                break;
            case "console.reload":
                reloadConsole();
                break;
            case "console.history":
                appendLine("command-line", "> " + command);
                getInputHistory();
                break;
            case "console.test":
                appendLine("command-line", "> " + command);
                showHelp();
                break;
            default:
                if (command.toLowerCase().indexOf('console.size') == 0){
                    appendLine("command-line", "> " + command);
                    var commandValue = command.split(' ');
                    setConsoleSize(parseInt(commandValue[1]));
                    break;
                }
                else if (command.toLowerCase().indexOf('console.dock') == 0){
                    appendLine("command-line", "> " + command);
                    var commandValue = command.split(' ');
                    dockConsole(parseInt(commandValue[1]));
                    break;
                }
                else if (command.toLowerCase().indexOf('console.invert') == 0){
                    appendLine("command-line", "> " + command);
                    var commandValue = command.split(' ');
                    invertConsole(parseInt(commandValue[1]));
                    break;
                }
                else if (command.toLowerCase().indexOf('console.transparency') == 0){
                    appendLine("command-line", "> " + command);
                    var commandValue = command.split(' ');
                    setConsoleTransparency(parseInt(commandValue[1]));
                    break;
                }
                else if (command.toLowerCase().indexOf('console.opacity') == 0){
                    appendLine("command-line", "> " + command);
                    var commandValue = command.split(' ');
                    setConsoleOpacity(parseInt(commandValue[1]));
                    break;
                }
                else if (command.toLowerCase().indexOf('help') == 0){
                    appendLine("command-line", "> " + command);
                    var commandValue = command.split(' ');
                    showHelp(commandValue[1]);
                    break;
                }
                dew.command(command, {}, function (output) {
                    appendLine("command-line", "> " + command);
                    if (output !== "") {
                        appendLine("", output);
                    }
                    scrollToBottom();
                }, function (error) {
                    appendLine("command-line", "> " + command);
                    appendLine("error-line", error.message);
                });
                break;
        }
        scrollToBottom();
        clearInput();
        pushInputHistory(command);
        resetInputHistoryIndex();
    }
}

$(window).load(function () {
    dew.getVersion(function (version) {
        $("#version").text(version);
    });

    dew.getCommands(function (commands){
        commandList = commands;
    }, function () {
        appendLine("error-line", "Could not retrieve list of commands!");
    });

    focusInput();

    $("html").on("click", "#input-box", function () {
        focusInput();
    });

    $(".console")
        .draggable({ handle: ".titlebar", containment: "body", snap: "body", snapMode: "inner", snapTolerance: 10, disabled: true })
        .resizable({ containment: "body", minWidth: 400, minHeight: 300, disabled: true });

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
            // Run the command when up is pressed
            selectInputHistoryIndex(-1);
            e.preventDefault();
        }
        else if (e.keyCode === 40) {
            // Run the command when down is pressed
            selectInputHistoryIndex(1);
        }
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

    $("html").on("click", ".command", function () {
        setInput($(this).data("command"));
        focusInput();
    });

    $("html").on("contextmenu", ".command", function () {
        setInput("help " + $(this).data("command"));
        focusInput();
    });

    $("html").on("click", "#button-help", function () {
        runCommand("Help");
        focusInput();
    });

    $("html").on("click", "#button-clear", function () {
        clearConsole();
        clearInput();
        focusInput();
    });

    $("html").on("click", "#button-reload", function () {
        location.reload();
    });

    $("html").on("click", "#button-size", function () {
        setConsoleSize(0);
        focusInput();
    });

    $("html").on("click", "#button-dock", function () {
        dockConsole(0);
        focusInput();
    });

    $("html").on("click", "#button-hide", function () {
        dew.hide();
    });

    setConsoleSize(consoleSize);
});