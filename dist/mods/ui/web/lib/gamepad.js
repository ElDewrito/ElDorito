var capturedInput = false;
var repGP;
var hasGP = false;
var timestamp; 
var lastButtons = [];
var lastAxes = [];
var axisThreshold = .5;
var controllerPort;
var lastHeldUpdated = 0;

var Action = {
    UiLeftTrigger: 0,
    UiRightTrigger: 1,
    UiUp: 2,
    UiDown: 3,
    UiLeft: 4,
    UiRight: 5,
    UiStart: 6,
    UiSelect: 7,
    UiLeftStick: 8,
    UiRightStick: 9,
    UiA: 10,
    UiB: 11,
    UiX: 12,
    UiY: 13,
    UiLeftBumper: 14,
    UiRightBumper: 15,

    BindableActionCount: 16,

    MoveForward: 16,
    MoveBack: 17,
    MoveLeft: 18,
    MoveRight: 19,

    LookUp: 20,
    LookDown: 21,
    LookLeft: 22,
    LookRight: 23,

    ActionCount: 24
};

var bindings = initializeBindings();
var actions = initializeActions();

function initializeBindings() {
    var bindings = {};

    bindings.controllerButtons = [];

    for(var i = 0; i < Action.BindableActionCount; i++) {
        bindings.controllerButtons[i] = -1;
    }

    bindings.controllerButtons[Action.UiUp] = 12;
    bindings.controllerButtons[Action.UiDown] = 13;
    bindings.controllerButtons[Action.UiRight] = 15;
    bindings.controllerButtons[Action.UiLeft] = 14;
    bindings.controllerButtons[Action.UiA] = 0;
    bindings.controllerButtons[Action.UiB] = 1;
    bindings.controllerButtons[Action.UiX] = 2;
    bindings.controllerButtons[Action.UiY] = 3;
    bindings.controllerButtons[Action.UiLeftBumper] = 4;
    bindings.controllerButtons[Action.UiRightBumper] = 5;
    bindings.controllerButtons[Action.UiStart] = 9;
    bindings.controllerButtons[Action.UiSelect] = 8;

    return bindings;
}

function initializeActions() {
    var actions = [];
    for(var i = 0; i < Action.ActionCount; i++) {
        actions[i] =  { flags: 0, ticks: 0 };
    }
    return actions;
}

function ProcessControllerButtons(bindings, actions, gamepad) {
    for(var i = 0; i < Action.BindableActionCount; i++) {
        var binding = bindings.controllerButtons[i];
        if(binding == -1) continue;

        var actionState = actions[i];

        if(gamepad.buttons[binding].pressed)
            actionState.ticks++;
        else
            actionState.ticks = 0;
    }
}

function ProcessAnalogueInput(actions, gamepad) {
    var lsHorizontalAxis = gamepad.axes[0],
        lsVerticalAxis = gamepad.axes[1],
        rsHorizontalAxis = gamepad.axes[2],
        rsVerticalAxis = gamepad.axes[3]

    var moveRight = actions[Action.MoveRight],
        moveLeft = actions[Action.MoveLeft],
        moveForward = actions[Action.MoveForward],
        moveBackward = actions[Action.MoveBack]

    var lookRight = actions[Action.LookRight],
        lookLeft = actions[Action.LookLeft],
        lookUp = actions[Action.LookUp],
        lookDown = actions[Action.LookDown]

    if(Math.abs(lsHorizontalAxis) > Math.abs(lsVerticalAxis))
    {
        if(lsHorizontalAxis > axisThreshold) moveRight.ticks++; else moveRight.ticks = 0;
        if(lsHorizontalAxis < -axisThreshold) moveLeft.ticks++; else moveLeft.ticks = 0;
    }
    else
    {
        if(lsVerticalAxis > axisThreshold) moveBackward.ticks++; else moveBackward.ticks = 0;
        if(lsVerticalAxis < -axisThreshold) moveForward.ticks++; else moveForward.ticks = 0;
    }

    if(Math.abs(rsHorizontalAxis) > Math.abs(rsVerticalAxis))
    {
        if(rsHorizontalAxis > axisThreshold) lookRight.ticks++; else lookRight.ticks = 0;
        if(rsHorizontalAxis < -axisThreshold) lookLeft.ticks++; else lookLeft.ticks = 0;
    }
    else
    {
        if(rsVerticalAxis > axisThreshold) lookDown.ticks++; else lookDown.ticks = 0;
        if(rsVerticalAxis < -axisThreshold) lookUp.ticks++; else lookUp.ticks = 0;
    }
}

function ProcessKeys(bindings, actions, keyStates) {
     for(var i = 0; i < Action.BindableActionCount; i++) {
        var binding = bindings.primaryKeys[i];
        if(binding == -1) continue;

        var actionState = actions[i];

        if(keyStates[binding])
            actionState.ticks++;
        else
            actionState.ticks = 0;
     }
}

function handleAction(e) {
    if(e < Action.BindableActionCount)
        return buttonAction(bindings.controllerButtons[e]);

    switch(e)
    {
        case Action.MoveForward:
            stickAction(1, '-');
            buttonAction(bindings.controllerButtons[Action.UiUp]);
            break;
        case Action.MoveBack:
            stickAction(1, '+');
            buttonAction(bindings.controllerButtons[Action.UiDown]);
            break;
        case Action.MoveRight:
             stickAction(0, '+');
             buttonAction(bindings.controllerButtons[Action.UiRight]);
            break;
        case Action.MoveLeft:
            stickAction(0, '-');
            buttonAction(bindings.controllerButtons[Action.UiLeft]);
            break;
        case Action.LookUp:
            stickAction(3, '-');
            break;
        case Action.LookDown:
            stickAction(3, '+');
            break;
        case Action.LookLeft:
            stickAction(2, '-');
            break;
        case Action.LookRight:
            stickAction(2, '+');
            break;
    }
}

function handleActions(actions) {   
    var shouldUpdateHeld = false;
    if(Date.now() - lastHeldUpdated > 100) {
      shouldUpdateHeld = true;
      lastHeldUpdated = Date.now();
    }

    for(var i = 0; i < actions.length; i++) {
        if(actions[i].ticks == 1 || (shouldUpdateHeld && actions[i].ticks > 25))
            handleAction(i);
    }
}

dew.on("show", function(e) {
    if(e.data.hasOwnProperty('locked')){
        capturedInput = e.data.locked;
    }else{
        capturedInput = true;
    }
    if(controllerSupport()){
        dew.command('Input.ControllerPort', {}).then(function(response){
            controllerPort = response;
        });
        $(window).on("gamepadconnected", function(){
            hasGP = true;
            repGP = window.setInterval(checkGamepad,1000/60);
            onControllerConnect();
        });
        $(window).on("gamepaddisconnected", function(){
            hasGP = false;
            window.clearInterval(repGP);
            onControllerDisconnect();
        });
        var checkGP = window.setInterval(function(){
            if(navigator.getGamepads()[controllerPort]){
                if(!hasGP) $(window).trigger("gamepadconnected");
                window.clearInterval(checkGP);
            }
        }, 500);
    }
});

dew.on("hide", function(e) {
    capturedInput = false;
});

function controllerSupport(){
    return "getGamepads" in navigator;
}

function checkGamepad(){
    var gamepad = navigator.getGamepads()[controllerPort];
    if(gamepad){
        if(!hasGP){ //if controller disconnects and reconnects
            $(window).trigger("gamepadconnected");
        }            
        
        if(capturedInput){
            ProcessControllerButtons(bindings, actions, gamepad);
            ProcessAnalogueInput(actions, gamepad);
            handleActions(actions);
        }
        
    }else{ //controller not reporting, must be disconnected
        $(window).trigger("gamepaddisconnected");               
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