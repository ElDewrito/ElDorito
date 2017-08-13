function fadeOut() {
	$(document.body).fadeOut( "fast", function() {
		dew.hide();
	});
}

var hide_timer

dew.on("show", function (event) {
	$(document.body).stop()
	clearTimeout(hide_timer)

    $(document.body).show();
	$('#savedto').html(document.createTextNode(event.data.filepath));

	hide_timer = setTimeout(fadeOut, 2000);
});