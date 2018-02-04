var DISCORD_DECLINE = 0;
var DISCORD_ACCEPT = 1;
var DISCORD_IGNORE = 2;

var playersJoinRequests = 0;
var isFirstShow = true;

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
		$("#notification").text(playersJoinRequests + " player(s) asking to join");
		$("#notification").show();
	}
	else{
		$("#notification").hide();
	}
}

$(document).ready(function () {
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
		}
	});
	
    dew.on("discord-joinrequest", function (e) {
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
		$("#notification").text(playersJoinRequests + " player(s) asking to join");
		
		setTimeout(function(){
			fadeOut(img, DISCORD_IGNORE);
		}, 30000); //Discord invites last 30 seconds
		
		user.appendTo($('#discord-container'));
    });
	
	$(document).keydown(function(e){
		$('#discord-container').css("top", -$('#discord-container').height());
		dew.captureInput(false);
		
		if(playersJoinRequests > 0) {
			$("#notification").text(playersJoinRequests + " player(s) asking to join");
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
	
	dew.show();
});