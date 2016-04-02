$(function() {
   $(window).keypress(function(e) {
       var key = e.which;
       if (key == 13){
            $( ".genericLoader" ).fadeOut( 500, function() {
                dew.hide();
            });
       } else if (key ==96){
            dew.show("console");
       }
   });
});

dew.on("show", function (event) {
    $( ".genericLoader" ).show();
});