var peerCons = [];
var peerIds = [];
var localStream = null;
var serverCon;
var hasMic = false;
var peerConnectionConfig = {
    'iceServers': [{
            'url': 'stun:stun.services.mozilla.com'
        }, {
            'url': 'stun:stun.l.google.com:19302'
        }
    ]
};
var speaking = [];
var Microphone, EchoCancellation, AGC, NoiseSupress;

var audioCtx = new AudioContext();

function OnMessage(msg) {
    if (msg.data == "bad password") {
        console.log("bad password");
        return;
    } else if (msg.data = "try again later") {
        setTimeout(function () {
            dew.command("server.websocketinfo").then(function (resp) {
                var info = JSON.parse(resp);
                serverCon.send(info.password);
            });
        }, 5000);
    }

    var data = JSON.parse(msg.data);

    if (data.broadcast) {
        console.log("broadcast from:" + data.uid);
        //use the uid sent with every message as a key lookup
        if (!peerCons[data.uid]) //They aren't here, lets create a new RTC connection and send them an offer
        {
            createPeer(data);

            //create an offer
            peerCons[data.uid].createOffer().then(function (description) {
                peerCons[data.uid].setLocalDescription(description).then(function () {
                    console.log("sent offer to:" + data.uid);
                    serverCon.send(JSON.stringify({
                            'sdp': peerCons[data.uid].localDescription,
                            'sendTo': data.uid
                        }));
                });
            });
        }
    } else if (data.sendTo) //message direct to us
    {
        if (!peerCons[data.uid]) //we don't know them yet
        {
            createPeer(data);
        }

        if (data.sdp) {
            //whether we are getting an answer or offer, set remote peer description
            peerCons[data.uid].setRemoteDescription(new RTCSessionDescription(data.sdp)).then(function () {
                if (data.sdp.type == 'offer') {
                    console.log('got an offer from: ' + data.uid);
                    peerCons[data.uid].createAnswer().then(function (description) {
                        dew.command("voip.stereovoice", {}).then(function (stereoResponse) {
                            description.sdp = setCodecParam(description.sdp, 'opus/48000', 'stereo', stereoResponse);
                            peerCons[data.uid].setLocalDescription(description).then(function () {
                                serverCon.send(JSON.stringify({
                                        'sdp': peerCons[data.uid].localDescription,
                                        'sendTo': data.uid
                                    }));
                            });
                        });
                    });
                }
            });
        } else if (data.ice) {
            peerCons[data.uid].addIceCandidate(new RTCIceCandidate(data.ice));
        }
    } else if (data.leave) //leave has the uid of leaving peer
    {
        console.log("Asked to remove peer:" + data.leave);
        try {
            peerCons[data.leave].close();
        } catch (e) {
            console.log(e);
        }
        removePeer(data.leave);
    }
}

function createPeer(data) {
    peerCons[data.uid] = new RTCPeerConnection(peerConnectionConfig);
    if (hasMic)
        peerCons[data.uid].addStream(localStream);
    peerCons[data.uid].onaddstream = remotestream;
    peerCons[data.uid].onicecandidate = sendicecandidate;
    peerCons[data.uid].oniceconnectionstatechange = icechange;
    peerCons[data.uid].onclose = removePeer;
    peerCons[data.uid].connectedState = false;
    //peer information
    peerCons[data.uid].uid = data.uid;
    peerCons[data.uid].user = data.uid.split("|")[0];
    //speaking
    peerCons[data.uid].speakingVolume = -Infinity;
    peerCons[data.uid].lastSpoke = 0;
    //stream and processing
    peerCons[data.uid].gainNode = null;
    peerCons[data.uid].audioObj = null;

    peerIds.push(data.uid);
}

function peerLastSpeak(peer) {
    if ((new Date).getTime() - peer.lastSpoke > 2000)
        stopSpeak(peer.user, peer);
}

function removePeer(uid) {
    if (!uid)
        uid = this.uid; //if the close handler called us

    var index = peerIds.indexOf(uid);
    peerIds.splice(index, 1);

    delete peerCons[uid];
	dew.notify("voip-user-leave", {"user":uid});
    stopSpeak(uid.split("|")[0]);
}

function removeElement(element) {
    element && element.parentNode && element.parentNode.removeChild(element);
}

function icechange() {
    if (this.iceConnectionState == 'disconnected') {
        console.log(this.user + ": peer disconnected");
        removePeer(this.uid);
    }
}

function sendicecandidate(event) {
    if (event.candidate != null) {
        console.log('ice from: ' + this.uid);
        serverCon.send(JSON.stringify({
                'ice': event.candidate,
                'sendTo': this.uid
            }));
    } else //spec: null candidate means end of candidates
    {
        if (this.connectedState == false) { //something went wrong
            //restart ice
            var uid = this.uid;
            this.createOffer().then(function (description) {
                peerCons[uid].setLocalDescription(description).then(function () {
                    serverCon.send(JSON.stringify({
                            'sdp': peerCons[uid].localDescription,
                            'sendTo': uid
                        }));
                });
            });
        }
    }
}

function remotestream(event) {
    this.connectedState = true;
    console.log('got stream from: ' + this.uid);

    var username = this.user;
    var peer = this;

    peer.audioObj = new Audio();
    peer.audioObj.srcObject = event.stream;
    peer.gainNode = audioCtx.createGain();
    peer.gainNode.gain.value = 1.0;
    peer.audioObj.onloadedmetadata = function () {
        var source = audioCtx.createMediaStreamSource(peer.audioObj.srcObject);
        peer.audioObj.play();
        peer.audioObj.muted = true;
        source.connect(peer.gainNode);
        peer.gainNode.connect(audioCtx.destination);
    }

    this.speech = window.hark(event.stream, {
            "threshold": "-60",
			"interval": "250"
        });
    this.speech.on("speaking", function () {
        speak(username, peer);
    });
    this.speech.on("stopped_speaking", function () {
        stopSpeak(username, peer);
    });
    this.speech.on("volume_change", function (volume) {
        peer.speakingVolume = volume;
        var speaker = {
            user: username,
            volume: volume
        }
        peer.lastSpoke = (new Date).getTime();

        dew.notify("voip-user-volume", speaker);
    });
}

function addToSpeakingPlayersList(user) {
    //push everyone down
    if ($("#speaking > p").length > 0) {
        $("#speaking > p").each(function () {
            y = $(this).height() * ($(this).index() + 1);
            $(this).css({
                'transform': 'translate(' + 0 + 'px, ' + y + 'px)',
                'transition': 'all 300ms ease'
            });
        });
    }

    $("<p id=\"" + user + "\">" + user + "</p>").prependTo("#speaking").css({
        'transform': 'translate(0px, -100px)', //offscreen
        'transition': 'all 300ms ease'
    });

    setTimeout(function () { //drag in from offscreen
        $("#" + escapeElementID(user)).css({
            'transform': 'translate(0px, 0px)',
            'transition': 'all 300ms ease'
        });
    }, 25);
}

function escapeElementID(user) {
    return user.split(' ').join('\\ ');
}

function removeFromSpeakingPlayersList(user) {
    var thisUser = $("#" + escapeElementID(user));
    var index = thisUser.index();
    if ($("#speaking > p").length > 0) {
        $("#speaking > p").each(function () {
            if ($(this).index() < index)
                return; //this user does not need to move
            else if ($(this).index() > index) {
                y = $(this).height() * $(this).index() - $(this).height();
                $(this).css({
                    'transform': 'translate(' + 0 + 'px, ' + y + 'px)',
                    'transition': 'all 300ms ease'
                });
            } else {
                y = $(this).height() * ($(this).index() + 1) - $(this).height();
                thisUser.css({
                    'transform': 'translate(-200px, ' + y + 'px)', //offscreen
                    'transition': 'all 300ms ease'
                });
                setTimeout(function () {
                    thisUser.remove();
                }, 300);
            }
        });
    }
}

function populateSpeakingPlayersList() {
    speaking.forEach(function (element) {
        addToSpeakingPlayersList(element);
    });
}

function destroySpeakingPlayersList() {
    //Destroy names
    if ($("#speaking > p").length > 0) {
        $("#speaking > p").each(function () {
            $(this).remove();
        });
    }
}

//true if last time somebody talked/stopped talking it was shown on the HUD.
var previouslyDisplayedSpeakersOnHUD = false; //Mainmenu is overlay, so defaults to false.

function speak(user, peer) {
    var speaker = {
        user: user,
        volume: peer.speakingVolume,
        isSpeaking: true
    }
    dew.notify("voip-speaking", speaker);
    
    if(peer.gainNode.gain.value == 0) //prevent name from showing if they are muted
        return;
    
    dew.getSessionInfo().then(function (info) {
        dew.callMethod("isMapLoading", {}).then(function (mapLoadingRes) {
            dew.command('VoIP.SpeakingPlayerOnHUD').then(function (hudToggleRes) {

                dew.callMethod("playerSpeaking", {
                    "name": user,
                    "value": true
                });

                if ($.inArray(user, speaking) == -1) {

                    speaking.push(user);

                    //If the player is on the mainmenu, or doesn't want the speaking player to appear on the HUD, use the Overlay.
                    if (info.mapName == "mainmenu" || JSON.parse(mapLoadingRes).loading || hudToggleRes == 0) {
                        if (previouslyDisplayedSpeakersOnHUD)
                            populateSpeakingPlayersList();
                        else
                            addToSpeakingPlayersList(user);

                        previouslyDisplayedSpeakersOnHUD = false;
                    } else {
                        if (!previouslyDisplayedSpeakersOnHUD)
                            destroySpeakingPlayersList();
                        previouslyDisplayedSpeakersOnHUD = true;
                    }
                }
            });
        });
    });
};

function stopSpeak(user, peer) {
    var speaker = {
        user: user,
        volume: peer.speakingVolume,
        isSpeaking: false
    }

    dew.notify("voip-speaking", speaker);

    dew.getSessionInfo().then(function (info) {
        dew.callMethod("isMapLoading", {}).then(function (mapLoadingRes) {
            dew.command('VoIP.SpeakingPlayerOnHUD').then(function (hudToggleRes) {
                dew.callMethod("playerSpeaking", {
                    "name": user,
                    "value": false
                });

                var index = $.inArray(user, speaking);
                if (index != -1)
                    speaking.splice(index, 1);

                //If the player is on the mainmenu, or doesn't want the speaking player to appear on the HUD, use the Overlay.
                if (info.mapName == "mainmenu" || JSON.parse(mapLoadingRes).loading || hudToggleRes == 0) {
                    if (previouslyDisplayedSpeakersOnHUD)
                        populateSpeakingPlayersList();
                    else
                        removeFromSpeakingPlayersList(user);

                    previouslyDisplayedSpeakersOnHUD = false;
                } else {
                    if (!previouslyDisplayedSpeakersOnHUD)
                        destroySpeakingPlayersList();

                    previouslyDisplayedSpeakersOnHUD = true;
                }
            });
        });
    });
}

function clearConnection() {
    dew.callMethod("voipConnected", {
        "value": false
    });

    try {
        serverCon.close();
        serverCon = undefined;
    } catch (e) {
        console.log(e);
    }
    try {
        peerIds.forEach(function (id) {
            clearInterval(peerCons[id].interval);
            peerCons[id].close();
        });
        peerIds = [];
        peerCons = [];
    } catch (e) {}
}

function startConnection(info) {
    clearConnection();

    if (info.password == "") //not-connected
        return;

    dew.command("voip.microphoneid", {}).then(function (val) {
        navigator.mediaDevices.enumerateDevices().then(function (dev) {
            Microphone = val;
            console.log("Microphone: " + val);

            var constraints;
            if (Microphone == "" || Microphone == "default") {
                constraints = {
                    video: false,
                    audio: {
                        echoCancellation: EchoCancellation,
                        autoGainControl: AGC,
                        noiseSuppression: NoiseSupress
                    }
                };
            } else {
                var id;
                dev.forEach(function (d) {
                    if (d.label == Microphone)
                        id = d.deviceId;
                });
                console.log("used id: " + id);
                if (id == undefined) { //fallback to default
                    constraints = {
                        video: false,
                        audio: {
                            echoCancellation: EchoCancellation,
                            autoGainControl: AGC,
                            noiseSuppression: NoiseSupress
                        }
                    };
                } else {
                    constraints = {
                        video: false,
                        audio: {
                            deviceId: id,
                            echoCancellation: EchoCancellation,
                            autoGainControl: AGC,
                            noiseSuppression: NoiseSupress
                        }
                    };
                }
            }

            navigator.mediaDevices.getUserMedia(constraints).then(function (stream) {
                hasMic = true;
                localStream = stream;

                var speechEvents = hark(localStream, {
                        "threshold": "-60",
						"interval": "250"
                    });
                speechEvents.on('speaking', function () {
                    if (serverCon != undefined) {
                        dew.callMethod("voipSpeaking", {
                            "value": true
                        });
                    }
                });
                speechEvents.on('stopped_speaking', function () {
                    if (serverCon != undefined) {
                        dew.callMethod("voipSpeaking", {
                            "value": false
                        });
                    }
                });

                speechEvents.on("volume_change", function (volume) {
                    var selfVolume = {
                        volume: volume
                    }

                    dew.notify("voip-self-volume", selfVolume);
                });

                serverCon = new WebSocket("ws://" + info.server, "dew-voip");
                serverCon.onmessage = OnMessage;
                serverCon.onclose = function (reason) {
                    console.log("disconnected from signal server: " + reason.reason);
                    clearConnection();
                }
                serverCon.onopen = function () {
                    setTimeout(function () {
                        serverCon.send(info.password);
                        console.log("sent password");
                        serverCon.send(JSON.stringify({
                                "broadcast": "garbage"
                            }));
                        dew.callMethod("voipConnected", {
                            "value": true
                        });
                    }, 500);
                }

                dew.command("voip.update", {}).then(function () {}); //trigger initial voip variables
            }).catch (function (err) {
                hasMic = false;

                serverCon = new WebSocket("ws://" + info.server, "dew-voip");
                serverCon.onmessage = OnMessage;
                serverCon.onclose = function (reason) {
                    console.log("disconnected from signal server: " + reason.reason);
                    clearConnection();
                }
                serverCon.onopen = function () {
                    setTimeout(function () {
                        serverCon.send(info.password);
                        console.log("sent password");
                        serverCon.send(JSON.stringify({
                                "broadcast": "garbage"
                            }));
                        dew.callMethod("voipConnected", {
                            "value": true
                        });
                    }, 500);
                }

                dew.command("voip.update", {}).then(function () {}); //trigger initial voip variables
            });
        });
    });
}

function setVolume(uid, volume) {
    peerCons[uid].gainNode.gain.value = volume;
}

function retry() {
    dew.command("server.websocketinfo").then(function (resp) {
        var info = JSON.parse(resp);
        startConnection(info);
    });
}

function PTT(toggle) {
    localStream.getAudioTracks()[0].enabled = toggle.talk;
}

function updateSettings(settings) {
    try {
        if (settings.PTT_Enabled == 1) {
            localStream.getAudioTracks()[0].enabled = false;
        } else {
            localStream.getAudioTracks()[0].enabled = true;
        }
    } catch (e) {}

    Microphone = settings.MicrophoneID;
    EchoCancellation = settings.EchoCancellation;
    AGC = settings.AGC;
    NoiseSupress = settings.NoiseSupress;

    if (settings.Enabled == 0) {
        clearConnection();
    } else {
        if (serverCon == undefined) {
            dew.command("server.websocketinfo", {}).then(function (resp) {
                var info = JSON.parse(resp);
                startConnection(info);
            });
        }
    }
}

$(document).ready(function () {
    console.log("Waiting for signal server");
    dew.on("signal-ready", function (info) {
        console.log("signal ready");
        startConnection(info.data);
    });

    dew.on("voip-ptt", function (state) {
        PTT(state.data);
    });

    dew.on("voip-settings", function (response) {
        updateSettings(response.data);
    });

    dew.on("show", function (args) {
        if (args.data.volume) {
            setVolume(args.data.volume.uid, args.data.volume.vol);
        } else if (args.data.getPeers) {
            dew.notify("voip-peers", peerIds);
        }
    });
    dew.command("voip.update", {}).then(function () {}); //triggers update of settings
    dew.show();
    dew.getSessionInfo().then(function (info) {
        if (info.established == true) {
            retry();
        }
    });
});
