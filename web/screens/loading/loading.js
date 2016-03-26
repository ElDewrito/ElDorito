var mapName = "";
var gameModes = ["slayer","ctf","slayer","oddball","koth","forge","vip","juggernaut","territories","assault","infection"];

function updateProgress(progress) {
    $("#progressbar").attr('value', progress);
    progress = 100 - progress;
    $(".loading").css({"-webkit-clip-path": "inset(" + progress + "% 0 0 0)"});
}

dew.on("show", function (event) {
    mapName = event.data.map || "";
    if (mapName != "mainmenu") {
        $(".generic").hide();
        $(".container").show();
        $("body").css({backgroundImage: "url('images/maps/" + mapName + ".png')"});
        dew.getMapVariantInfo(function (info) {
            $("#title").text(info.name);
            $("#desc").text(info.description);
        });
        dew.getGameVariantInfo(function (info) {
            $("#gametypeicon").attr("src", "images/gametypes/" + gameModes[info.mode] + ".png");
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
                $("#timelimit").text(timeConvert(info.timeLimit)); 
            } else {
                $("#timelimit").text("Unlimited");  
            }                
        });
        dew.command("Server.NameClient", { internal: true }, function (name) {
            $(".serverName").text(name);
        });
        dew.command("Server.MessageClient", { internal: true }, function (message) {
            if(message.length > 0){
                $(".serverMessage").show();
                $(".serverMessage").html(textWithNewLines(message));
            } else {
                $(".serverMessage").hide();
            }
        });
        $(".header").show();
        $(".footer").show();
    } else {
        $(".generic").show();
        $(".header").hide();
        $(".container").hide();
        $(".footer").hide();
    }
    updateProgress(0);
});

dew.on("loadprogress", function (event) {
    var progress = event.data.currentBytes / event.data.totalBytes * 100;
    updateProgress(progress);
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

function timeConvert(mins){
    var hours = Math.trunc(mins/60);
    var minutes = mins % 60;
    if (minutes <= 9) { minutes = "0" + minutes; };
    return hours +":"+ minutes;
}