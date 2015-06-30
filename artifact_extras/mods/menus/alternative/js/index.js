var id = 0;
var windowW = $(window).width();

$(document).ready(function() {
  getServers();
  if (windowW <= 768) {
    $("body").append('<div class="alert"><h1>Small Screen Detected!</h1><span>We\'ve detected a small screen, this website may not scale correctly.');
    setTimeout(function() {
      $('.alert').toggleClass("hide");
      setTimeout(function() {
        $('.alert').remove();
      }, 6000);
    }, 5000);
  }
  $(document).keypress(function(e) {
    if (e.which == 13) {
      genRandom();
    }
  });
  //addServer("127.0.0.1", "false", "uplusion23's Server", "uplusion23", "Foundry", "Fat Man v2", "Fat Man", "inLobby", "14", "16", "0ms");
  $(document).on('click', ".list .item .section", function() {
    if ($(this).parent().hasClass("active")) {
      // Do Nothing.
    } else {
      var itemID = $(this).attr("id");
      $(this).parent().toggleClass("active");
    }
  });
  $(document).on('click', ".content .head .close", function() {
    var itemID = $(this).parent().parent().parent().attr("id");
    $(".list .item.active#" + itemID).toggleClass("active");
  });
  $(".menu").click(function() {
    $(".cover").toggleClass("show");
    $(".controls").toggleClass("active");
    $(".menu").toggleClass("active");
  });
  $(".cover").click(function() {
    $(".cover").toggleClass("show");
    $(".controls").toggleClass("active");
    $(".menu").toggleClass("active");
  });
});

function addServer(ip, isPassworded, name, host, map, mapfile, gamemode, status, numplayers, maxplayers, ping) {
  id++;
  var str = '<div class="item" id="' + id + '">';
  str += '<div class="section first"><span>' + name + '</span></div>';
  str += '<div class="section second"><span>' + mapfile + '</span></div>';
  str += '<div class="section third"><span>' + gamemode + '</span></div>';
  str += '<div class="section fifth"><span>' + numplayers + '/' + maxplayers + '</span></div>';
  str += '<div class="section fourth"><span>' + ping + '</span></div>';

  str += '<div class="content">';
  //Start Head 
  str += '<div class="head"><div class="close"><i class="zmdi zmdi-close"></i></div>';
  str += '<div class="primary"><h1>' + name + '</h1><h2>' + gamemode + '</h2><h3>on</h3><h2>' + mapfile + '</h2></div>';
  str += '</div>';
  //End Head

  str += '<div class="one-of-three"><img src="http://thefeeltrain.github.io/img/maps/' + mapfile.toUpperCase() + '.png" onerror="this.src=\'http://i.imgur.com/1gFcWCI.png\';" height="200" width="450"></div>';
  str += '<div class="one-of-three block">';
  str += '<a class="join-btn btn" href="#" onclick=\'callbacks.connect("' + ip + '");\'>Join Server</a>';
  str += '</div>';
  str += '<div class="one-of-three block"></div>';
  str += '</div>'; // Close Content

  str += '</div>';
  $(".list").append(str);
}

$("#refresh").click(function() {
  $(".item").remove();
  getServers();
});

function getServers() {
  $.getJSON("http://192.99.124.162/list", function(data) {
    if (data.result.code != 0) {
      $("body").append('<div class="alert"><h1>Error received from master</h1><span>' + data.result.msg + '</span></div>');
      return;
    }
    console.log(data);
    for (var i = 0; i < data.result.servers.length; i++) {
      var serverIP = data.result.servers[i];
      queryServer(serverIP);
    }
  });
}
 
function promptPassword(serverIP) {
    var password = prompt("The server at " + serverIP + " is passworded, enter the password to join", "");
    if(password != null)
    {
        window.open("dorito:" + serverIP + "/" + password);
    }
}

function queryServer(serverIP) {
  console.log(serverIP);
  var startTime = Date.now();
  $.getJSON("http://" + serverIP, function(serverInfo) {
    var timeTaken = Date.now() - startTime;
    console.log(timeTaken);
    if (serverInfo.name === undefined) return;
    var isPassworded = serverInfo.passworded !== undefined;
    addServer(serverIP, isPassworded, serverInfo.name, serverInfo.hostPlayer, serverInfo.map, serverInfo.mapFile, serverInfo.variant, serverInfo.status, serverInfo.numPlayers, serverInfo.maxPlayers, timeTaken);
    console.log(serverInfo);
  });
}