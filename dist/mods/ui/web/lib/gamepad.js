var capturedInput = false;
var repGP;
var hasGP = false;
var timestamp; 
var lastButtons = [];
var lastAxes = [];
var axisThreshold = .5;

dew.on("show", function(event) {
    capturedInput = true;
    if(controllerSupport()){
        $(window).on("gamepadconnected", function(){
            hasGP = true;
            //console.log("Gamepad connected");
            repGP = window.setInterval(checkGamepad,100);
            onControllerConnect();
        });
        $(window).on("gamepaddisconnected", function(){
            hasGP = false;
            //console.log("Gamepad disconnected");
            window.clearInterval(repGP);
            onControllerDisconnect();
        });
        var checkGP = window.setInterval(function(){
            if(navigator.getGamepads()[0]){
                if(!hasGP) $(window).trigger("gamepadconnected");
                window.clearInterval(checkGP);
            }
        }, 500);
    }
});

dew.on("hide", function(event) {
    capturedInput = false;
});

function controllerSupport(){
    return "getGamepads" in navigator;
}

function checkGamepad(){
    var gamepad = navigator.getGamepads()[0];
    if(gamepad.timestamp != timestamp){
        if(capturedInput){
            for( var i = 0; i < gamepad.buttons.length; i++ ) {
                currentState = gamepad.buttons[i].pressed
                var prevState = lastButtons[i];
                if( !prevState && currentState ){ //Button i Pressed    
                    buttonAction(i);                
                }else if( prevState && !currentState ){//Button i Released

                }
                lastButtons[i] = currentState;
            }
            for( var x = 0; x < gamepad.axes.length; x++ ) {
                currentState = 0
                if(gamepad.axes[x] > 0){
                    if(gamepad.axes[x] > axisThreshold){
                        currentState = 1;
                    }
                }else if(gamepad.axes[x] < 0){
                    if(gamepad.axes[x] < -axisThreshold){
                        currentState = -1;
                    }           
                }
                var prevState = lastAxes[x];
                if( prevState != 1 && currentState == 1 ){
                    stickAction("+", x);
                }else if( prevState != -1 && currentState == -1 ){
                    stickAction("-", x);
                }else if( prevState != 0 && currentState == 0 ){  
                    stickAction("0", x);       
                }
                lastAxes[x] = currentState;
            }
        }
        timestamp = gamepad.timestamp;
    }
}

//Copy the below to whatever screen for easy controller support
function onControllerConnect(){
    
}

function onControllerDisconnect(){
    
}

function stickAction(direction, x){
    if(x<2){//left stick
        if(x==0 && direction=="+"){//LS Right
          
        }else if(x==0 && direction=="-"){//LS Left
  
        }else if(x==1 && direction=="+"){//LS Down

        }else if(x==1 && direction=="-"){//LS Up
       
        }
    }else{//right stick
        if(x==2 && direction=="+"){ //RS Right
          
        }else if(x==2 && direction=="-"){//RS Left
 
        }else if(x==3 && direction=="+"){//RS Down
  
        }else if(x==3 && direction=="-"){//RS Up
                   
        }
    }
}

function buttonAction(i){
    switch (i) {
        case 0: // A
            break;
        case 1: // B
            break;
        case 2: // X
            break;
        case 3: // Y
            break;
        case 8: // Back
            break;
        case 12: // Up
            break;
        case 13: // Down
            break;
        default:
            //console.log("nothing associated with " + i);
    }  
}