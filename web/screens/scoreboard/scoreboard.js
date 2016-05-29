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
        tempArray.push({name: teamArray[i].name, score: 0});
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
                orderTeams(tempArray);
            }
            $(where).append(
                $('<tr>', {
                    css: {
                        backgroundColor: hexToRgb(bgColor,cardOpacity)
                    },
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
        sortTable(where, 5);   
        if(!teamGame){ //add rank numbers
            for (i = 0; i< $(where+' tr').length; i++){
                $(where+' tr:eq('+i+') td:eq(0)').text(i+1);  
            }
            lobby.sort(function(b, a) {
                return parseFloat(a.score) - parseFloat(b.score);
            });
            if(lobby[0].score == lobby[1].score) {
                $('#winnerText').text('Tie!');
            } else {
                $('#winnerText').text(lobby[0].name+' Team wins!');
            }
        }  
    }  
}

function hexToRgb(hex, opacity) {
	var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
	return "rgba(" + parseInt(result[1], 16) + "," + parseInt(result[2], 16) + "," + parseInt(result[3], 16) + "," + opacity + ")";
}

function orderTeams(list){
    list.sort(function(b, a) {
        return parseFloat(a.score) - parseFloat(b.score);
    });
    if(list[0].score == list[1].score) {
        $('#winnerText').text('Tie!');
    } else {
        $('#winnerText').text(list[0].name.substr(0,1).toUpperCase() + list[0].name.substr(1)+' Team wins!');
    }
    for (i = 0; i < list.length; i++) { 
        $('#'+list[i].name+' td:eq(0)').text(parseInt(i)+1);
         $('#window table').append($('#'+list[i].name));
    }
}

function sortTable(where, score){
    var rows = $(where+' tr:not(.teamHeader)').get();
    rows.sort(function(b, a) {
        return parseInt($(a).children('td').eq(score).text(),10) - parseInt($(b).children('td').eq(score).text(),10);
    });
    $.each(rows, function(index, row) {
        $(where).append(row);
    });
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