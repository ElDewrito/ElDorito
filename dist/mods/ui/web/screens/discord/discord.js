var DISCORD_DECLINE = 0;
var DISCORD_ACCEPT = 1;
var DISCORD_IGNORE = 2;

var playersJoinRequests = 0;
var isFirstShow = true;
var selectedIndex = -1;

var lastRequestUsername = "";

function fadeOut(userElement, reply) {
	playersJoinRequests--;
	dew.callMethod("discord-reply", {
		"userId": userElement.attr("userId"),
		"reply": reply
	});
	
	if(reply == DISCORD_ACCEPT)
		userElement.attr('src', 'dew://assets/discord/check.png');
	else if(reply == DISCORD_DECLINE)
		userElement.attr('src', 'dew://assets/discord/x.png');
	
	userElement.parent().fadeOut(500, function(){
		$(this).css({"visibility":"hidden",display:'block'}).slideUp();
	});
	
	setTimeout(function(){
		userElement.parent().remove();
	}, 600);
	
	if(playersJoinRequests > 0) {
		if(playersJoinRequests == 1){
			$("#notification").text(lastRequestUsername + " wants to join your game");
		}
		else{
			$("#notification").text(playersJoinRequests + " player(s) want to join your game");
		}
		$("#notification").show();
	}
	else{
		$("#notification").hide();
	}
}

$(window).resize(function(){
	$('#discord-container').css("top", -$('#discord-container').height());
});

$(document).ready(function () {
	$("#notification").hide();
	$.contextMenu({
		selector: '#discord-container .discord-user img',
		trigger: 'right',
		callback: function(key, options) {
			switch(key){
				case "Accept":
					fadeOut($(this), DISCORD_ACCEPT);
					return;
				case "Decline":
					fadeOut($(this), DISCORD_DECLINE);
					return;
				case "Ignore":
					fadeOut($(this), DISCORD_IGNORE);
					return;
			}
		},
		items: {
			"Accept": {name: "Accept"},
			"Decline": {name: "Decline"},
			"Ignore": {name: "Ignore"}
		}
	});
	
	dew.on("show", function(){
		if(isFirstShow){
			$('#discord-container').css("top", -$('#discord-container').height());
			$('#notification').text("temp");
			$("#notification").hide();
			isFirstShow = false;
			return;
		}
		else {
			$('#discord-container').css("top", "0px");
			$("#notification").hide();
			dew.captureInput(true);
			
			dew.command("settings.gamepad").then(function(resp){
				if(resp == 1){
					$('#discord-container .discord-user img').each(function(){
						$(this).removeClass("selected");
					});
					
					if($('#discord-container .discord-user img').length > 0){
						selectedIndex = 0;
						$('#discord-container .discord-user img').eq(0).addClass("selected");
					}
					else{
						selectedIndex = -1;
					}
				}
			});
		}
	});
	
    dew.on("discord-joinrequest", function (e) {
		lastRequestUsername = e.data.username + '#' + e.data.discriminator;
		var user = $('<div />', {
			class: 'discord-user'
		}).text(e.data.username + '#' + e.data.discriminator);
		
		var img;
		if(e.data.avatar != ""){
			img = $('<img />', {
				src: "https://cdn.discordapp.com/avatars/" + e.data.userId + "/" + e.data.avatar
			});
		}
		else{
			img = $('<img />', {
				src: "https://discordapp.com/assets/1cbd08c76f8af6dddce02c5138971129.png" //red default discord profile
			});
		}
		img.attr("userId", e.data.userId);
		
		img.click(function(){
			fadeOut(img, DISCORD_ACCEPT);
		});
		img.appendTo(user);
		
		playersJoinRequests++;
		$("#notification").show();
		if(playersJoinRequests == 1){
			$("#notification").text(lastRequestUsername + " wants to join your game");
		}
		else{
			$("#notification").text(playersJoinRequests + " player(s) want to join your game");
		}
		
		setTimeout(function(){
			fadeOut(img, DISCORD_IGNORE);
		}, 30000); //Discord invites last 30 seconds
		
		user.appendTo($('#discord-container'));
    });
	
	$(document).keydown(function(e){
		$('#discord-container').css("top", -$('#discord-container').height());
		dew.captureInput(false);
		
		if(playersJoinRequests > 0) {
			if(playersJoinRequests == 1){
				$("#notification").text(lastRequestUsername + " wants to join your game");
			}
			else{
				$("#notification").text(playersJoinRequests + " player(s) want to join your game");
			}
			$("#notification").show();
		}
		else{
			$("#notification").hide();
		}
		
		if(e.keyCode === 27){ //escape
			$('#discord-container').css("top", -$('#discord-container').height());
			dew.captureInput(false);
		}
	});
	
	dew.on('controllerinput', function(e){
		if(e.data.Left == 1){
			var selector = $('#discord-container .discord-user img');
			selector.eq(selectedIndex).removeClass("selected");
			if(selectedIndex == 0){
				selectedIndex = selector.length - 1;
			}
			else{
				selectedIndex--;
			}
			selector.eq(selectedIndex).addClass("selected");
		}
		else if(e.data.Right == 1){
			var selector = $('#discord-container .discord-user img');
			selector.eq(selectedIndex).removeClass("selected");
			if(selectedIndex == selector.length - 1){
				selectedIndex = 0;
			}
			else{
				selectedIndex++;
			}
			selector.eq(selectedIndex).addClass("selected");
		}
		else if(e.data.A == 1){
			fadeOut($('#discord-container .discord-user img').eq(selectedIndex), DISCORD_ACCEPT);
			if(selectedIndex > 0)
				selectedIndex--;
			$('#discord-container .discord-user img').eq(selectedIndex).addClass("selected");
		}
		else if(e.data.X == 1){
			fadeOut($('#discord-container .discord-user img').eq(selectedIndex), DISCORD_DECLINE);
			if(selectedIndex > 0)
				selectedIndex--;
			$('#discord-container .discord-user img').eq(selectedIndex).addClass("selected");
		}
		else if(e.data.Y == 1){
			fadeOut($('#discord-container .discord-user img').eq(selectedIndex), DISCORD_IGNORE);
			if(selectedIndex > 0)
				selectedIndex--;
			$('#discord-container .discord-user img').eq(selectedIndex).addClass("selected");
		}
		else if(e.data.B == 1){
			$('#discord-container').css("top", -$('#discord-container').height());
			dew.captureInput(false);
			
			if(playersJoinRequests > 0) {
				if(playersJoinRequests == 1){
					$("#notification").text(lastRequestUsername + " wants to join your game");
				}
				else{
					$("#notification").text(playersJoinRequests + " player(s) want to join your game");
				}
				$("#notification").show();
			}
			else{
				$("#notification").hide();
			}
		}
	});
	
	dew.show();
});