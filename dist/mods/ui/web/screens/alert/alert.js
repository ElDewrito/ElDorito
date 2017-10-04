var pageWidth, pageHeight;
var basePage = {
    width: 1280,
    height: 720,
    scale: 1,
    scaleX: 1,
    scaleY: 1
};

dew.on("show", function (event) {
    $("#title").text(event.data.title);
    $("#description").text(event.data.body);
    $(".dialog").show();
});

dew.on("hide", function (event) {
    $(".dialog").hide();
});

$(window).load(function () {
    $(".dialog").hide();
    
    $("#ok").on("click", function (){
        dew.hide();
    });
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