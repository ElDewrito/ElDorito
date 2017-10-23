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
    $("#description").text(event.data.description);
    $("#text").val(event.data.defaultValue);
    $(".dialog").show();
    $("#text").focus();
    
    if(event.data.title == "Edit Description"){
        $('#text').attr({'maxlength':'127','rows':'10'});
    }else{
        $('#text').attr({'maxlength':'15','rows':'1'});
    };
});

dew.on("hide", function (event) {
    $(".dialog").hide();
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

$(window).load(function () {
    $(".dialog").hide();

    $("html").on("submit", "form", function (event) {
        event.preventDefault();
        var value = $("#text").val().trim();
        if (value === "") {
            return;
        }
        dew.submitVirtualKeyboard(value).then(() => dew.hide());
    });

    $("html").on("keydown", function (event) {
        if (event.keyCode === 13 /* Enter */) {
            event.preventDefault();
            $("form").submit();
        } else if (event.keyCode === 27 /* Escape */) {
            dew.cancelVirtualKeyboard().then(() => dew.hide());
        }
    });
    
    $("#ok").on("click", function (){
        $("form").submit();
    });
    
    $("#cancel").on("click", function (){
        dew.cancelVirtualKeyboard().then(() => dew.hide());
    });
});