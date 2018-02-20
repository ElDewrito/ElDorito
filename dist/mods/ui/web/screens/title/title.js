var hasGP = false;
var announceNum = 0;

$("html").on("keydown", function(e) {
    if (e.which == 13){
        hideScreen();
    } 
    if(e.keyCode == 192 || e.keyCode == 112 || e.keyCode == 223){
        dew.show("console");
    }
});

dew.on("show", function(e){
    dew.command('Game.SkipTitleSplash', {}).then(function(result){
        if(result == 1){
            dew.captureInput(false);
            $(".genericLoader").hide();
            $("#blackLayer").fadeOut(2000, function() {
                dew.hide();
            });
        }else{
            $("#blackLayer").hide();
            $.getJSON( "http://scooterpsu.github.io/announcements.json", function(data) {
                if(data.announcements.length){
                    //dew.command('Game.FirstRun', {}).then(function(response){
                        //if(response == 1){
                           $("#announcementBox").show(); 
                        //};
                    //});
                    for(var i = 0; i < data.announcements.length; i++){
                        $('#announcementBox').append(
                            $('<div>',{
                                class: 'announcement'
                            }).append($('<p>',{
                                class: 'announceTitle',
                                text: data.announcements[i].title
                            })).append($('<p>',{
                                class: 'announceContent',
                                html: data.announcements[i].content
                            })).append($('<img>',{
                                class: 'announceImage',
                                src: 'dew://assets/announcements/'+data.announcements[i].image
                            }))
                        );
                    }
                    $('.announcement').eq(0).show();
                }
            }); 
            dew.command('Settings.Gamepad', {}).then(function(result){
                if(result == 1){
                    onControllerConnect();
                    hasGP = true;
                }else{
                    onControllerDisconnect();
                    hasGP = false;
                }
            });
        }
    });
});

function onControllerConnect(){
    dew.command('Game.IconSet', {}).then(function(controllerType){
        $('#dpad').attr('src','dew://assets/buttons/'+controllerType+'_Dpad.png');
        $("#dpad").show();
        $( "#up, #down, #left, #right" ).hide();
        $(".instructionText img").attr("src","dew://assets/buttons/"+controllerType+"_Start.png");
        $("#esc").attr("src","dew://assets/buttons/"+controllerType+"_B.png");
        $("#enter").attr("src","dew://assets/buttons/"+controllerType+"_A.png");
    });
}

function onControllerDisconnect(){
    $("#dpad").hide();
    $( "#up, #down, #left, #right" ).show();
    $(".instructionText img").attr("src","dew://assets/buttons/Keyboard_White_Enter.png");
    $("#esc").attr("src","dew://assets/buttons/Keyboard_White_Esc.png");
    $("#enter").attr("src","dew://assets/buttons/Keyboard_White_Enter.png");
}

dew.on('controllerinput', function(e){       
    if(hasGP){
        if(e.data.Start == 1){
            hideScreen();   
        }
    }
});

function hideScreen(){
    $( "body" ).fadeOut( 500, function() {
        dew.hide();
    });
}

function nextAnnounce(){
    if(announceNum < $('.announcement').length-1){
        $('.announcement').hide();
        announceNum++;
        $('.announcement').eq(announceNum).show();
    }
}

function closeAnnounce(){
    $('#announcementBox').hide();
    /*dew.command('Game.FirstRun 0', {}).then(function(){
        dew.command('writeconfig');
    });*/
}