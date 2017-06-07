var peerCons = [];
var localStream = null;
var serverCon;
var peerConnectionConfig = {'iceServers': [{'url': 'stun:stun.services.mozilla.com'}, {'url': 'stun:stun.l.google.com:19302'}]};
var speaking = [];

function OnMessage(msg)
{
	if(msg.data == "bad password")
	{
		console.log("bad password");
		return;
	}
	
	var data = JSON.parse(msg.data);
	
	if(data.broadcast)
	{
		console.log("broadcast from:" + data.uid);
		//use the uid sent with every message as a key lookup
		if(!peerCons[data.uid]) //They aren't here, lets create a new RTC connection and send them an offer
		{
			createPeer(data);
			
			//create an offer
			peerCons[data.uid].createOffer().then(function(description)
			{
				peerCons[data.uid].setLocalDescription(description).then(function()
				{
					serverCon.send(JSON.stringify({
						'sdp': peerCons[data.uid].localDescription,
						'sendTo': data.uid
					}));
				});
			});
			
		}
	}
	else if(data.sendTo) //message direct to us
	{
		if(!peerCons[data.uid]) //we don't know them yet
		{
			createPeer(data);
		}
		
		if(data.sdp)
		{
			//whether we are getting an answer or offer, set remote peer description
			peerCons[data.uid].setRemoteDescription(new RTCSessionDescription(data.sdp)).then(function()
			{
				if(data.sdp.type == 'offer')
				{
					console.log('got an offer from: ' + data.uid);
					peerCons[data.uid].createAnswer().then(function(description)
					{
						peerCons[data.uid].setLocalDescription(description).then(function()
						{
							//respond with our answer
							serverCon.send(JSON.stringify({
								'sdp': peerCons[data.uid].localDescription,
								'sendTo': data.uid
							}));
						});
					});
				}
			});
		}
		else if(data.ice)
		{
			console.log(data.ice);
			peerCons[data.uid].addIceCandidate(new RTCIceCandidate(data.ice));
		}
	}
	else if(data.leave) //leave has the uid of leaving peer
	{
		console.log("Asked to remove peer:" + data.leave);
		try{
			peerCons[data.leave].close();
		}
		catch(e){
			console.log(e);
		}
		removePeer(data.leave);
	}
}

function createPeer(data)
{
	peerCons[data.uid] = new RTCPeerConnection(peerConnectionConfig);
	peerCons[data.uid].addStream(localStream);
	peerCons[data.uid].onaddstream = remotestream;
	peerCons[data.uid].onicecandidate = sendicecandidate;
	peerCons[data.uid].oniceconnectionstatechange = icechange;
	peerCons[data.uid].connectedState = false;
	peerCons[data.uid].uid = data.uid;
	peerCons[data.uid].user = data.uid.split("|")[0];
	peerCons[data.uid].onclose = removePeer;
}

function removePeer(uid)
{
	if(!uid)
		uid = this.uid; //if the close handler called us
	
	removeElement(document.getElementById(uid));
	
	stopSpeak(uid.split("|")[0]);
	
	delete peerCons[uid];
	
	console.log("rtc peers listing");
	console.log(peerCons);
}

function removeElement(element) {
    element && element.parentNode && element.parentNode.removeChild(element);
}

function icechange()
{
	if(this.iceConnectionState == 'disconnected')
	{
		console.log("peer disconnected");
		removePeer(this.uid);
	}
}

function sendicecandidate(event)
{
	if(event.candidate != null)
	{
		console.log('uid from: ' + this.uid);
		
		serverCon.send(JSON.stringify({
			'ice': event.candidate,
			'sendTo': this.uid
		}));
	}
}

function remotestream(event)
{
	console.log('got stream from: ' + this.uid);
	$("#remoteVideos").append("<video id=\"" + this.uid + "\" autoplay=\"true\"></video>");
	document.getElementById(this.uid).src = window.URL.createObjectURL(event.stream);
	this.speech = window.hark(event.stream, {"threshold":"-60"});
	var username = this.user;
	this.speech.on("speaking", function(){
		speak(username);
	});
	this.speech.on("stopped_speaking", function(){
		stopSpeak(username);
	});
}

function speak(user)
{
	console.log(user + " speaking");
	if($.inArray(user, speaking) == -1)
	{
		speaking.push(user);
	}
	updateDisplay();
}

function stopSpeak(user)
{
	console.log(user + " stopped speaking");
	var index = $.inArray(user, speaking);
	if(index != -1)
		speaking.splice(index, 1);
	updateDisplay();
}

function updateDisplay()
{
	speaking.sort();
	$("#speaking").html('');
	for(var i = 0; i < speaking.length; i++)
	{
		$("#speaking").append("<p>" + speaking[i] + "</p>");
	}
}

function clearConnection()
{
	try{
		serverCon.close();
	}
	catch(e){
		console.log(e);
	}
	try
	{
		peerCons.forEach(function(peer){
			peer.close(); //our onclose handler should clean the video elements
		});
		peerCons = [];
	}
	catch(e){
		console.log(e);
	}
}

function startConnection(info)
{
	clearConnection();//make sure
	
	serverCon = new WebSocket("ws://" + info.server, "dew-voip");
	serverCon.onmessage = OnMessage;
	serverCon.onclose = function()
	{
		console.log("disconnected from signal server");
		clearConnection();
	}
	serverCon.onopen = function()
	{
		//must send the password before the server will accept anything from us
		serverCon.send(info.password);
		console.log("sent password");
		serverCon.send(JSON.stringify(
		{
			"broadcast": "garbage"
		}));
	}
	
	var constraints = {
		video: false,
		audio: true
	};
	
	navigator.mediaDevices.getUserMedia(constraints).then(function(stream){
		localStream = stream;
	});
}

function setVolume(uid, volume)
{
	document.getElementById(uid).volume = volume;
}

function retry()
{
	dew.command("server.websocketinfo").then(function(resp){
		var info = JSON.parse(resp);
		startConnection(info);
	});
}

$(document).ready(function(){
	console.log("waiting for signal server");
	dew.on("signal-ready", function(info){
		console.log("signal ready");
		startConnection(info.data);
	});
	
	dew.on("show", function(args){
		if(args.data.volume){
			setVolume(args.data.volume.uid, args.data.volume.vol);
		}
	});
	dew.show();
});
