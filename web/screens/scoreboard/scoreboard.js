var locked = false;

$(window).load(function(){
    $(document).keydown(function(e) {
        if(locked && e.keyCode == 9)
            dew.hide();
    });

    dew.on("show", function(e){
        locked = e.data.locked;
        dew.captureInput(locked);
    });

    dew.on("scoreboard", function(e){
        console.log(e.data);
    });
});
