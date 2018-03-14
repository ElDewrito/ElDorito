lastTime = Date.now();

dew.on('screenshot_taken', function(e) {
	let currentTime = Date.now();
	if(currentTime - lastTime > 2500)
	{
		lastTime = currentTime;
		dew.toast({
			body:`
				<div style="margin-bottom:7px;">SCREENSHOT SAVED</div>
				<div style="font-size:0.75em">&quot;${e.data.filepath}&quot;</div>
				`
		});
	}
});