var messageQueue = [];
var messageInProgress = false;
$("html").on("keydown", function(e) {
    if (e.which == 113)
        dew.hide();
});

$(window).load(function() {
    $(function() {
        $('.notification').addClass('notification-hidden');
    })

});


function openNotification(whichNotification) {

    var obj = messageQueue[0];
    document.getElementById('messageText').innerHTML = obj.text;
    $('#messageImage').attr("src", 'images/' + obj.image + '.png');
    messageInProgress = true;
    var thisNotification = $('.notification.' + whichNotification);
    thisNotification.removeClass('notification-hidden');
    setTimeout(function() {
        play();
        thisNotification.addClass('open');
        $('.notification.open').addClass('show').addClass('open--rot');
        setTimeout(function() {
            $('.notification.open').addClass('open--width');
        }, 750);

        $('body').append('<div class="overlay"></div>');
    }, 50);
}

function closeNotification() {

    var type = $('.notification.open');
    type.removeClass('open--width');
    setTimeout(function() {
        type.removeClass('open--rot')
    }, 400);
    setTimeout(function() {
        type.removeClass('show');
    }, 600);
    setTimeout(function() {
        $('.overlay').remove();
        type.removeClass('open');
    }, 800);
    setTimeout(function() {
        type.addClass('notification-hidden')
        messageInProgress = false;
        messageQueue.shift();
        checkQueue();
    }, 850);
}

function play() {
    var audioElement = new Audio('sounds/AchievementUnlocked.ogg');
    audioElement.play();

}
dew.on("HaloStatsRankMessage", function(event) {
    dew.show();
    var image = {
        "text": "You have been Promoted! " + event.data.RankName,
        "image": event.data.Rank
    };
    messageQueue.push(image);
    if (messageInProgress == false || messageQueue.length == 1) {
        openNotification('positive');
        setTimeout(function() {
            closeNotification();
        }, 4000);
    }

});

function checkQueue() {
    if (messageQueue.length > 0) {
        openNotification('positive');
        setTimeout(function() {
            closeNotification();
        }, 4000);
    }
}
