var locked = false;
var isHost = false;
var cardOpacity = 0.9;
var medalsPath = 'medals://';
capturedInput = false;
var imageFormat = 'svg';
var itemNumber = 0;
var controllerType;

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

$(window).load(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            if($('#playerBreakdown').is(":visible")){
                $('#playerBreakdown').hide();
            } else {
               dew.hide(); 
            }
        }
    });
    $(document).keydown(function(e){
        if(e.keyCode == 9|| e.keyCode == 13){
            $('#playerBreakdown').hide();
            dew.captureInput(false);
            dew.hide();
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
                    dew.command("Server.KickUid " + flipUID($(this).attr('data-uid')));
                    break;
                case "ban":
                    dew.command("Server.KickBanUid " + flipUID($(this).attr('data-uid')));
                    break;
                case "mute":
                    console.log($(this).attr('id'),$(this).attr('data-uid')); //VOIP: replace with mute command
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
            }
        }
    });
});

dew.on("scoreboard", function(e){
    dew.command('Game.ExpandedScoreboard', {}).then(function(response){
        displayScoreboard(response);
    });
});

dew.on("voip-user-volume", function(data){
});

dew.on("voip-speaking", function(data){ 
    //$('#'+playerName).find('.speaker').css({'opacity':'1 !important'});  //VOIP: show speaker if talking
});

dew.on("show", function(e){
    locked = e.data.locked;
    dew.captureInput(locked);
    capturedInput = locked;
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
    dew.command('Game.ExpandedScoreboard', {}).then(function(response){
        displayScoreboard(response);
    });
    dew.command('Game.MedalPack', {}).then(function(response){
        medalsPath = "medals://" + response + "/";
        $.getJSON(medalsPath+'events.json', function(json) {
            eventJson = json;
            if(eventJson['settings']){
                if(eventJson['settings'].hasOwnProperty('imageFormat')){
                    imageFormat = eventJson['settings'].imageFormat;
                }
            }            
        });
    });
});

function displayScoreboard(expandedScoreboard){
    dew.getSessionInfo().then(function(i){
        isHost = i.isHost;
    });
    dew.getScoreboard().then(function (e){ 
        var scoreboardheader = '<th></th><th class="name">Players</th>';
        if(locked || (expandedScoreboard == 1)){
            switch(e.gameType){
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
                    scoreboardheader += '<th>King Kills</th><th>Time In Hill</th><th>Controlling</th>';    
                    break;
                case "vip":
                case "juggernaut":
                case "assault":
                default:
                    scoreboardheader += '<th>Kills</th><th>Assists</th><th>Deaths</th><th>Best Streak</th>';          
            }
            $('#window').css({'width':'54%','left':'23%'});
        } else {
            $('#window').css({'width':'38%','left':'31%'});
        }
        scoreboardheader += '<th>Score</th>'; 
        $('#header').html(scoreboardheader);
        buildScoreboard(e.players, e.hasTeams, e.teamScores, e.gameType, JSON.parse(e.playersInfo),expandedScoreboard);
    });
    dew.command("Server.NameClient", { internal: true }).then(function (name){
        $("#serverName").text(name);
    });    
    if(hasGP && locked){
        updateSelection(itemNumber);
    }
}

function buildScoreboard(lobby, teamGame, scoreArray, gameType, playersInfo,expandedScoreboard){
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
                    var teamHeader = '<tbody id="'+teamArray[lobby[i].team].name+'" data-score="'+scoreArray[lobby[i].team]+'" class="team"><tr class="player teamHeader" style="background-color:'+hexToRgb(teamArray[lobby[i].team].color, cardOpacity)+';"><td class="rank"></td><td class="name">'+teamArray[lobby[i].team].name.toUpperCase()+' TEAM</td><td class="score">'+scoreArray[lobby[i].team]+'</td></tr></tbody>';
                    $('#window table').append(teamHeader);    
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
                    'data-playerIndex':lobby[i].playerIndex,
                }).mouseover(function(){
                    col = $(this).attr('data-color'),
                    bright = adjustColor(col, 30);
                    $(this).css("background-color", hexToRgb(bright, cardOpacity));
                }).mouseout(function(){
                    col = $(this).attr('data-color');
                    $(this).css("background-color", hexToRgb(col, cardOpacity));
                })
                .append($('<td class="rank">'))
                .append($('<td class="name">').text(lobby[i].name)) //name
            );   
            var emblemPath = 'dew://assets/emblems/ed.png';
            if(lobby[i].isAlive){
                if(playersInfo[lobby[i].playerIndex]){
                    emblemPath = playersInfo[lobby[i].playerIndex].e;
                }
            } else {
                emblemPath = 'dew://assets/emblems/dead.png';   
            }
            $("[data-playerIndex='" + lobby[i].playerIndex + "'] .name").prepend('<img class="emblem" src="'+emblemPath+'">');
            if(locked || (expandedScoreboard == 1)){
                switch(gameType){
                    case "oddball":
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat">').text(lobby[i].kills)) //kills
                            .append($('<td class="stat">').text(lobby[i].ballKills)) //ball kills
                        $('.teamHeader .name').attr('colspan',3);
                        break;
                    case "infection":
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat">').text(lobby[i].kills)) //kills
                            .append($('<td class="stat">').text(lobby[i].humansInfected)) //infected
                            .append($('<td class="stat">').text(lobby[i].zombiesKilled)) //zombies killed  
                        $('.teamHeader .name').attr('colspan',4);
                        break;
                    case "ctf":
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat">').text(lobby[i].kills)) //kills
                            .append($('<td class="stat">').text(lobby[i].ballKills)) //flag kills
                        $('.teamHeader .name').attr('colspan',3);
                        break;
                    case "koth":
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat">').text(lobby[i].kingsKilled)) //kings killed
                            .append($('<td class="stat">').text(lobby[i].timeInHill)) //time in hill
                            .append($('<td class="stat">').text(lobby[i].timeControllingHill)) //time controlling hill
                        $('.teamHeader .name').attr('colspan',4);
                        break;                   
                    case "vip":
                    case "juggernaut":
                    case "assault":
                    default:
                        $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat">').text(lobby[i].kills)) //kills
                            .append($('<td class="stat">').text(lobby[i].assists)) //assists
                            .append($('<td class="stat">').text(lobby[i].deaths)) //deaths 
                            .append($('<td class="stat">').text(lobby[i].bestStreak)) //best streak 
                        $('.teamHeader .name').attr('colspan',5);
                }
            }
            $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<td class="stat score">').text(lobby[i].score)) //score    
            $("[data-playerIndex='" + lobby[i].playerIndex + "']").append($('<img class="emblem speaker" src="dew://assets/emblems/speaker.png">')) //voip speaking indicator              
            if(teamGame){
                sortMe('scoreboard','tbody');
                sortMe(teamArray[lobby[i].team].name,'tr');
            } else {
                sortMe('singlePlayers','tr');
            }
            rankMe(teamGame);
        }     
        $('.clickable').find('td').on('click',function(){
            $('.clickable td').off('click');
            var playerName = $(this).parent().attr('id');
            playerBreakdown(playerName);
        });
        $('.speaker').on('click', function(e){
            $('.speaker').off('click');
            console.log($(this).parent().attr('id'), $(this).parent().attr('data-uid')); //VOIP: replace with volume control
        });        
    }  
}

function hexToRgb(hex, opacity){
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return "rgba(" + parseInt(result[1], 16) + "," + parseInt(result[2], 16) + "," + parseInt(result[3], 16) + "," + opacity + ")";
}

function sortMe(sortWhat, sortWhich){
    var $wrapper = $('#'+sortWhat);
    $wrapper.find(sortWhich).sort(function(b, a) {
        return parseFloat(a.dataset.score) - parseFloat(b.dataset.score);
    }).appendTo($wrapper);
}

function rankMe(teamGame){
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
    if($(outer).length > 1 && ($(outer+':eq(0)').attr('data-score') == $(outer+':eq(1)').attr('data-score'))){
        winnerText = 'Tie!';
    }
    $('#winnerText').text(winnerText);

    var lastScore = $(outer+':eq(0)').attr('data-score');
    for (i = 0; i < $(outer).length; i++){ 
        if($(outer+':eq('+i+')').attr('data-score') != lastScore){
            x=i+1;
            lastScore = $(outer+':eq('+i+')').attr('data-score');
        }
        if(teamGame){
            $(outer).eq(i).find(inner).find('.rank').text(x);
        }else{
            $(outer).eq(i).find('.rank').text(x);           
        }
    }
}

function playerBreakdown(name){
    dew.command('Server.ListPlayersJSON').then(function(res){
        var lobby = JSON.parse(res);
        var playerIndex = lobby.findIndex(x => x.name == name)
        var color = hexToRgb(lobby[playerIndex].color, 0.9);
        
        if(playerIndex==0){
            $('#previousPlayer').prop("disabled",true);
        }else{
            $('#previousPlayer').prop("disabled",false);      
            $('#previousPlayer').on('click', function(){
                $('#previousPlayer').off('click');
                playerBreakdown(lobby[playerIndex-1].name); 
            });            
        }
        if(playerIndex==(lobby.length-1)){
            $('#nextPlayer').prop("disabled",true);
        }else{
            $('#nextPlayer').prop("disabled",false);   
            $('#nextPlayer').on('click', function(){ 
                $('#nextPlayer').off('click');
                playerBreakdown(lobby[playerIndex+1].name); 
            });            
        }

        dew.getStats(name).then(function (stats){
            $('#playerBreakdown').show();
            $('#playerName').text(name);  
            $('#playerName').css({'background-color': color});  
            
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
                var enemyIndex = lobby.findIndex(x => x.name == playerKilledArray[0].PlayerName);
                var bgColor = "#C0C0C0";
                if(enemyIndex > -1){ bgColor = lobby[enemyIndex].color; };
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
                var enemyIndex = lobby.findIndex(x => x.name == killedByArray[0].PlayerName);
                var bgColor = "#C0C0C0";
                if(enemyIndex > -1){ bgColor = lobby[enemyIndex].color; };
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
                            html: "<span class='medalCount'>x "+medalArray[i].count+"</span>"
                        }).mouseover(function(){
                            $('#bigMedal').css("background-image","url("+ medalsPath + "images/" + $(this).attr('id') +"."+ imageFormat +")");
                            $('.medalName').text(medalDetails[medalDetails.findIndex(x => x.string == $(this).attr('id'))].name);
                            $('.medalDesc').text(medalDetails[medalDetails.findIndex(x => x.string == $(this).attr('id'))].desc);
                        })
                    );
                }
            }
            var fillerCount = 10 - $('#medalBox').children().length;
            for(i=0;i<fillerCount;i++){
                $('#medalBox').append('<div class="medal">');
            }
        })
    }); 
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


function flipUID(uid){
    var bits = uid.match(/.{1,2}/g);
    var newUID = "";
    for (var i = 0; i < bits.length; i++){
        newUID = bits[i] + newUID;
    }
    return newUID;
}

function onControllerConnect(){
    $('#closeButton').css('padding-right', '1.75vw');
    dew.command('Game.IconSet', {}).then(function(response){
        controllerType = response;
        $('#closeButton .button').attr('src','dew://assets/buttons/'+controllerType+'_Start.png');
        $('#closeButton .button').show();
    });  
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

function buttonAction(i){
    switch (i) {
        case 0: // A
            if(!$('#playerBreakdown').is(":visible")){
                $('.clickable').eq(itemNumber).click();
            }
            break;
        case 1: // B
            if($('#playerBreakdown').is(":visible")){
                $('#playerBreakdown').hide();
            } else {
               dew.hide(); 
            }
            break;
        case 4: // LB
            if($('#playerBreakdown').is(":visible")){
                $('#previousPlayer').click();
            }
            break;
        case 5: // RB
            if($('#playerBreakdown').is(":visible")){
                $('#nextPlayer').click();
            }
            break;
        case 9: // Start    
            $('#playerBreakdown').hide();
            dew.hide();
            break;
        case 12: // Up
            upNav();
            break;
        case 13: // Down
            downNav();
            break;
        default:
            //console.log("nothing associated with " + i);
    }  
}

function stickAction(direction, x){
    if(x<2){//left stick
        if(x==0 && direction=="+"){//LS Right

        }else if(x==0 && direction=="-"){//LS Left

        }else if(x==1 && direction=="+"){//LS Down
            downNav();
        }else if(x==1 && direction=="-"){//LS Up
            upNav();
        }
    }
}

function updateSelection(item){
    if($('.clickable').length){
        for(var i = 0; i < $('.clickable').length; i++) {
            $('.clickable').eq(i).css("background-color", hexToRgb($('.clickable').eq(i).attr('data-color'), cardOpacity));
        }
    }
    col = $("[data-playerIndex='" + item+ "']").attr('data-color'),
    bright = adjustColor(col, 30);
    $('.clickable').eq(item).css("background-color", hexToRgb(bright, cardOpacity));
}

function upNav(){
    if(itemNumber > 0){
        itemNumber--;
        updateSelection(itemNumber);
    }
}

function downNav(){
    if(itemNumber < $('.clickable').length-1){
        itemNumber++;
        updateSelection(itemNumber);
    }           
}