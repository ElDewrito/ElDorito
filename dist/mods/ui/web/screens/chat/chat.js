var isTeamChat = false;
var stayOpen = false;
var hideTimer;
var hideDelay = 4500;
var fadeTime = 800;
var nameCardOpacity = 0.7;
var teamArray = [
    {name: 'red', color: '#620B0B'},
    {name: 'blue', color: '#0B2362'},
    {name: 'green', color: '#1F3602'},
    {name: 'orange', color: '#BC4D00'},
    {name: 'purple', color: '#1D1052'},
    {name: 'gold', color: '#A77708'},   
    {name: 'brown', color: '#1C0D02'}, 
    {name: 'pink', color: '#FF4D8A'}, 
    {name: 'white', color: '#D8D8D8'}, 
    {name: 'black', color: '#0B0B0B'}           
];
var playerName;
var hideChat;

$(window).load(function(){
    dew.command('Player.Name').then(function(res) {
        playerName = new RegExp("@"+res, "ig");
    });
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            chatboxHide();
        }
    });
    $(document).keydown(function(e){
        if (e.keyCode === 13){ //Enter
            dew.sendChat($("#chatBox").val(), isTeamChat);
            $("#chatBox").val("");
            chatboxHide();
        }else{
            $("#chatBox").focus();
        }
    });
    
    $("#chatBox").keydown(function(e){
        if(e.keyCode === 33) {
            $("#chatWindow").scrollTop($("#chatWindow").scrollTop()-($('#chatWindow p').height() * 6));   
        }
        if(e.keyCode === 34) {
            $("#chatWindow").scrollTop($("#chatWindow").scrollTop()+($('#chatWindow p').height() * 6));        
        }
    });

    $("body").click(function(){
        $("#chatBox").focus();
    });
    
    dew.on("show", function(e){
        dew.command('Game.HideChat').then(function(res) {
            if(res == 1){
                hideChat = true;
            } else {
                hideChat = false;
                dew.command('Player.Name').then(function(res) {
                    playerName = new RegExp("@"+res, "ig");
                });
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
                                    $("#chatWindow").removeClass("hide-scrollbar");
                                });
                            }else{
                                $("#chatBox").hide();
                                $("#chatWindow").css("bottom", "0");
                                $("#chatWindow").addClass("hide-scrollbar");
                                fadeAway();
                            }
                        }
                        $("#chatWindow").scrollTop($('#chatWindow')[0].scrollHeight);
                    }else{
                        dew.hide();
                    }
                });
            }
        });
    });

    dew.on("chat", function(e){
        dew.command('Game.HideChat').then(function(res) {
            if(res == 1){
                hideChat = true;
            }else{
                hideChat = false;
            }
        });
        dew.getSessionInfo().then(function(i){
            if(e.data.hasOwnProperty('color')){
                var bgColor =  e.data.color;
                if (i.hasTeams){
                    if(e.data.hasOwnProperty('teamIndex')){
                        bgColor = teamArray[e.data.teamIndex].color;
                    }
                }
                bgColor = hexToRgba(bgColor, nameCardOpacity);
            }
            var messageClass = 'nameCard';
            var chatClass = e.data.chatType;
            if((e.data.message).match(playerName)){
                chatClass += ' mention';                
            }
            if(e.data.message.startsWith('/me ')){
                messageClass += ' emote';
                chatClass += ' emote';
                e.data.message = e.data.message.substring(4, e.data.message.length);
            }
            $("#chatWindow").append($('<span>', { class: messageClass, css: { backgroundColor: bgColor}, text: e.data.sender }).wrap($('<p>', { class: chatClass })).parent().append($("<div>").html(e.data.message).text().replace(/\bhttp[^ ]+/ig, aWrap))); 
            if(!hideChat){
                dew.show();
            }
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
    $("#chatBox").val('');
    $("#chatBox").hide();
    $("#chatWindow").css("bottom", "0");
    $("#chatWindow").addClass("hide-scrollbar");
}

function hexToRgba(hex,opacity){
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return 'rgba('+parseInt(result[1], 16)+","+parseInt(result[2], 16)+","+parseInt(result[3], 16)+","+opacity+")";
}

function aWrap(link) {
    return '<a href="' + link + '" target="_blank">' + link + '<\/a>';
};

function buttonAction(i){
    switch (i) {
        case 1: // B  
            chatboxHide();
            break;
        default:
            //console.log("nothing associated with " + i);
    }  
}