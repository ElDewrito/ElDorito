var locked = false;
var isHost = false;
var cardOpacity = 0.9;
var medalsPath = 'medals://';
var imageFormat = 'svg';
var itemNumber = 0;
var controllerType;
var hasGP = false;
var talkingArray = [];
var isVisible = false;
var lobbyJSON;
var expandedScoreboard;
var axisThreshold = .5;
var stickTicks = { left: 0, right: 0, up: 0, down: 0 };
var repGP;
var lastHeldUpdated = 0;
var mapName;
var playerVolumes = {};
var scoreboardData = null;
var multiRound = false;
var lastRound = true;
var postgame = false;

var lastActivePlayerSet = 0;
var lastPlayerHasObjectiveSet = 0;
var lastSortTime = 0;
var lastAlivePlayerSet = 0;
var cachedPlayersInfo = null;
var medalSelection = 0;

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

var weaponDetails = [
    {name:'Guardians', 'string':'GuardiansUnknown'},
    {name:'Guardians', 'string':'Guardians'},
    {name:'Falling Damage', 'string':'FallingDamage'},
    {name:'Generic Collision', 'string':'GenericCollision'},
    {name:'Armor Lock Crush', 'string':'ArmorLockCrush'},
    {name:'Generic Melee', 'string':'GenericMelee'},
    {name:'Generic Explosion', 'string':'GenericExplosion'},
    {name:'Magnum', 'string':'Magnum'},
    {name:'Plasma Pistol', 'string':'PlasmaPistol'},
    {name:'Needler', 'string':'Needler'},
    {name:'Mauler', 'string':'Mauler'},
    {name:'SMG', 'string':'SMG'},
    {name:'Plasma Rifle', 'string':'PlasmaRifle'},
    {name:'Battle Rifle', 'string':'BattleRifle'},
    {name:'Covenant Carbine', 'string':'CovenantCarbine'},
    {name:'Shotgun', 'string':'Shotgun'},
    {name:'Sniper Rifle', 'string':'SniperRifle'},
    {name:'Beam Rifle', 'string':'BeamRifle'},
    {name:'Assault Rifle', 'string':'AssaultRifle'},
    {name:'Spiker', 'string':'Spiker'},
    {name:'Fuel Rod Cannon', 'string':'FuelRodCannon'},
    {name:'Missile Pod', 'string':'MissilePod'},
    {name:'Rocket Launcher', 'string':'RocketLauncher'},
    {name:'Spartan Laser', 'string':'SpartanLaser'},
    {name:'Brute Shot', 'string':'BruteShot'},
    {name:'Flamethrower', 'string':'Flamethrower'},
    {name:'Sentinel Gun', 'string':'SentinelGun'},
    {name:'Energy Sword', 'string':'EnergySword'},
    {name:'Gravity Hammer', 'string':'GravityHammer'},
    {name:'Frag Grenade', 'string':'FragGrenade'},
    {name:'Plasma Grenade', 'string':'PlasmaGrenade'},
    {name:'Spike Grenade', 'string':'SpikeGrenade'},
    {name:'Firebomb Grenade', 'string':'FirebombGrenade'},
    {name:'Flag', 'string':'Flag'},
    {name:'Bomb', 'string':'Bomb'},
    {name:'Bomb Explosion', 'string':'BombExplosion'},
    {name:'Ball', 'string':'Ball'},
    {name:'Machinegun Turret', 'string':'MachinegunTurret'},
    {name:'Plasma Cannon', 'string':'PlasmaCannon'},
    {name:'Plasma Mortar', 'string':'PlasmaMortar'},
    {name:'Plasma Turret', 'string':'PlasmaTurret'},
    {name:'Shade Turret', 'string':'ShadeTurret'},
    {name:'Banshee', 'string':'Banshee'},
    {name:'Ghost', 'string':'Ghost'},
    {name:'Mongoose', 'string':'Mongoose'},
    {name:'Scorpion', 'string':'Scorpion'},
    {name:'Scorpion Gunner', 'string':'ScorpionGunner'},
    {name:'Spectre', 'string':'Spectre'},
    {name:'Spectre Gunner', 'string':'SpectreGunner'},
    {name:'Warthog', 'string':'Warthog'},
    {name:'Warthog Gunner', 'string':'WarthogGunner'},
    {name:'Warthog Gauss Turret', 'string':'WarthogGaussTurret'},
    {name:'Wraith', 'string':'Wraith'},
    {name:'Wraith Gunner', 'string':'WraithGunner'},
    {name:'Tank', 'string':'Tank'},
    {name:'Chopper', 'string':'Chopper'},
    {name:'Hornet', 'string':'Hornet'},
    {name:'Mantis', 'string':'Mantis'},
    {name:'Prowler', 'string':'Prowler'},
    {name:'Sentinel Beam', 'string':'SentinelBeam'},
    {name:'Sentinel RPG', 'string':'SentinelRPG'},
    {name:'Teleporter', 'string':'Teleporter'},
    {name:'Tripmine', 'string':'Tripmine'},
    {name:'DMR', 'string':'DMR'}
];

var medalDetails = [
    {name:'Unknown1', 'string':'Unknown1', 'desc':'Last Man Standing?'},
    {name:'Unknown4', 'string':'Unknown4', 'desc':'Flag Kill?'},
    {name:'Perfection!', 'string':'perfection', 'desc':'Win a Slayer game without dying with at least 15 kills.'},
    {name:'Extermination!', 'string':'extermination', 'desc':'Wipe out an enemy team with at least an overkill.'},
    {name:'Killing Spree!', 'string':'killing_spree', 'desc':'Kill 5 opponents in a row without dying.'},
    {name:'Killing Frenzy!', 'string':'killing_frenzy', 'desc':'Kill 10 opponents in a row without dying.'},
    {name:'Running Riot!', 'string':'running_riot', 'desc':'Kill 15 opponents in a row without dying.'},
    {name:'Rampage!', 'string':'rampage', 'desc':'Kill 20 opponents in a row without dying.'},
    {name:'Untouchable!', 'string':'untouchable', 'desc':'Kill 25 opponents in a row without dying.'},
    {name:'Invincible!', 'string':'invincible', 'desc':'Kill 30 opponents in a row without dying.'},
    {name:'Shotgun Spree!', 'string':'shotgun_spree', 'desc':'Kill 5 opponents in a row with the shotgun without dying.'},
    {name:'Sword Spree!', 'string':'sword_spree', 'desc':'Kill 5 opponents in a row with the Energy Sword without dying.'},
    {name:'Sniper Spree!', 'string':'sniper_spree', 'desc':'Kill 5 opponents in a row with a sniper weapon without dying.'},
    {name:'Splatter Spree!', 'string':'splatter_spree', 'desc':'Splatter 5 opponents in a row using a vehicle without dying.'},
    {name:'Open Season!', 'string':'open_season', 'desc':'Kill 10 opponents in a row with the shotgun without dying.'},
    {name:'Slice n Dice!', 'string':'slice_n_dice', 'desc':'Kill 10 opponents in a row with the Energy Sword without dying.'},
    {name:'Sharpshooter!', 'string':'sharpshooter', 'desc':'Kill 10 opponents in a row with a sniper weapon without dying.'},
    {name:'Vehicular Manslaughter!', 'string':'vehicular_manslaughter', 'desc':'Splatter 10 opponents in a row using a vehicle without dying.'},
    {name:'Double Kill!', 'string':'double_kill', 'desc':'Kill 2 opponents within 4 seconds of each other.'},
    {name:'Triple Kill!', 'string':'triple_kill', 'desc':'Kill 3 opponents within 4 seconds of each other.'},
    {name:'Overkill!', 'string':'overkill', 'desc':'Kill 4 opponents within 4 seconds of each other.'},
    {name:'Killtacular!', 'string':'killtacular', 'desc':'Kill 5 opponents within 4 seconds of each other.'},
    {name:'Killtrocity!', 'string':'killtrocity', 'desc':'Kill 6 opponents within 4 seconds of each other.'},
    {name:'Killimanjaro!', 'string':'killimanjaro', 'desc':'Kill 7 opponents within 4 seconds of each other.'},
    {name:'Killtastrophe!', 'string':'killtastrophe', 'desc':'Kill 8 opponents within 4 seconds of each other.'},
    {name:'Killpocalypse!', 'string':'killpocalypse', 'desc':'Kill 9 opponents within 4 seconds of each other.'},
    {name:'Killionaire!', 'string':'killionaire', 'desc':'Kill 10 opponents within 4 seconds of each other.'},
    {name:'Beat Down!', 'string':'beat_down', 'desc':'Hit and kill an opponent with a melee attack.'},
    {name:'Assassin!', 'string':'assassin', 'desc':'Hit and kill an opponent with a melee attack from behind.'},
    {name:'Sniper Kill!', 'string':'sniper_kill', 'desc':'Kill an opponent with a Sniper Rifle or the Beam Rifle.'},
    {name:'Grenade Stick!', 'string':'grenade_stick', 'desc':'Kill an opponent by sticking them with a sticky or brute grenade.'},
    {name:'Laser Kill!', 'string':'laser_kill', 'desc':'Kill an opponent by using the Spartan Laser.'},
    {name:'Oddball Kill!', 'string':'oddball_kill', 'desc':'Get a melee kill when holding the oddball.'},
    {name:'Flag Kill!', 'string':'flag_kill', 'desc':'Get a melee kill with the flag.'},
    {name:'Splatter!', 'string':'splatter', 'desc':'Hit and kill an opponent with your vehicle.'},
    {name:'Incineration!', 'string':'incineration', 'desc':'Kill an opponent with a flame-based weapon.'},
    {name:'Killjoy!', 'string':'killjoy', 'desc':'End an opponents killing spree.'},
    {name:'Death from the Grave!', 'string':'from_the_grave', 'desc':'Kill an opponent after you have died.'},
    {name:'Highjacker!', 'string':'hijacker', 'desc':'Board a land-based vehicle by forcibly removing the opponent in it.'},
    {name:'Bulltrue!', 'string':'bulltrue', 'desc':'Kill an opponent that is in the act of a sword lunge.'},
    {name:'Wheelman!', 'string':'wheelman', 'desc':'Be the driver of a vehicle when a passenger kills an opponent.'},
    {name:'Skyjacker!', 'string':'skyjacker', 'desc':'Board an aircraft by forcibly removing the opponent in it.'},
    {name:'Killed Flag Carrier!', 'string':'flag_carrier_kill', 'desc':'Kill the opponent flag carrier in CTF.'},
    {name:'Flag Score!', 'string':'flag_captured', 'desc':'Score the flag in CTF.'},
    {name:'Killed Juggernaut!', 'string':'juggernaut_kill', 'desc':'Kill the Juggernaut in a Juggernaut game.'},
    {name:'Killed VIP!', 'string':'vip_kill', 'desc':'Kill an opponents VIP in a VIP game.'},
    {name:'Killed Bomb Carrier!', 'string':'bomb_carrier_kill', 'desc':'Kill an opponent bomb carrier in Assault.'},
    {name:'Bomb Planted!', 'string':'bomb_planted', 'desc':'Plant the bomb in Assault.'},
    {name:'Last Man Standing!', 'string':'last_man_standing', 'desc':'Be the last human in an infection game.'},
    {name:'Hail to the King!', 'string':'hail_to_the_king', 'desc':'Kill 5 opponents in a row from inside the hill before it moves.'},
    {name:'Infection Spree! ', 'string':'infection_spree', 'desc':'Kill 5 humans in a row as the zombie without dying.'},
    {name:'Zombie Killing Spree!', 'string':'zombie_killing_spree', 'desc':'Kill 5 zombies in a row as the human without dying.'},
    {name:'Juggernaut Spree!', 'string':'juggernaut_spree', 'desc':'Kill 5 opponents in a row as the Juggernaut without dying.'},
    {name:'Mmmm Brains!', 'string':'mmm_brains', 'desc':'Kill 10 humans in a row as the zombie without dying.'},
    {name:'Hells Janitor!', 'string':'hells_janitor', 'desc':'Kill 10 zombies in a row as the human without dying.'},
    {name:'Unstoppable!', 'string':'unstoppable', 'desc':'Kill 10 opponents in a row as the Juggernaut without dying.'},
    {name:'Steaktacular!', 'string':'steaktacular', 'desc':'Luke owes you a steak dinner.'},
    {name:'Linktacular!', 'string':'linktacular', 'desc':'Play in a matchmade game comprised of all Bungie.net users.'},
    {name:'Killed Vehicle!', 'string':'vehicle_kill', 'desc':'Destroy an enemy vehicle.'},
    {name:'Headshot!', 'string':'headshot', 'desc':'Kill an enemy with a headshot.'}
];

var settingsArray = { 'Game.MedalPack': 'default', 'Settings.Gamepad': '0', 'Game.IconSet': '360'};

$(document).ready(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            if($('#playerBreakdown').is(":visible")){
                $('#playerBreakdown').hide();
            } else {
                hideScoreboard();
            }
        }
        if (e.keyCode == 44) {
            dew.command('Game.TakeScreenshot');  
        }
    });
    $(document).keydown(function(e){
        if(e.keyCode == 9|| e.keyCode == 13){
            hideScoreboard();
        } 
        if(e.keyCode == 84 || e.keyCode == 89){
            var teamChat = false;
            if(e.keyCode == 89){ teamChat = true };
            dew.show("chat", {'captureInput': true, 'teamChat': teamChat});
        }
        if(e.keyCode == 192 || e.keyCode == 112 || e.keyCode == 223){
            dew.show("console");
        }
    });
    $.contextMenu({
        selector: '.clickable', 
        callback: function(key, options){
            switch(key){
                case "kick":
                    dew.command("Server.KickUid " + $(this).attr('data-uid'));
                    break;
                case "ban":
                    dew.command("Server.KickBanUid " + $(this).attr('data-uid'));
                    break;
                case "mute":
                    setPlayerVolume($(this).attr('id'),$(this).attr('data-uid'),0);
                    break;
                case "unmute":
                    setPlayerVolume($(this).attr('id'),$(this).attr('data-uid'),100);
                    break;
                default:
                    console.log(key + " " + $(this).attr('data-name') + " " + $(this).attr('data-uid'));
            }
        },
        items: {
            "kick": {
                name: "Kick",                
                disabled: function(key, options){ 
                    return !isHost; 
                }
            },
            "ban": {
                name: "Ban",                
                disabled: function(key, options){
                    return !isHost; 
                }
            },
            "mute": {
                name: "Mute"
            },
            "unmute": {
                name: "Unmute"
            }
        }
    });
    $('#closeButton').on('click', function(e){
        hideScoreboard();
    });
    loadSettings(0);
});

function loadSettings(i){
	if (i != Object.keys(settingsArray).length) {
		dew.command(Object.keys(settingsArray)[i], {}).then(function(response) {
			settingsArray[Object.keys(settingsArray)[i]] = response;
			i++;
			loadSettings(i);
		});
	}
}

dew.on("variable_update", function(e){
    for(i = 0; i < e.data.length; i++){
        if(e.data[i].name in settingsArray){
            settingsArray[e.data[i].name] = e.data[i].value;
        }
    }
});

dew.on('controllerinput', function(e){     
    if(hasGP){
        if(e.data.A == 1){
            if(!$('#playerBreakdown').is(":visible")){
                var playerName = $('.clickable').eq(itemNumber).attr('id');
                playerBreakdown(playerName);
            }
        }
        if(e.data.B == 1){
            if($('#playerBreakdown').is(":visible")){
                $('#playerBreakdown').hide();
            } else {
                hideScoreboard(); 
            }
        }
        if (e.data.X == 1) {
            if (!$('#playerBreakdown').is(":visible")) {
                var volume = playerVolumes[$('.clickable').eq(itemNumber).attr('id')];
                if (volume == 0) {
                    setPlayerVolume($('.clickable').eq(itemNumber).attr('id'), $('.clickable').eq(itemNumber).attr('data-uid'), 100);
                } else {
                    setPlayerVolume($('.clickable').eq(itemNumber).attr('id'), $('.clickable').eq(itemNumber).attr('data-uid'), 0);
                };
            }
        }
        if(e.data.Up == 1) {
            upNav();
        }
        if(e.data.Down == 1){
            downNav();
        }
        if(e.data.Left == 1){
            leftNav();
        }
        if(e.data.Right == 1){
            rightNav();
        }
        if(e.data.LeftBumper == 1){
            if($('#playerBreakdown').is(":visible")){
                $('#previousPlayer').click();
            }
        }
        if(e.data.RightBumper == 1){
            if($('#playerBreakdown').is(":visible")){
                $('#nextPlayer').click();
            }
        }
        if(e.data.Select == 1){
            hideScoreboard();
        }
        if(e.data.AxisLeftX > axisThreshold){
            stickTicks.right++;
        }else{
            stickTicks.right = 0;
        }
        if(e.data.AxisLeftX < -axisThreshold){
            stickTicks.left++;
        }else{
            stickTicks.left = 0;
        }
        if(e.data.AxisLeftY > axisThreshold){
            stickTicks.up++;
        }else{
            stickTicks.up = 0;
        }
        if(e.data.AxisLeftY < -axisThreshold){
            stickTicks.down++;
        }else{
            stickTicks.down = 0;
        }
    }
});

function updateScoreboardPlayer(rowElement, player, playersInfo, gameType) {
    var patchSet = {
        kills: player.kills,
        assists: player.assists,
        deaths: player.deaths,
        bestStreak: player.bestStreak,
        score: player.score,
        totalScore: player.totalScore,
        kingsKilled: player.kingsKilled,
        ballKills: player.ballKills,
        zombiesKilled: player.zombiesKilled,
        humansInfected: player.humansInfected,
        timeInHill: player.timeInHill,
        timeControllingHill: player.timeControllingHill
    };

    var aliveStatusChanged = ((lastAlivePlayerSet >> player.playerIndex) & 1) != player.isAlive;
   
    var cells = rowElement[0].children;
    for(cell of cells){
        var element =  $(cell);
        var cellName = cell.dataset.name;
        if(cellName in patchSet) {   
            element.text(patchSet[cellName].toString());
        }

        if(aliveStatusChanged && cellName === 'name') {
            var emblemPath = null;
            if(!player.isAlive){
                emblemPath = 'dew://assets/emblems/dead.png';   
            } else{
                if(playersInfo[player.playerIndex]){
                    emblemPath = playersInfo[player.playerIndex].e;
                }else{
                    emblemPath = 'dew://assets/emblems/generic.png'; 
                }                
            }
            $(element.find('.player-emblem')[0]).attr('src',emblemPath);
        }
    }

    rowElement.attr('data-score', player.score);
}
    
dew.on("scoreboard", function(e){
    if(e.data.scoreboardData && e.data.scoreboardData.players){
        scoreboardData = e.data;
    }
    mapName = e.data.mapName
    if(e.data.numberOfRounds){
        multiRound = e.data.numberOfRounds == 1 ? false : true;
        lastRound = e.data.numberOfRounds == e.data.currentRound + 1 ? true : false;
    }
    if(isVisible){
         var activePlayerSet = 0;
         var alivePlayerSet = 0;
         var playerHasObjectiveSet = 0;
         var rebuildScoreboard = false;
         var teamsVisitedSet = 0;

         if(!scoreboardData || !scoreboardData.players)
            return;

         for(var i = 0; i < scoreboardData.players.length; i++) {         
             var player = scoreboardData.players[i];
             activePlayerSet |= (1 << player.playerIndex);
            
             var row = $(`[data-playerIndex=${player.playerIndex}]`);
             if(row.length === 0 || row[0].dataset.uid !== player.UID || player.team != parseInt(row[0].dataset.team)) {
                 rebuildScoreboard = true;
                 break;
             }
             else {
                 updateScoreboardPlayer(row, player, cachedPlayersInfo, scoreboardData.gameType);
             }

            if(player.isAlive)
                alivePlayerSet |= (1 << player.playerIndex);

            if(player.hasObjective)
                playerHasObjectiveSet |= (1 << player.playerIndex);

            if(scoreboardData.hasTeams) {
                if(!(teamsVisitedSet & (1 << player.team))) {
                    teamsVisitedSet |= (1 << player.team);

                    var tbody = $(`#${teamArray[player.team].name}`);
                    var row = tbody.find('.teamHeader');
                    if(row.length === 0)
                        continue;

                    tbody[0].dataset.score = scoreboardData.teamScores[player.team];

                    var patchSet = {
                        score: scoreboardData.teamScores[player.team],
                        totalScore: scoreboardData.totalScores[player.team]
                    };

                    var cells = row[0].children;
                    for(cell of cells){
                        var cellName = cell.dataset.name;
                        if(cellName in patchSet) {
                            $(cell).text(patchSet[cellName].toString());
                        }
                    }
                }
            }
         }

         if(lastAlivePlayerSet != alivePlayerSet) {
             lastAlivePlayerSet = alivePlayerSet;
         }
         if(lastActivePlayerSet != activePlayerSet) {
             rebuildScoreboard = true;
             lastActivePlayerSet = activePlayerSet;
         }
         if(lastPlayerHasObjectiveSet != playerHasObjectiveSet) {
            rebuildScoreboard = true;
            lastPlayerHasObjectiveSet = playerHasObjectiveSet;
         }
         
         if(rebuildScoreboard) {
             displayScoreboard();
             cachedPlayersInfo = JSON.parse(scoreboardData.playersInfo);
         }
         
         var now = Date.now();
         if(now - lastSortTime > 250) {
            lastSortTime = now;

            if(scoreboardData.hasTeams){
                sortMe('scoreboard','tbody',multiRound);
                for(var i = 0; i < 8; i++)
                    sortMe(teamArray[i].name, 'tr',multiRound);
            } else {
                sortMe('singlePlayers','tr',multiRound);
            }

            rankMe(scoreboardData.hasTeams,multiRound);
        }
    }
});

function escapeElementID(user) {
    return user.split(' ').join('\\ ');
}

dew.on("voip-user-volume", function(e){
	if(e.data.volume > -60){
        isSpeaking(e.data.user,true);
	}

    var playerVoip = playerVolumes[e.data.user];
    if (playerVoip) {
        playerVoip.currentVoice = e.data.volume;
		if(isVisible){
			var escapedName = escapeElementID(e.data.user); //make names with spaces safe for selector
			if (playerVoip.volume == 0) {
				$('#' + escapedName).find('.speaker').attr('src', 'dew://assets/emblems/speaker-mute.png');
			} else if (e.data.volume < -75) {
				$('#' + escapedName).find('.speaker').attr('src', 'dew://assets/emblems/speaker-off.png');
			} else if (e.data.volume < -50) {
				$('#' + escapedName).find('.speaker').attr('src', 'dew://assets/emblems/speaker-low.png');
			} else {
				$('#' + escapedName).find('.speaker').attr('src', 'dew://assets/emblems/speaker-full.png');
			}
		}
    }
    else {
        playerVolumes[e.data.user] = {};
        playerVolumes[e.data.user].volume = 100.0;
        playerVolumes[e.data.user].currentVoice = e.data.volume;
    }
});

dew.on("voip-self-volume", function (e) {
    dew.getSessionInfo().then(function (info) {
        if (info.established == true) {
            var playerVoip = playerVolumes[e.data.user];
            if (playerVoip) {
                playerVoip.currentVoice = e.data.volume;
            } else {
                playerVolumes[e.data.user] = {};
                playerVolumes[e.data.user].volume = 100.0;
                playerVolumes[e.data.user].currentVoice = e.data.volume;
            }
        }
    });
});
dew.on("voip-peers", function(e){
});

dew.on("voip-speaking", function(e){
});

dew.on("voip-user-leave", function(e){
	delete playerVolumes[e.data.user];
});

dew.on("show", function(e){
	postgame = e.data.postgame;
    isVisible = true;
    dew.captureInput(e.data.locked);
    locked = e.data.locked;
    if(e.data.locked){
        $('#closeButton').show();

            medalsPath = "medals://" + settingsArray['Game.MedalPack'] + "/";
            $.getJSON(medalsPath+'events.json', function(json) {
                eventJson = json;
                if(eventJson['settings']){
                    if(eventJson['settings'].hasOwnProperty('imageFormat')){
                        imageFormat = eventJson['settings'].imageFormat;
                    }
                }            
            });


            if(settingsArray['Settings.Gamepad'] == 1){
                onControllerConnect();
                hasGP = true;
                if(!repGP){
                    repGP = window.setInterval(checkGamepad,1000/60);
                }
            }else{
                onControllerDisconnect();
                hasGP = false;
                if(repGP){
                    window.clearInterval(repGP);
                    repGP = null;
                }
            }

        dew.command('Server.ListPlayersJSON').then(function(res){
            lobbyJSON = JSON.parse(res);
        });
    }else{
        $('#closeButton').hide();
    }
    if(e.data.postgame && lastRound){
        $('#winnerText').show();
    }else{
        $('#winnerText').hide();
    }
   
    dew.getSessionInfo().then(function(i){          
        isHost = i.isHost;
    });
    
    if(e.data.scoreboardData && e.data.scoreboardData.players){
        scoreboardData = e.data.scoreboardData;
    }
    expandedScoreboard = e.data.expanded;
    if(scoreboardData.playersInfo || cachedPlayersInfo){
        if(scoreboardData.playersInfo){
            cachedPlayersInfo = JSON.parse(scoreboardData.playersInfo);
        }
        displayScoreboard();
    }else{
        hideScoreboard();
    }
});

dew.on("hide", function(e){
    isVisible = false;
    $('#playerBreakdown').hide();    
    dew.captureInput(false);
    $('.context-menu-list').trigger("contextmenu:hide");
});

function displayScoreboard(){
    if(jQuery.isEmptyObject(scoreboardData))
        return;	
    var scoreboardheader = '<th></th><th class="name" colspan="2">Players</th>';
    if(locked || (expandedScoreboard == 1)){
        switch(scoreboardData.gameType){
            case "ctf":
                scoreboardheader += '<th>Kills</th><th>Flag Kills</th>';  
                break;
            case "oddball":
                scoreboardheader += '<th>Kills</th><th>Ball Kills</th>';    
                break;
            case "infection":
                scoreboardheader += '<th>Kills</th><th>Infections</th><th>Zombie Kills</th>';    
                break;
            case "koth":
                scoreboardheader += '<th>Kills</th><th>King Kills</th><th>Time In Hill</th><th>Controlling</th>';    
                break;
            case "vip":
            case "juggernaut":
            case "assault":
            default:
                scoreboardheader += '<th>Kills</th><th>Assists</th><th>Deaths</th><th>Best Streak</th>';          
        }
        $('#window').css({'width':'62%','left':'18%'});
    } else {
        $('#window').css({'width':'38%','left':'31%'});
    }
    scoreboardheader += '<th></th><th>Score</th>'; 
    if(multiRound){
        scoreboardheader += '<th>Round</th>';
    }
    $('#header').html(scoreboardheader);
    buildScoreboard(scoreboardData.players, scoreboardData.hasTeams, scoreboardData.teamScores, scoreboardData.gameType, JSON.parse(scoreboardData.playersInfo),expandedScoreboard, scoreboardData.teamHasObjective,scoreboardData.totalScores);
    dew.command("Server.NameClient", { internal: true }).then(function (name){
        $("#serverName").text(name);
    });    
    dew.getMapVariantInfo().then(function (info) {
        $("#mapName").text(info.name);
    });
    if(hasGP && locked){
        updateSelection(itemNumber);
    }
}

function buildScoreboard(lobby, teamGame, scoreArray, gameType, playersInfo,expandedScoreboard, teamHasObjective, totalArray){
    var emblemPath;
	var hostPath = 'dew://assets/emblems/crown.png';
	var rankPath = "dew://assets/ranks/0.png"
    var where = '#singlePlayers';
    if(lobby.length > 0){
        $('#singlePlayers').empty();
        $('.team').remove();
        for(var i=0; i < lobby.length; i++){
            var bgColor = lobby[i].color;
            if(teamGame){
                bgColor = teamArray[lobby[i].team].color;
                where = '#'+teamArray[lobby[i].team].name;
                if($(where).length == 0){
                    var teamHeader = '<tbody id="'+teamArray[lobby[i].team].name+'" data-score="'+scoreArray[lobby[i].team]+'" data-totalscore="'+totalArray[lobby[i].team]+'" data-teamIndex="'+lobby[i].team+'" class="team"><tr class="player teamHeader" style="background-color:'+hexToRgb(teamArray[lobby[i].team].color, cardOpacity)+';"><td class="rank"></td><td class="name">'+teamArray[lobby[i].team].name.toUpperCase()+' TEAM</td>';
                    if(multiRound){
                        teamHeader+='<td class="score" data-name="totalScore">'+totalArray[lobby[i].team]+'</td>';
                    }
                    teamHeader+='<td class="score" data-name="score">'+scoreArray[lobby[i].team]+'</td></tr></tbody>';
                    $('#window table').append(teamHeader);    
                    if((teamHasObjective & (1 << lobby[i].team))) {
                        $('#'+teamArray[lobby[i].team].name+' .teamHeader').append('<img class="emblem objective" src="dew://assets/emblems/'+gameType+'.png">')                    
                    }
                }    
            } 
            $(where).append(
                $('<tr>', {
                    css: {
                        backgroundColor: hexToRgb(bgColor,cardOpacity)
                    },
                    id: lobby[i].name,
                    class: 'player clickable',
                    'data-uid': lobby[i].UID,
                    'data-color': bgColor,
                    'data-score':lobby[i].score,
                    'data-totalScore':lobby[i].totalScore,
                    'data-playerIndex':lobby[i].playerIndex,
                    'data-team':lobby[i].team,
                }).mouseover(function(){
                    col = $(this).attr('data-color'),
                    bright = adjustColor(col, 30);
                    $(this).css("background-color", hexToRgb(bright, cardOpacity));
                }).mouseout(function(){
                    col = $(this).attr('data-color');
                    $(this).css("background-color", hexToRgb(col, cardOpacity));
                }).mouseup(function(e){
                    if(e.button !== 0)
                        return;
                    var playerName = $(this).attr('id');
                    playerBreakdown(playerName);
                }).append($('<td class="rank" data-name="rank">'))
                .append($('<td class="name" data-name="name">').text(lobby[i].name)) //name
            );   
			if(playersInfo[lobby[i].playerIndex]){
				rankPath = "dew://assets/ranks/" + playersInfo[lobby[i].playerIndex].r + ".png";
			}
            if(!lobby[i].isAlive && mapName != 'mainmenu'){
                emblemPath = 'dew://assets/emblems/dead.png';   
            }else{
				if(playersInfo[lobby[i].playerIndex]){
					emblemPath = playersInfo[lobby[i].playerIndex].e;
				}else{
					emblemPath = 'dew://assets/emblems/generic.png'; 
				}                
            } 
            $("[data-playerIndex='" + lobby[i].playerIndex + "'] .name").prepend('<img class="emblem player-emblem" src="'+emblemPath+'">');
			if(lobby[i].isHost)
			$("[data-playerIndex='" + lobby[i].playerIndex + "'] .name").append('<img class="emblem" style="float: right;" src="'+hostPath+'">');
            $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="serviceTag" data-name="serviceTag">').text(lobby[i].serviceTag))
            if(locked || (expandedScoreboard == 1)){
                switch(gameType){
                    case "oddball":
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat" data-name="kills">').text(lobby[i].kills)) //kills
                            .append($('<td class="stat">').text(lobby[i].ballKills)) //ball kills
                        $('.teamHeader .name').attr('colspan',5);
                        break;
                    case "infection":
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat" data-name="kills">').text(lobby[i].kills)) //kills
                            .append($('<td class="stat">').text(lobby[i].humansInfected)) //infected
                            .append($('<td class="stat">').text(lobby[i].zombiesKilled)) //zombies killed  
                        $('.teamHeader .name').attr('colspan',6);
                        break;
                    case "ctf":
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat" data-name="kills">').text(lobby[i].kills)) //kills
                            .append($('<td class="stat">').text(lobby[i].ballKills)) //flag kills
                        $('.teamHeader .name').attr('colspan',5);
                        break;
                    case "koth":
                       $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat" data-name="kills">').text(lobby[i].kills)) //kills
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat" data-name="kingsKilled">').text(lobby[i].kingsKilled)) //kings killed
                            .append($('<td class="stat" data-name="timeInHill">').text(lobby[i].timeInHill)) //time in hill
                            .append($('<td class="stat" data-name="timeControllingHill">').text(lobby[i].timeControllingHill)) //time controlling hill
                        $('.teamHeader .name').attr('colspan',7);
                        break;                   
                    case "vip":
                    case "juggernaut":
                    case "assault":
                    default:
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat" data-name="kills">').text(lobby[i].kills)) //kills
                            .append($('<td class="stat" data-name="assists">').text(lobby[i].assists)) //assists
                            .append($('<td class="stat" data-name="deaths">').text(lobby[i].deaths)) //deaths 
                            .append($('<td class="stat" data-name="bestStreak">').text(lobby[i].bestStreak)) //best streak 
                        $('.teamHeader .name').attr('colspan',7);
                }
            }
			else{
				$('.teamHeader .name').attr('colspan',3);
			}
            $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="playerRank" data-name="playerRank">'))
            if(multiRound){
                $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat score" data-name="totalScore">').text(lobby[i].totalScore)) //Total Score 
            }    
            $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat score" data-name="score">').text(lobby[i].score)) //score          
			$("[data-playerIndex='" + lobby[i].playerIndex + "'] .playerRank").prepend('<img class="rankimg" src="'+rankPath+'">');
			
			var thisSpeakerIcon = "";
			var isInVoip = false;
			var volumeLevel = 100;
			var playerVoip = playerVolumes[lobby[i].name];
			if (playerVoip) {
			    volumeLevel = playerVoip.volume;
			    isInVoip = true;
			    if (playerVoip.volume == 0)
			        thisSpeakerIcon = 'dew://assets/emblems/speaker-mute.png';
			    else if (playerVoip.currentVoice < -75)
			        thisSpeakerIcon = 'dew://assets/emblems/speaker-off.png';
			    else if (playerVoip.currentVoice < -50)
			        thisSpeakerIcon = 'dew://assets/emblems/speaker-low.png';
			    else
			        thisSpeakerIcon = 'dew://assets/emblems/speaker-full.png'
			}
			if(isInVoip){
			    $("[data-playerIndex='" + lobby[i].playerIndex + "'] .name").prepend($('<img class="emblem speaker talking" src="' + thisSpeakerIcon + '"><input class="volSlider" type="range" min="0" max="200" step="1" value="' + volumeLevel + '"></input>')) //voip speaking indicator
            }else{
				$("[data-playerIndex='" + lobby[i].playerIndex + "'] .name").prepend($('<img class="emblem speaker" src=""><input class="volSlider" type="range" min="0" max="200" step="1" value="100"></input>')) //voip speaking indicator
            }
            if(lobby[i].hasObjective){
                $('.objective').remove();
                if(gameType == "oddball"||gameType == "assault"||gameType == "ctf"){
                    $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<img class="emblem objective" src="dew://assets/emblems/'+gameType+'.png">')) //objective (flag/oddball) indicator
                }
            }
            if(teamGame){
                sortMe('scoreboard','tbody',multiRound);
                sortMe(teamArray[lobby[i].team].name,'tr',multiRound);
            } else {
                sortMe('singlePlayers','tr',multiRound);
            }
            rankMe(teamGame,multiRound);
        }      
        $('.volSlider').on('change click', function(e){
            e.stopPropagation();
            setPlayerVolume($(this).parent().parent().attr('id'),$(this).parent().parent().attr('data-uid'),$(this).val());
            displayScoreboard();
        });
        $('.speaker').on('click', function(e){
            e.stopPropagation();
            $(this).parent().find('.volSlider').show();

            var playerVoip = playerVolumes[$(this).parent().parent().attr('id')];
            if (playerVoip) {
                $(this).value(playerVoip.volume);
            }
        });
    }
}

function hexToRgb(hex, opacity){
    var r = parseInt(hex.substr(1,2), 16);
    var g = parseInt(hex.substr(3,2), 16);
    var b = parseInt(hex.substr(5,2), 16);
    return 'rgba('+ r + "," + g + "," + b + "," + opacity+")";
}

function sortMe(sortWhat, sortWhich, multiRound){
    var $wrapper = $('#'+sortWhat);
    $wrapper.find(sortWhich).sort(function(b, a) {
        if(multiRound && postgame && lastRound){
            var d = parseInt(a.dataset.totalScore) - parseInt(b.dataset.totalScore);
        }else{
            var d = parseInt(a.dataset.score) - parseInt(b.dataset.score);
        }
        if(d === 0) {
            if(sortWhat == "singlePlayers"){
                d = parseInt(a.dataset.playerindex) - parseInt(b.dataset.playerindex);
            }else{
                d = parseInt(a.dataset.team) - parseInt(b.dataset.team);              
            }
        }
        return d;
    }).appendTo($wrapper);
}

function rankMe(teamGame,multiRound){
    var outer = '.player';
    if(teamGame){
        outer = '.team';
        var inner = '.teamHeader';
    }        
    var x = 1;
    var winnerText;
    if(teamGame){
        winnerText = $(outer+':eq(0)').attr('id').substr(0,1).toUpperCase() + $(outer+':eq(0)').attr('id').substr(1)+' Team wins!';
    } else {
        winnerText = $(outer+':eq(0)').attr('id')+' wins!';
    }
    if(multiRound && postgame && lastRound){
        if($(outer).length > 1 && ($(outer+':eq(0)').attr('data-totalscore') == $(outer+':eq(1)').attr('data-totalscore'))){
            winnerText = 'Tie!';
        }
    }else{
        if($(outer).length > 1 && ($(outer+':eq(0)').attr('data-score') == $(outer+':eq(1)').attr('data-score'))){
            winnerText = 'Tie!';
        } 
    }
    $('#winnerText').text(winnerText);

    var lastScore = $(outer+':eq(0)').attr('data-score');
    for (i = 0; i < $(outer).length; i++){ 
        if($(outer+':eq('+i+')').attr('data-score') != lastScore){
            x=i+1;
            if(multiRound && postgame && lastRound){
                lastScore = $(outer+':eq('+i+')').attr('data-score');
            }else{
                lastScore = $(outer+':eq('+i+')').attr('data-totalscore');
            }
        }
        if(teamGame){
            $(outer).eq(i).find(inner).find('.rank').text(x);
        }else{
            $(outer).eq(i).find('.rank').text(x);           
        }
    }
}

function playerBreakdown(name){
    var playerIndex = lobbyJSON.findIndex(x => x.name == name);
    var color = hexToRgb(lobbyJSON[playerIndex].color, 0.9);
    if(cachedPlayersInfo[playerIndex]){
        emblemPath = cachedPlayersInfo[playerIndex].e;
    }else{
        emblemPath = 'dew://assets/emblems/generic.png'; 
    }     
    
    if(playerIndex==0){
        $('#previousPlayer').prop("disabled",true);
    }else{
        $('#previousPlayer').prop("disabled",false);      
        $('#previousPlayer').on('click', function(){
            $('#previousPlayer').off('click');
            playerBreakdown(lobbyJSON[playerIndex-1].name); 
        });            
    }
    if(playerIndex==(lobbyJSON.length-1)){
        $('#nextPlayer').prop("disabled",true);
    }else{
        $('#nextPlayer').prop("disabled",false);   
        $('#nextPlayer').on('click', function(){ 
            $('#nextPlayer').off('click');
            playerBreakdown(lobbyJSON[playerIndex+1].name); 
        });            
    }

    dew.getStats(name).then(function (stats){
        $('#playerBreakdown').show();
        medalSelection = 0;
        $('#playerName').text(name);  
        $('#playerName').css({'background-color': color});  
        $('#playerName').prepend('<img class="emblem player-emblem" src="'+emblemPath+'">');
        
        var weaponArray = $.map(stats.weapons, function(value, index){
            var tempArray = {'Weapon': index};
            for (var prop in value){
            tempArray[prop] = value[prop];
            }
            return tempArray;
        });
        
        var kills = 0;
        var deaths = 0;
        var suicides = 0;
        var headshots = 0;
        $.each( weaponArray, function( index, value ){
            kills += value.Kills;
            deaths += value.KilledBy;
            suicides += value.SuicidesWith;
            headshots += value.HeadshotsWith;
        });
        
        $('#playerKills').text("Kills: "+kills);  
        $('#playerDeaths').text("Deaths: "+deaths);
        $('#playerKDSpread').text("K/D Spread: "+(kills-deaths));
        $('#playerSuicides').text("Suicides: "+suicides);
    
        weaponArray.sort(function(b, a){
            return parseFloat(a.Kills) - parseFloat(b.Kills);
        });
        $('#toolOfDestruction tbody').empty();
        if(weaponArray[0].Kills!=0){
            $('#toolOfDestruction tbody').append(
                $('<tr>')
                .append($('<td>').text(weaponDetails[weaponDetails.findIndex(x => x.string == weaponArray[0].Weapon)].name))
                .append($('<td>').text("Kills: "+weaponArray[0].Kills))
            );
        }else{
            $('#toolOfDestruction tbody').append("<tr><td>None</td></tr>"); 
        } 
        
        $('#killedMost').hide();
        $('#killedMost tbody').empty();
        var playerKilledArray = stats.playerkilledplayer;
        if(playerKilledArray.length > 0){
            $('#killedMost').show();
            playerKilledArray.sort(function(b, a){
                return parseFloat(a.Kills) - parseFloat(b.Kills);
            }); 
            var enemyIndex = lobbyJSON.findIndex(x => x.name == playerKilledArray[0].PlayerName);
            var bgColor = "#C0C0C0";
            if(enemyIndex > -1){ bgColor = lobbyJSON[enemyIndex].color; };
            $('#killedMost tbody').append(
                $('<tr>', {
                    class: 'player', 
                    id: playerKilledArray[0].PlayerName,
                    css: {
                        backgroundColor: hexToRgb(bgColor, 0.9)
                    }
                }).click(function(){
                    playerBreakdown($(this).attr('id'));
                })
                .append($('<td>').text(playerKilledArray[0].PlayerName)) //name
                .append($('<td>').text(playerKilledArray[0].Kills)) //kills
            );
        }    		

        $('#killedMostBy').hide();          
        $('#killedMostBy tbody').empty();
        var killedByArray = stats.playerkilledby;	
        if(killedByArray.length > 0){		
            $('#killedMostBy').show(); 			
            killedByArray.sort(function(b, a){
                return parseFloat(a.Kills) - parseFloat(b.Kills);
            }); 
            var enemyIndex = lobbyJSON.findIndex(x => x.name == killedByArray[0].PlayerName);
            var bgColor = "#C0C0C0";
            if(enemyIndex > -1){ bgColor = lobbyJSON[enemyIndex].color; };
            $('#killedMostBy tbody').append(
                $('<tr>', {
                    class: 'player', 
                    id: killedByArray[0].PlayerName,
                    css: {
                        backgroundColor: hexToRgb(bgColor, 0.9)
                    }
                }).click(function(){
                    playerBreakdown($(this).attr('id'));
                })
                .append($('<td>').text(killedByArray[0].PlayerName)) //name
                .append($('<td>').text(killedByArray[0].Kills)) //kills
            );
        }

        var medalArray = $.map(stats.medals, function(value, index){
            return {'name':index, 'count':value};
        });
                
        medalArray.sort(function(b, a){
            return parseFloat(a.count) - parseFloat(b.count);
        }); 
        
        if(medalArray[0].count!=0){
            $('#bigMedal').css("background-image","url("+ medalsPath + "images/" + medalArray[0].name +"."+ imageFormat +")");    
            $('.medalName').text(medalDetails[medalDetails.findIndex(x => x.string == medalArray[0].name)].name);
            $('.medalDesc').text(medalDetails[medalDetails.findIndex(x => x.string == medalArray[0].name)].desc);
        }else{
            $('#bigMedal').css("background-image","none");    
            $('.medalName').text(null);
            $('.medalDesc').text(null);
        }
        $('#medalBox').empty();
        for(i=0;i<10;i++){
            if(medalArray[i].count!=0){
                $('#medalBox').append(
                    $('<div>', {
                        class: 'medal',
                        id: medalArray[i].name,
                        css: {
                            'background-image': "url("+ medalsPath + "images/" + medalArray[i].name +"."+ imageFormat +")"
                        },
                        html: "<span class='medalCount'>x "+medalArray[i].count+"</span>",
                        'data-medal-name': medalDetails[medalDetails.findIndex(x => x.string == medalArray[i].name)].name,
                        'data-medal-desc': medalDetails[medalDetails.findIndex(x => x.string == medalArray[i].name)].desc
                    }).mouseover(function(){
                        $('#bigMedal').css("background-image","url("+ medalsPath + "images/" + $(this).attr('id') +"."+ imageFormat +")");
                        $('.medalName').text($(this).attr('data-medal-name'));
                        $('.medalDesc').text($(this).attr('data-medal-desc'));
                    })
                );
            }
        }
        var fillerCount = 10 - $('#medalBox').children().length;
        for(i=0;i<fillerCount;i++){
            $('#medalBox').append('<div class="medal empty">');
        }
    })
}

function adjustColor(color, amount){
    var colorhex = (color.split("#")[1]).match(/.{2}/g);
    for (var i = 0; i < 3; i++){
        var e = parseInt(colorhex[i], 16);
        e += amount;
        if(amount > 0){
            colorhex[i] = ((e > 255) ? 255 : e).toString(16);
        }else{
            colorhex[i] = ((e < 0) ? 0 : e).toString(16);           
        }
    }
    return "#" + colorhex[0] + colorhex[1] + colorhex[2];
}

function onControllerConnect(){
    $('#closeButton').css('padding-right', '1.75vw');
    controllerType = settingsArray['Game.IconSet'];
    $('#closeButton .button').attr('src','dew://assets/buttons/'+controllerType+'_Back.png');
    $('#previousPlayer .button').attr('src','dew://assets/buttons/'+controllerType+'_LB.png');
    $('#nextPlayer .button').attr('src','dew://assets/buttons/'+controllerType+'_RB.png');
    $('#windowClose .button').attr('src','dew://assets/buttons/'+controllerType+'_B.png');
    $('.button').show();
}

function onControllerDisconnect(){
    $('#closeButton').css('padding-right', '0');  
    $('#closeButton .button').hide();   
    if($('.clickable').length){
        for(var i = 0; i < $('.clickable').length; i++) {
            $('.clickable').eq(i).css("background-color", hexToRgb($('.clickable').eq(i).attr('data-color'), cardOpacity));
        }
    }    
}

function updateSelection(item){
     if($('#playerBreakdown').is(":visible")){
        $('.medal').css('background-color','rgba(0, 0, 0, 0.3)');
        $('.medal').eq(item).css('background-color','rgba(255, 255, 255, 0.3)');
        if(!$('.medal').eq(item).hasClass('empty')){
            $('#bigMedal').css("background-image","url("+ medalsPath + "images/" + $('.medal').eq(item).attr('id') +"."+ imageFormat +")");
            $('.medalName').text($('.medal').eq(item).attr('data-medal-name'));
            $('.medalDesc').text($('.medal').eq(item).attr('data-medal-desc'));
        }
     }else{
        if($('.clickable').length){
            for(var i = 0; i < $('.clickable').length; i++) {
                $('.clickable').eq(i).css("background-color", hexToRgb($('.clickable').eq(i).attr('data-color'), cardOpacity));
            }
            col = $('.clickable').eq(item).attr('data-color'),
            bright = adjustColor(col, 30);
            $('.clickable').eq(item).css("background-color", hexToRgb(bright, cardOpacity));
        }
     }
}

function upNav(){
    if($('#playerBreakdown').is(":visible")){
        if(medalSelection > 4){
            medalSelection-=5;
            updateSelection(medalSelection)
        }
    }else{
        if(itemNumber > 0){
            itemNumber--;
            updateSelection(itemNumber);
        }
    }
}

function downNav(){
    if($('#playerBreakdown').is(":visible")){
        if(medalSelection < 5){
            medalSelection+=5;
            updateSelection(medalSelection)
        }
    }else{
        if(itemNumber < $('.clickable').length-1){
            itemNumber++;
            updateSelection(itemNumber);
        }    
    }    
}

function leftNav(){
    if($('#playerBreakdown').is(":visible")){
         if(medalSelection % 5 != 0){
            medalSelection--;
            updateSelection(medalSelection);
        }
    }
}

function rightNav(){
    if($('#playerBreakdown').is(":visible")){
        if(medalSelection % 5 != 4){
            medalSelection++;
            updateSelection(medalSelection);
        }  
    }    
}

function checkGamepad(){
    var shouldUpdateHeld = false;
    if(Date.now() - lastHeldUpdated > 100) {
        shouldUpdateHeld = true;
        lastHeldUpdated = Date.now();
    }
    if(stickTicks.up == 1 || (shouldUpdateHeld && stickTicks.up > 25)){
        upNav();
    }
    if(stickTicks.down == 1 || (shouldUpdateHeld && stickTicks.down > 25)){
        downNav();
    }
    if(stickTicks.left == 1 || (shouldUpdateHeld && stickTicks.left > 25)){
        leftNav();
    }
    if(stickTicks.right == 1 || (shouldUpdateHeld && stickTicks.right > 25)){
        rightNav();
    }
};

function setPlayerVolume(name,uid,level){
    dew.show("voip", {
        volume:{
            uid:name + "|" + uid,
            vol:level / 100.0
        }
    });
	
	var playerVoip = playerVolumes[name];
    if(playerVoip){
		playerVoip.volume = level;
    }
}

function isSpeaking(name,visible){
    if(visible){
        $('#'+name).find('.speaker').addClass('talking');
    }else{
        //$('#'+name).find('.speaker').removeClass('talking');  
    }
}

function hideScoreboard() {
    dew.callMethod('scoreboardHide', {});
}
