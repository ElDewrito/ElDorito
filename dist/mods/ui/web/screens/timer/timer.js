var showDelay = 1, 
    hideDelay = 1,
    initialValue = 0,
    lastTimerBeeped = -1;

dew.on('timerStart', function(e) {
    initialValue = e.data.value;
    showDelay = e.data.type == 'startTimer' ? 4 : 1;
    counterOffset = e.data.type == 'startTimer' ? 2 : 0;
});

dew.on('timerUpdate', function(e) {    
    if((initialValue - e.data.value) >= showDelay) {
        dew.show();
    }

    var percent = ((initialValue - e.data.value - counterOffset)/(initialValue - counterOffset));
    loadCircle(percent);

    if(lastTimerBeeped != e.data.value) {
        lastTimerBeeped = e.data.value;
        if(e.data.value > -1 && e.data.value <= 2) {
            dew.command('Game.PlaySound 0x0B07');
        }
    }
    
    $('.counter').text(e.data.value+1);
});

dew.on('timerEnd', function(e) {
    setTimeout(function() { dew.hide(); }, hideDelay * 1000);
});

dew.on('hide', function(e){
    $('.circle.left').css({"-webkit-transform": "rotate(0deg)"});
    $('.circle.left').css({clip: "rect(0vh, 7vh, 7vh, 0vh)"});
    $('.circle.right').css({"-webkit-transform": "rotate(0deg)"});
    $('.circle.right').css({clip: "rect(0vh, 7vh, 14vh, 0vh)"});
    $('.wrapper.left').css({clip: "rect(7vh, 14vh, 14vh, 0vh)"});
    $('.wrapper.right').css({clip: "rect(0vh, 14vh, 14vh, 7vh)"});    
});

function loadCircle(Percentage){
    var Progress = Percentage * 360;
    if (Progress < 180) {
        $('.circle.right').css({"-webkit-transform": "rotate(" + Progress + "deg)"});
    } else {
        $('.circle.right').css({"-webkit-transform": "rotate(180deg)"});
        if (Progress > 270) {
            $('.circle.left').css({clip: "rect(0vh, 7vh, 14vh, 0vh)"});
            $('.wrapper.left').css({clip: "rect(0vh, 7vh, 14vh, 0vh)"});
        }
    }
    $('.circle.left').css({"-webkit-transform": "rotate(" + Progress + "deg)"});
}
