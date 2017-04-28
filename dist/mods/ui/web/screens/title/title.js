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
});

function onControllerConnect(){
    $("#dpad").show();
    $( "#up, #down, #left, #right" ).hide();
    $(".instructionText img").attr("src","dew://assets/buttons/XboxOne_Menu.png");
    $("#esc").attr("src","dew://assets/buttons/XboxOne_B.png");
    $("#enter").attr("src","dew://assets/buttons/XboxOne_A.png");
}

function onControllerDisconnect(){
    $("#dpad").hide();
    $( "#up, #down, #left, #right" ).show();
    $(".instructionText img").attr("src","dew://assets/buttons/Keyboard_White_Enter.png");
    $("#esc").attr("src","dew://assets/buttons/Keyboard_White_Esc.png");
    $("#enter").attr("src","dew://assets/buttons/Keyboard_White_Enter.png");
}

function buttonAction(i){
    switch (i) {
        case 9: // Start
            $( ".genericLoader" ).fadeOut( 500, function() {
                dew.hide();
            });
            break;
        default:
            // console.log("nothing associated with " + i);
    }  
}