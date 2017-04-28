$(document).ready(function() {
   $(window).keypress(function(e) {
       var key = e.which;
       if (key == 13){
            $( ".genericLoader" ).fadeOut( 500, function() {
                dew.hide();
            });
       } else if (key ==192){
            dew.show("console");
       }
   });
   setInterval( CheckPageFocus, 200 );
   if(controllerSupport()){
        $(window).on("gamepadconnected", function(){
            hasGP = true;
            //console.log("Gamepad connected");
            $("#dpad").show();
            $( "#up, #down, #left, #right" ).hide();
            $(".instructionText img").attr("src","dew://assets/buttons/XboxOne_Menu.png");
            $("#esc").attr("src","dew://assets/buttons/XboxOne_B.png");
            $("#enter").attr("src","dew://assets/buttons/XboxOne_A.png");
            repGP = window.setInterval(checkGamepad,100);
        });
        $(window).on("gamepaddisconnected", function(){
            hasGP = false;
            //console.log("Gamepad disconnected");
            $("#dpad").hide();
            $( "#up, #down, #left, #right" ).show();
            $(".instructionText img").attr("src","dew://assets/buttons/Keyboard_White_Enter.png");
            $("#esc").attr("src","dew://assets/buttons/Keyboard_White_Esc.png");
            $("#enter").attr("src","dew://assets/buttons/Keyboard_White_Enter.png");
            window.clearInterval(repGP);
        });
        var checkGP = window.setInterval(function(){
            if(navigator.getGamepads()[0]){
                if(!hasGP) $(window).trigger("gamepadconnected");
                window.clearInterval(checkGP);
            }
        }, 500);
    }
});

dew.on("show", function (event) {
    $( ".genericLoader" ).show();
});

function CheckPageFocus() {
    if ( document.hasFocus() ) {
        pageFocus = true;
    } else {
        pageFocus = false;
    }
}

function controllerSupport(){
    return "getGamepads" in navigator;
}

var pageFocus = false;
var hasGP = false;
var repGP;
var timestamp; 
var lastButtons = [];
function checkGamepad(){
    var gamepad = navigator.getGamepads()[0];
    if(gamepad.timestamp != timestamp){
        if(pageFocus){
            for( var i = 0; i < gamepad.buttons.length; i++ ) {
                currentState = gamepad.buttons[i].pressed
                var prevState = lastButtons[i];
                if( !prevState && currentState ){ //Button i Pressed    
                    buttonAction(i);                
                }else if( prevState && !currentState ){//Button i Released
                }
                lastButtons[i] = currentState;
            }
        }
        timestamp = gamepad.timestamp;
    }
}

function buttonAction(i){
    //console.log(i);
    switch (i) {
        case 0: // A
                dew.hide();
            break;
        case 9: // Start
                dew.hide();
            break;
        default:
            // console.log("nothing associated with " + i);
    }  
}