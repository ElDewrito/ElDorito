var isTeamChat = false;
var stayOpen = false;
var hideTimer;
var hideDelay = 4500;
var fadeTime = 800;
var teamGame;
var established;

$(window).load(function () {
    $(document).keydown(function (e) {
        if (e.keyCode === 27) {
            chatboxClear();
        }
    });

    dew.on("show", function(e) {
        dew.getSessionInfo().then(function (res) {
            teamGame = res.hasTeams;
            established = res.established;
            if (res.mapName != "mainmenu"){
                $('#chatWindow').css( 'background-color', 'transparent');
            } else {
                $('#chatWindow').css( 'background-color', 'rgba(0,0,0,0.5)');                
            }
            if (established){
                if(e.data.hasOwnProperty('teamChat')){
                    isTeamChat = e.data.teamChat;
                    if (isTeamChat && !teamGame){
                        dew.hide();
                    } else {
                        $("#chat").show();
                    }
                }              
            } else {
                dew.hide();
            }
            if(e.data.teamChat && teamGame){
                $("#chatBox").attr("placeholder", "TEAM");
            } else {
                $("#chatBox").attr("placeholder", "GLOBAL");                    
            }
            clearTimeout(hideTimer);
            if (!stayOpen) {
                dew.captureInput(e.data.captureInput);
                if (e.data.captureInput) {               
                    stayOpen = true; 
                    $("#chatBox").show();
                    document.getElementById('chatBox').focus();
                } else {
                    $("#chatBox").hide();
                    fadeAway();
                }
            }
        });
    });

    dew.on("chat", function (e) {
        var bgColor =  e.data.color;
        dew.getSessionInfo().then(function (res) {
            teamGame = res.hasTeams;
            if (teamGame){
                if (e.data.teamIndex == 0){
                    bgColor = "#c02020";
                } else if (e.data.teamIndex == 1){
                    bgColor = "#214EC0";
                }
            }
            bgColor = hexToRgba(bgColor);
            var messageClass = 'nameCard';
            var chatType = e.data.chatType
            if(e.data.message.startsWith('/me ')){
                messageClass += ' emote';
                chatType += ' emote';
                e.data.message = e.data.message.substring(4, e.data.message.length);
            }
            $("#chatWindow").append($('<span>', { class: messageClass, css: { backgroundColor: bgColor}, text: e.data.sender }).wrap($('<p>', { class: chatType })).parent().append($("<div>").text(e.data.message).html()));
        });
        dew.show();
    });

    $("html").on("keyup", "#chatBox", function (e) {
        if (e.keyCode === 13) {
            dew.sendChat($("#chatBox").val(), isTeamChat);
            $("#chatBox").val("");
            chatboxClear();
        }
    });
});

function fadeAway(){
    hideTimer = setTimeout(function(){ 
        $("#chat").fadeOut(fadeTime, function() {
            dew.hide(); 
        });
    }, hideDelay);
}

function chatboxClear(){
    dew.captureInput(false);
    fadeAway();
    stayOpen = false;
    $("#chatBox").hide();
}

function hexToRgba(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return 'rgba('+parseInt(result[1], 16)+","+parseInt(result[2], 16)+","+parseInt(result[3], 16)+",0.7)";
}