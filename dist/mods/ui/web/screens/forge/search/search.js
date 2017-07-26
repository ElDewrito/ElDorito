(function() {

var MAX_RESULTS = 7;

var results = [];
var history = makeHistory();
var fuse;
var usingMouse = false;

$(function() {
	$(window).on('keydown', function(e) {
		if(!fuse)
			return;

		var dir = 0;
		switch(e.keyCode) {
			case 27: // escape
				dew.command('Game.PlaySound 0xb01');
				clear();
				return dew.hide();
			case 13: // return
				if(!results.length)
					performSearch($('#search_text').val());
				else
					spawnSelectedItem();
				return;
			case 38: // up
			case 40: // down
				navigate(e.keyCode == 38 ? -1 : 1);
				e.preventDefault();
				e.stopPropagation();
			break;
		}
	});

	$('#search_text').on('input', function(e) {
		performSearch($(this).val());
	});

	$('.search-container')
		.on('mouseenter mouseleave', '.search-result-item', function(e) {
			$('.search-container .search-result-item').removeClass('selected');
			$(this).addClass('selected');
		})
		.on('click', '.search-result-item', function(e) {
			spawnSelectedItem();
		});
});

dew.on('show', function() {
	dew.command('Game.PlaySound 0xb02');

	results = [];
	history.reset();

	$('#search_text').focus();
	dew.command('Forge.DumpPalette', {}).then(function(e) {
		fuse = new Fuse(JSON.parse(e),  {
		  shouldSort: true,
		  threshold: 0.6,
		  location: 0,
		  distance: 100,
		  maxPatternLength: 32,
		  minMatchCharLength: 1,
		  keys: ["name"]
		});
	});
});

function navigate(dir) {
	if(results.length) {	
		navigateSelection(dir);
	}
	else {
		navigateHistory(dir);
	}
}

function navigateSelection(dir) {
	var $current = getSelected();
	if(!$current.length) {
		$('#search_results .search-result-item').first().addClass('selected');
		return;
	}

	var $next = null;
	if(dir > 0) {	
		$next =  $current.next();
		if(!$next.length)
			$next = $('#search_results').find('.search-result-item').first();
	}
	else {
		$next = $current.prev();
		if(!$next.length)
			$next = $('#search_results').find('.search-result-item').last();
	}

	$selected = getSelected();
	if($selected && $selected.length)
		$selected.removeClass('selected');
	if($next && $next.length) {
		dew.command('Game.PlaySound 0xafe');
		$next.addClass('selected');
	}
}

function navigateHistory(dir) {

	var oldValue = history.current();
	if(dir < 0) 
		history.forward();
	else 
		history.back();

	if(oldValue != history.current())
		dew.command('Game.PlaySound 0xaff');

	$('#search_text').val(history.current());
}

function getSelected() {
	return $('#search_results').find('.search-result-item.selected');
}

function clear() {
	$('#search_results').html('');
	$('#search_text').val('');
}

function spawnItem(tagIndex) {
	dew.command('Game.PlaySound 0xb00');
	dew.command('Forge.SpawnItem ' + tagIndex);
	dew.hide();
}

function spawnSelectedItem() {
	var $selected = getSelected();
	if($selected.length) {
		var tagIndex = $selected.data('tag-index');
		var name = $selected.data('name');
		if(!tagIndex)
			return;

		spawnItem(tagIndex);
		history.add(name);
		clear();
	}
}

function toTitleCase(str) {
	var result = '', nextUpper = true;
	for(var i = 0; i < str.length; i++) {
		var c = nextUpper ? str[i].toUpperCase() : str[i].toLowerCase();
		nextUpper = c == ' ';	
		result += c;
	}
	return result;
}

function buildSearchResultTitle(item) {
	return '<span class="item-remaining">' + item.remaining + '</span><a href="#">' + toTitleCase(item.name) + '</a>';
}

function buildHtmlSearchResults(results) {
	var html = '';
	var numResultsToDisplay = Math.min(MAX_RESULTS, results.length);

	for(var i = 0; i < numResultsToDisplay; i++) {
		var item = results[i];
		html += '<li class="search-result-item" data-tag-index="' + item.tagIndex + '" data-name="' + item.name + '">' + buildSearchResultTitle(item)  + '</li>';
	}
	return '<ul class="search-results">' + html + '</ul>';
}

function performSearch(query) {
	if(!fuse)
		return;
	results = fuse.search(query);
	$('#search_text').focus();
	$('#search_results').html(buildHtmlSearchResults(results));
	$('#search_results .search-result-item').removeClass('selected');
	$('#search_results .search-result-item').first().addClass('selected');
}

function makeHistory(comparator) {
	var history = [], index = -1;
	comparator = comparator || function(a,b) { return a === b };
	return {
		reset: function() {
			index = -1;
		},
		clear: function() {
			history = [];
		},
		add: function(item) {
			if(!item)
				return;
			history = history.filter(function(x) { return !comparator(x, item); });
			history.unshift(item);
		},
		remove: function(item) {
			history = history.filter(comparator);
		},
		current: function() {
			return history[index];
		},
		forward: function() {
			if(++index > history.length-1)
				index = 0;
		},
		back: function() {
			if(--index < 0)
				index = history.length-1;
			console.log(index);
		}
	}
}

})();