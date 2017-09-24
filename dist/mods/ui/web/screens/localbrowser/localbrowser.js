// UI and server info fetching code written by emoose
var serverlist = new Map();

function purge_old_servers(){
	var update_need = false
	for (var [server, last_broadcast] of serverlist) {
		if ((Date.now() - last_broadcast) > 3500) {
			serverlist.delete(server)
			update_need = true
		}
	}
	if (update_need)
		updateServerList();
}

dew.on("received_broadcast", function(e){
	var server_addr = e.data.server_address + ":" + e.data.info_server_port;
	var existing = serverlist.has(server_addr)
	serverlist.set(server_addr, Date.now())
	if (!existing)
		updateServerList();
})

dew.on("show", function(e){
	dew.command("lan_listener_start");
})

dew.on("hide", function(){
	dew.command("lan_listener_stop");
})

$("html").on("keydown", function(e) {
    if (e.key == "Escape"){
        dew.hide();
    } 
    if(e.keyCode == 192 || e.keyCode == 112 || e.keyCode == 223){
        dew.show("console");
    }
});

window.setInterval(purge_old_servers, 1000);
window.setInterval(updateServerList, 10000); // refresh user data

$("#refresh").click(function() {
    updateServerList();
});

function connect_to_server(join_string)
{
	dew.command("connect " + join_string);
	dew.hide();
}

function updateServerList() {
    $("#serverlist > tbody").empty();

	for (var [server, last_broadcast] of serverlist) {
		queryServer(server);
	}
}
    
function queryServer(serverIP) {
    var startTime = Date.now();
    $.getJSON("http://" + serverIP, function(serverInfo) {
        var timeTaken = Date.now() - startTime;
        console.log(timeTaken);
        if(serverInfo.name === undefined) return;
        var isPassworded = serverInfo.passworded !== undefined;
        //if no serverInfo.map, they jumped into an active game without unannouncing their server, causing a duplicate unjoinable game
        if(!serverInfo.map) return;
        
	    //if any variables include js tags, skip them
	    if(!invalidServer(serverInfo.name, serverInfo.variant, serverInfo.variantType, serverInfo.mapFile, serverInfo.maxPlayers, serverInfo.numPlayers, serverInfo.hostPlayer)) {
            addServer(serverIP, isPassworded, serverInfo.name, serverInfo.hostPlayer, serverInfo.map, serverInfo.mapFile, serverInfo.variant, serverInfo.status, serverInfo.numPlayers, serverInfo.maxPlayers, serverInfo.eldewritoVersion, timeTaken);
        }
    });
}

function promptPassword(serverIP) {
    var password = prompt("The server at " + serverIP + " is passworded, enter the password to join", "");
    if(password != "")
        connect_to_server(serverIP + ' ' + password);
}

function sanitizeString(str) {
    return String(str).replace(/(<([^>]+)>)/ig,"") //shouldn't need to strip tags with the below replacements, but I'll keep it anyway
                      .replace(/&/g, '&amp;')
                      .replace(/</g, '&lt;')
                      .replace(/>/g, '&gt;')
                      .replace(/'/g, '&#39;')
                      .replace(/"/g, '&quot;');
}

function invalidServer() {
    if (/[<>]/.test(Array.prototype.slice.call(arguments).join(''))) {
        console.log("Javascript potentially in one of the variables, skipping server");
        return true;
    } else {
        return false;
    }
}

function addServer(ip, isPassworded, name, host, map, mapfile, gamemode, status, numplayers, maxplayers, version, ping) {
    //because people can't be trusted with html, filter it out
    name = sanitizeString(name).substring(0,50);
    host = sanitizeString(host).substring(0,50);
    map = sanitizeString(map).substring(0,50);
    mapfile = sanitizeString(mapfile).substring(0,50);
    gamemode = sanitizeString(gamemode).substring(0,50);
    status = sanitizeString(status).substring(0,50);
    numplayers = parseInt(numplayers);
    maxplayers = parseInt(maxplayers);
    version = sanitizeString(version).substring(0, 10);

    if (isPassworded) name = '[\uD83D\uDD12] ' + name;

    if (version) name = '[' + version + '] ' + name;

    var servName = "<td>" + name  + " <b>(" +  host + "</b>)" + "</td>";
    var servMap = "<td>" + map + " (" + mapfile + ")" +  "</td>";
    var servGameType = "<td>" + gamemode + "</br>" + "</td>";
    var servIP = "<td>" + ip + "</td>";
    var servStatus = "<td>" + status + "</td>";
    var servPlayers = "<td>" + numplayers + "/" + maxplayers + "</td>";
    
	var onclick = (isPassworded ? 'promptPassword' : 'connect_to_server') + "('" + ip + "');";

    //$('#serverlist tr:last').after
    $('#serverlist > tbody').append("<tr onclick=\"" + onclick + "\">" + servName + servIP + servGameType + servMap +   servStatus + servPlayers + "</tr>");
}