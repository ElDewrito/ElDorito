var locked = false;
var isHost = false;
var cardOpacity = 0.9;

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

$(window).load(function(){
    $(document).keydown(function(e) {
        if((locked && e.keyCode == 9)||(locked && e.keyCode == 27)){
            dew.hide();
        } 
        if (e.keyCode == 84 || e.keyCode == 89){
            var teamChat = false;
            if(e.keyCode == 89){ teamChat = true };
            dew.show("chat", {'captureInput': true, 'teamChat': teamChat});
        }
        if (e.keyCode == 192 || e.keyCode == 112){
            dew.show("console");
        }
    });
    $.contextMenu({
        selector: '.clickable', 
        callback: function(key, options) {
            switch(key) {
                case "mute":
                    dew.command("VoIP.MutePlayer " + flipUID($(this).attr('data-uid')));
                    break;
                case "kick":
                    dew.command("Server.KickUid " + flipUID($(this).attr('data-uid')));
                    break;
                case "ban":
                    dew.command("Server.KickBanUid " + flipUID($(this).attr('data-uid')));
                    break;
                default:
                    console.log(key + " " + $(this).attr('data-name') + " " + $(this).attr('data-uid'));
            }
        },
        items: {
            "mute": {  name: "Mute" },
            "kick": {
                name: "Kick",                
                disabled: function(key, options) { 
                    return !isHost; 
                }
            },
            "ban": {
                name: "Ban",                
                disabled: function(key, options) {
                    return !isHost; 
                }
            }
        }
    });
});

dew.on("scoreboard", function(e){
    displayScoreboard();
});

dew.on("show", function(e){
    locked = e.data.locked;
    dew.captureInput(locked);
    if(locked){
        $('#closeButton').show();
    }else{
        $('#closeButton').hide();
    }
    if(e.data.postgame){
        $('#winnerText').show();
    }else{
        $('#winnerText').hide();
    }
});

function displayScoreboard(){
    dew.getSessionInfo().then(function(i){
        isHost = i.isHost;
    });
    dew.getScoreboard().then(function (e) { 
        $('#header').empty();
        //if(e.gameType=="slayer"){
            $('#header').append(
                '<th></th><th>Players</th><th>Kills</th><th>Deaths</th><th>Assists</th><th>Score</th>'
            );
        //}
        buildScoreboard(e.players, e.hasTeams, e.teamScores);
    });
    dew.command("Server.NameClient", { internal: true }).then(function (name) {
        $("#serverName").text(name);
    });    
}

function buildScoreboard(lobby, teamGame, scoreArray){
    var tempArray = [];
    for(var i=0; i < scoreArray.length; i++) {
        tempArray.push({name: teamArray[i].name, score: -999});
    }
    var where = '#scoreboard';
    if(lobby.length > 0) {
        $('#scoreboard').empty();
        $('#window tbody').slice(1).remove();
        for(var i=0; i < lobby.length; i++) {
            var bgColor = lobby[i].color;
            if(teamGame){
                bgColor = teamArray[lobby[i].team].color;
                where = '#'+teamArray[lobby[i].team].name;
                if($(where).length == 0) {
                    $('#window table').append('<tbody id="'+teamArray[lobby[i].team].name+'"><tr class="player teamHeader" style="background-color:'+hexToRgb(teamArray[lobby[i].team].color, cardOpacity)+';"><td></td><td>'+teamArray[lobby[i].team].name.toUpperCase()+' TEAM</td><td></td><td></td><td></td><td></td></tr></tbody>');    
                }        
                $(where+' td:eq(2)').text(parseInt($(where+' td:eq(2)').text() || 0)+lobby[i].kills); //kills
                $(where+' td:eq(3)').text(parseInt($(where+' td:eq(3)').text() || 0)+lobby[i].deaths); //deaths
                $(where+' td:eq(4)').text(parseInt($(where+' td:eq(4)').text() || 0)+lobby[i].assists); //assists  
                $(where+' td:eq(5)').text(parseInt($(where+' td:eq(5)').text() || 0)+lobby[i].score); //score                  
                tempArray[tempArray.findIndex(x => x.name == teamArray[lobby[i].team].name)].score = $(where+' td:eq(5)').text();              
                sortScoreboard(tempArray, teamGame, 'teams');
            }
            $(where).append(
                $('<tr>', {
                    css: {
                        backgroundColor: hexToRgb(bgColor,cardOpacity)
                    },
                    id: lobby[i].name,
                    class: 'player clickable',
                    'data-uid': lobby[i].UID,
                    'data-name': lobby[i].name,
                    'data-color': bgColor
                }).click(function() {
                    console.log($(this).attr('data-name') + " " +$(this).attr('data-uid'));
                    dew.getStats($(this).attr('data-name')).then(function (e) {
                        console.log(e);
                    })
                }).mouseover(function() {
                    col = $(this).attr('data-color'),
                    bright = adjustColor(col, 30);
                    $(this).css("background-color", hexToRgb(bright, cardOpacity));
                }).mouseout(function() {
                    col = $(this).attr('data-color');
                    $(this).css("background-color", hexToRgb(col, cardOpacity));
                })
                .append($('<td>'))
                .append($('<td>').text(lobby[i].name)) //name
                .append($('<td>').text(lobby[i].kills)) //kills
                .append($('<td>').text(lobby[i].deaths)) //deaths
                .append($('<td>').text(lobby[i].assists)) //assists
                .append($('<td>').text(lobby[i].score)) //score
            );                     
        }      
        sortScoreboard(lobby, teamGame, 'players');  
    }  
}

function hexToRgb(hex, opacity) {
	var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
	return "rgba(" + parseInt(result[1], 16) + "," + parseInt(result[2], 16) + "," + parseInt(result[3], 16) + "," + opacity + ")";
}

function sortScoreboard(list, teamGame, sortWhat){
    list.sort(function(b, a) {
        return parseFloat(a.score) - parseFloat(b.score);
    });
    if(list.length > 1){
        if(list[0].score == list[1].score) {
            $('#winnerText').text('Tie!');
        } else {
            if(teamGame){
                $('#winnerText').text(list[0].name.substr(0,1).toUpperCase() + list[0].name.substr(1)+' Team wins!');
            }else{
                $('#winnerText').text(list[0].name+' wins!');           
            }
        }
    }
    var lastScore = list[0].score;
    var x = 1;
    for (i = 0; i < list.length; i++) { 
        if((teamGame && sortWhat == 'teams')||(!teamGame && sortWhat == 'players')){
            if(list[i].score != lastScore){x+=1;}
            $('#'+list[i].name+' td:eq(0)').text(x);
            lastScore = list[i].score;
        }
        if(teamGame && sortWhat =='players'){
            where = '#'+teamArray[list[i].team].name;
        }else if(!teamGame && sortWhat =='players'){
            where = '#scoreboard';
        }else{
            where = '#window table';
        } 
        $(where).append($('#'+list[i].name));     
    }
}

function adjustColor(color, amount) {
	var colorhex = (color.split("#")[1]).match(/.{2}/g);
	for (var i = 0; i < 3; i++) {
		var e = parseInt(colorhex[i], 16);
		e += amount;
        if(amount > 0){
            colorhex[i] = ((e > 255) ? 255 : e).toString(16);
        } else {
            colorhex[i] = ((e < 0) ? 0 : e).toString(16);           
        }
	}
	return "#" + colorhex[0] + colorhex[1] + colorhex[2];
}


function flipUID(uid){
    var bits = uid.match(/.{1,2}/g);
    var newUID = "";
    for (var i = 0; i < bits.length; i++){
        newUID = bits[i] + newUID;
    }
    return newUID;
}