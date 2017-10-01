var mapName = "";
var gameModes = ["slayer","ctf","slayer","oddball","koth","forge","vip","juggernaut","territories","assault","infection"];
var safeDomains = ["discord.gg"];

$("html").on("keydown", function(e) {
    if(e.which == 84 || e.which == 89){
        var teamChat = false;
        if(e.which == 89){ teamChat = true };
        dew.show("chat", {'captureInput': true, 'teamChat': teamChat});
    }
    if(e.which == 192 || e.which == 112){
        dew.show("console");
    }
});

function textWithNewLines(text) {
    var htmls = [];
    var lines = text.split("\\n");
    var tmpDiv = jQuery(document.createElement('div'));
    for (var i = 0 ; i < lines.length ; i++) {
        htmls.push(tmpDiv.text(lines[i].trim()).html());
    }
    return htmls.join("<br>");
}

function aWrap(link) {
    var parser = document.createElement('a');
    parser.href = link;
    if(safeDomains.indexOf(parser.hostname) > -1){
        return '<a href="' + link + '" target="_blank">' + link + '<\/a>';
    } else {
        return '';
    }
};

function updateProgress(progress) {
    $("#progressbar").attr('value', progress);
    $(".loading").css({"-webkit-clip-path": "inset(" + progress + "% 0 0 0)"});
}

function loadMap(mapName) {
    $(".mapLoader").show();
    $(".mapLoader").css({backgroundImage: "url('dew://assets/maps/large/" + mapName + ".jpg'), url('dew://assets/maps/large/unknown.jpg')"});
    $(".genericLoader").hide();
    dew.getMapVariantInfo().then(function (info) {
        $("#title").text(info.name);
        $("#desc").text(info.description);
    });
    dew.getGameVariantInfo().then(function (info) {
        $("#gametypeicon").attr("src", "dew://assets/gametypes/" + gameModes[info.mode] + ".png");
        $("#gametype").text(info.name);  
        if(info.rounds > 0){
            $("#gamerounds").text(info.rounds);  
        } else {
            $("#gamerounds").text("Unlimited"); 
        }
        if(info.scoreToWin > -1){
            $("#gamescore").text(info.scoreToWin);   
        } else {
            $("#gamescore").text("Unlimited");  
        }
        if(info.timeLimit > 0){
            $("#timelimit").text(info.timeLimit+":00"); 
        } else {
            $("#timelimit").text("Unlimited");  
        }                
    });
    dew.command("Server.NameClient", { internal: true }).then(function (name) {
        $(".serverName").text(name);
    });
    dew.command("Server.MessageClient", { internal: true }).then(function (message) {
        if(message.length > 0){
            message = message.substr(0, 512);
            $(".serverMessage").show();
            $(".serverMessage").html(textWithNewLines(message).replace(/\bhttp[^ ]+/ig, aWrap));
        } else {
            $(".serverMessage").hide();
        }
    });
}

function loadGeneric() {
    $(".genericLoader").show();
    $(".mapLoader").hide();
}

function resetLoader() {
    $(".genericLoader, .mapLoader").hide();
    $("#gametype, #gamerounds, #gamescore, #timelimit, .serverName, .serverMessage, .title, .desc").text("");
    $(".mapLoader").css({backgroundImage: ""});
    $("#gametypeicon").attr("src", "");
}

dew.on("show", function (event) {
    mapName = event.data.map || "";
    if (mapName != "mainmenu") {
        loadMap(mapName);
        dew.captureInput(true);
    } else {
        loadGeneric();
        dew.captureInput(false);
    }
    updateProgress(0);
});

dew.on("hide", function (e) {
    resetLoader();
});

dew.on("loadprogress", function (event) {
    var progress = event.data.currentBytes / event.data.totalBytes * 100;
    updateProgress(progress);
});