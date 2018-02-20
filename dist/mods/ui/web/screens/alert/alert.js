var hasGP = false;

dew.on("show", function (event) {   
    var form = $("<form>");
    
    form.append( 
        $("<div>", {
            id: 'title',
            text: event.data.title
        })
    );

    form.append( 
        $("<div>", {
            id: 'description',
            text: event.data.body
        })
    );
    
	var ok = 'OK';
	if(event.data.type == "url") ok = 'YES';
	
    form.append( 
        $("<button>", {
            id: 'ok',
            text: ok,
            click: function(e){ 
                e.preventDefault();
                $(this).parent().parent().remove();
                if(!$('.dialog').length){
					if(event.data.type == "url"){
						window.open(event.data.url, '_blank');
					}
                    dew.hide();
                };
            }
        })
    );
	
	if(event.data.type == "url"){
		form.append( 
			$("<button>", {
				id: 'cancel',
				text: 'CANCEL',
				click: function(e){ 
					e.preventDefault();
					$(this).parent().parent().remove();
					if(!$('.dialog').length){
						dew.hide();
					};
				}
			})
		);
	}

    form.wrap( 
        $("<div>", {
            class: 'dialog'
        })
    );
    
    $(".container").append(form.parent());
    
    dew.command('Settings.Gamepad', {}).then(function(result){
        if(result == 1){
            hasGP = true;
        }else{
            hasGP = false;
        }
    });
});

dew.on("hide", function (event) {
    $(".container").empty();
});

$(document).ready(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            $('.dialog:eq(0) button').click(); 
        }
    });
   $(document).keydown(function (e) {
        if (e.keyCode === 13 || e.keyCode === 32) {
            $('.dialog:eq(0) button').click(); 
        }
    });
});

dew.on('controllerinput', function(e){       
    if(hasGP){
        if(e.data.A == 1 || e.data.B == 1){
            $('.dialog:eq(0) button').click(); 
        }
    }
});