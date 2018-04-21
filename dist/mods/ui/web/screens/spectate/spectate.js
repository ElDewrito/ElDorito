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

dew.on("spectate_change", function(e){
    dew.show();
    dew.getScoreboard().then(function (x){ 
        var playerIndex = x.players.findIndex(i => i.name == e.data.displayName);
        showSpectateOverlay(playerIndex, x.players, x.hasTeams, JSON.parse(x.playersInfo));
    });
});

dew.on("spectate_end", function(e){
    dew.hide();
});

function showSpectateOverlay(i, lobby, teamGame, playersInfo){
    var emblemPath;
	var hostPath = 'dew://assets/emblems/crown.png';
    if(lobby.length > 0){
        $('#spectatingOverlay').empty();
        var bgColor = lobby[i].color;
        if(teamGame){
            bgColor = teamArray[lobby[i].team].color;
        }
        $('#spectatingOverlay').append(
            $('<tr>', {
                css: {
                    backgroundColor: hexToRgb(bgColor,cardOpacity)
                },
                id: lobby[i].name,
                class: 'player clickable',
                'data-uid': lobby[i].UID,
                'data-color': bgColor,
                'data-score':lobby[i].score,
                'data-playerIndex':lobby[i].playerIndex,
            })
            .append($('<td class="name">').text(lobby[i].name)) //name
        );   
		if(playersInfo[lobby[i].playerIndex]){
			emblemPath = playersInfo[lobby[i].playerIndex].e;
		}else{
			emblemPath = 'dew://assets/emblems/generic.png'; 
		}                
        $("[data-playerIndex='" + lobby[i].playerIndex + "'] .name").prepend('<img class="emblem" src="'+emblemPath+'">');
        $("[data-playerIndex='" + lobby[i].playerIndex + "'] .name").append('<img class="arrows" src="sort_both.png">');
		if(lobby[i].isHost)
		$("[data-playerIndex='" + lobby[i].playerIndex + "'] .name").append('<img class="emblem" src="'+hostPath+'" style="float: right;">');
    }  
}

function hexToRgb(hex, opacity){
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return "rgba(" + parseInt(result[1], 16) + "," + parseInt(result[2], 16) + "," + parseInt(result[3], 16) + "," + opacity + ")";
}