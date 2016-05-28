var isTeamChat = false;
var stayOpen = false;
var hideTimer;
var hideDelay = 4500;
var fadeTime = 800;
var nameCardOpacity = 0.7;

$(window).load(function(){
    $(document).keydown(function(e){
        if(e.keyCode === 27) { //ESC
            chatboxHide();
        }else if (e.keyCode === 13){ //Enter
            dew.sendChat($("#chatBox").val(), isTeamChat);
            $("#chatBox").val("");
            chatboxHide();
        }else{
            $("#chatBox").focus();
        }
    });

    $("body").click(function(){
        $("#chatBox").focus();
    });

    dew.on("show", function(e){
        clearTimeout(hideTimer);
        if(e.data.hasOwnProperty('teamChat')){
            isTeamChat = e.data.teamChat;
        }
        dew.getSessionInfo().then(function(i){
            if(i.established){
                if(isTeamChat && !i.hasTeams){
                    isTeamChat = false;
                } 
                $("#chat").stop();
                $("#chat").show(true, true);
                $('body').removeClass();
                if(i.mapName != "mainmenu"){
                    $("body").addClass("inGame");
                }else{
                    $("body").addClass("inLobby");
                }
                if(isTeamChat && i.hasTeams){
                    $("#chatBox").attr("placeholder", "TEAM");
                }else{
                    $("#chatBox").attr("placeholder", "GLOBAL");
                }
                if(!stayOpen){
                    dew.captureInput(e.data.captureInput);
                    if (e.data.captureInput) {
                        stayOpen = true;
                        $("#chatBox").show(0, "linear", function(){
                            $("#chatBox").focus();
                            $("#chatWindow").css("bottom", "3.54vh");
                        });
                    }else{
                        $("#chatBox").hide();
                        $("#chatWindow").css("bottom", "0");
                        fadeAway();
                    }
                }
            }else{
                dew.hide();
            }
        });
    });

    dew.on("chat", function(e){
        dew.getSessionInfo().then(function(i){
            if(e.data.hasOwnProperty('color')){
                var bgColor =  e.data.color;
                if (i.hasTeams){
                    if(e.data.hasOwnProperty('teamIndex')){
                        if(e.data.teamIndex == 0){
                            bgColor = "#c02020";
                        }else if(e.data.teamIndex == 1){
                            bgColor = "#214EC0";
                        }
                    }
                }
                bgColor = hexToRgba(bgColor, nameCardOpacity);
            }
            var messageClass = 'nameCard';
            var chatClass = e.data.chatType;
            if(e.data.message.startsWith('/me ')){
                messageClass += ' emote';
                chatClass += ' emote';
                e.data.message = e.data.message.substring(4, e.data.message.length);
            }
            $("#chatWindow").append($('<span>', { class: messageClass, css: { backgroundColor: bgColor}, text: e.data.sender }).wrap($('<p>', { class: chatClass })).parent().append($("<div>").text(e.data.message).html()));
            dew.show();
        });
    });
});

function fadeAway(){
    clearTimeout(hideTimer);
    hideTimer = setTimeout(function(){
        $("#chat").fadeOut(fadeTime, function(){
            dew.hide();
        });
    }, hideDelay);
}

function chatboxHide(){
    dew.captureInput(false);
    fadeAway();
    stayOpen = false;
    $("#chatBox").hide();
}

function hexToRgba(hex,opacity){
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return 'rgba('+parseInt(result[1], 16)+","+parseInt(result[2], 16)+","+parseInt(result[3], 16)+","+opacity+")";
}