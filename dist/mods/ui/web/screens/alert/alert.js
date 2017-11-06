var pageWidth, pageHeight;
var basePage = {
    width: 1280,
    height: 720,
    scale: 1,
    scaleX: 1,
    scaleY: 1
};

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
    
    form.append( 
        $("<button>", {
            id: 'ok',
            text: 'OK',
            click: function(e){ 
                e.preventDefault();
                $(this).parent().parent().remove();
                if(!$('.dialog').length){
                    dew.hide();
                };
            }
        })
    );

    form.wrap( 
        $("<div>", {
            class: 'dialog'
        })
    );
    
    $(".container").append(form.parent());
});

dew.on("hide", function (event) {
    $(".container").empty();
});

$(window).load(function () {

});

$(function(){
    var $page = $('.page_content');

    getPageSize();
    scalePages($page, pageWidth, pageHeight);
  
    $(window).resize(function() {
        getPageSize();            
        scalePages($page, pageWidth, pageHeight);
    });
  
    function getPageSize() {
        pageHeight = $('#container').height();
        pageWidth = $('#container').width();
    }

    function scalePages(page, maxWidth, maxHeight) {            
        var scaleX = 1, scaleY = 1;                      
        scaleX = maxWidth / basePage.width;
        scaleY = maxHeight / basePage.height;
        basePage.scaleX = scaleX;
        basePage.scaleY = scaleY;
        basePage.scale = (scaleX > scaleY) ? scaleY : scaleX;
        page.attr('style', '-webkit-transform:scale(' + basePage.scale + ');');
    }
});