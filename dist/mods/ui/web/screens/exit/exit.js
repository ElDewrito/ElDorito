var hasGP = false;
//Dok was here
dew.on("show", function (event) {
    var form = $("<form>");

    form.append(
        $("<div>", {
            id: 'title',
            text: 'Exit!'
        })
    );

    form.append(
        $("<div>", {
            id: 'description',
            text: 'Are you sure?'
        })
    );
    form.append(
        $("<exitbutton>", {
            class: 'exit',
            id: 'exit',
            text: ' Exit ',
            click: function (e) {
                e.preventDefault();
                $(this).parent().parent().remove();
                if (!$('.dialog').length) {
                    dew.command("exit");
                };
            }
        })
    );
    form.append(
        $("<cancelbutton>", {
            class: 'cancel',
            id: 'cancel',
            text: ' Cancel ',
            click: function (e) {
                e.preventDefault();
                $(this).parent().parent().remove();
                if (!$('.dialog').length) {
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

    dew.command('Settings.Gamepad', {}).then(function (result) {
        if (result == 1) {
            onControllerConnect();
            hasGP = true;
        } else { hasGP = false; }
    });
});

dew.on("hide", function (event) {
    $(".container").empty();
});

$(document).ready(function () {
    $(document).keydown(function (e) {
        if (e.keyCode === 13 || e.keyCode === 32 || e.keyCode === 89) //Enter, Space, Y
            dew.command("exit");
    });
    $(document).keyup(function (e) {
        if (e.keyCode === 27 || e.keyCode === 66 || e.keyCode === 78) //Escape, B, N
            dew.hide();
    });
});

dew.on('controllerinput', function (e) {
    if (hasGP) {
        if (e.data.Y == 1) {
            dew.command('exit');
        } else if (e.data.B == 1) { dew.hide(); }
    }
});

function onControllerConnect() {
    dew.command('Game.IconSet', {}).then(function (response) {
        $("#exit").append("<img class='exit' src='dew://assets/buttons/" + response + "_Y.png'>");
        $("#cancel").append("<img class='cancel' src='dew://assets/buttons/" + response + "_B.png'>");
    });
};
