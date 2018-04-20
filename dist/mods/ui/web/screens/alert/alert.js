var settingsArray = { 'Settings.Gamepad': '0' , 'Game.IconSet': '360'};

dew.on("variable_update", function(e){
    for(i = 0; i < e.data.length; i++){
        if(e.data[i].name in settingsArray){
            settingsArray[e.data[i].name] = e.data[i].value;
        }
    }
});

function loadSettings(i){
	if (i != Object.keys(settingsArray).length) {
		dew.command(Object.keys(settingsArray)[i], {}).then(function(response) {
			settingsArray[Object.keys(settingsArray)[i]] = response;
			i++;
			loadSettings(i);
		});
	}
}

$(document).ready(function(){
        loadSettings(0);
});

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
            html: event.data.body.split('|r|n').join('<br />')
        })
    );
    
	var okText = 'Ok';
	if(event.data.type == "url"|event.data.type == "update"){ 
        okText = 'Yes'; 
    }

    var buttonContainer = $('<div>', {
        'class': 'alertButtonContainerBottom'
    });
	
    buttonContainer.append($("<button>", {
            id: 'ok',
            html: '<img class="button">'+okText,
            click: function(e){ 
                e.preventDefault();
                $(this).parent().parent().parent().remove();
                if(!$('.dialog').length){
					if(event.data.type == "url"){
						window.open(event.data.url, '_blank');
					}else if(event.data.type == "update"){
						
                        dew.command('Game.Update', function() {
                            //Update go!
                        }).catch(function (error) {
                             dew.show('alert',{"title":"Error", "body":error.message});
                        });
                    }
					dew.command('Game.PlaySound 0xb00');
                    dew.hide();
                };
            }
        }));
    
	
	if(event.data.type == "url" || event.data.type == "update"){
		buttonContainer.append( 
			$("<button>", {
				id: 'cancel',
				html: '<img class="button">No',
				click: function(e){ 
					e.preventDefault();
					$(this).parent().parent().parent().remove();
					if(!$('.dialog').length){
						dew.command('Game.PlaySound 0xb01');
						dew.hide();
					};
				}
			})
		);
    }
    
    form.append(buttonContainer);

    form.wrap( 
        $("<div>", {
            class: 'dialog'
        })
    );
    
    $(".container").append(form.parent());
    
    if(settingsArray['Settings.Gamepad'] == 1){
        $('#ok .button').attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_A.png');
        $('#cancel .button').attr('src','dew://assets/buttons/'+settingsArray['Game.IconSet']+'_B.png');
        $('.button').show();   
    }else{
        $('.button').hide();   
    }
});

dew.on("hide", function (event) {
    $(".container").empty();
});

$(document).ready(function(){
    $(document).keyup(function (e) {
        if (e.keyCode === 27) {
            $('.dialog:eq(0) #cancel').click(); 
        }
    });
   $(document).keydown(function (e) {
        if (e.keyCode === 13 || e.keyCode === 32) {
            $('.dialog:eq(0) #ok').click(); 
        }
    });
});

dew.on('controllerinput', function(e){       
    if(settingsArray['Settings.Gamepad'] == 1){
        if(e.data.A == 1){
            $('.dialog:eq(0) #ok').click(); 
        }
        if(e.data.B == 1){
            $('.dialog:eq(0) #cancel').click(); 
        }
    }
});