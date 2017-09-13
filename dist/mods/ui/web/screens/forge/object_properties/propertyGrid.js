function makePropertyGridWidget(_containerElem) {
	var emitter = new Emitter();
	var _cachedCount = 0;
	var _meta = {};
	var _strings = {};

	var api = {
		on: (type, callback) => emitter.on(type, callback),
		handleUiInput(uiButtonCode) {
			switch(uiButtonCode) {
				case 0: //eUiButtonCodeA
					$selected = getSelected();
					var name = $selected.data('property-name');
					emitter.emit('select', name);
					dew.command('Game.PlaySound 0xb00');
				break;
				case 8:
				case 9:
					listController.navigate(uiButtonCode == 8 ? -1 : 1);
					dew.command('Game.PlaySound 0xafe');
				break;
				case 10:
				case 11:
					if(navigateHorizontal(uiButtonCode == 10 ? -1 : 1))
						dew.command('Game.PlaySound 0xaff');
					
				break;
			}
		},
		populateValues: (initialValues) => {
			var keys = Object.keys(initialValues);
			for(var i = 0; i < keys.length; i++) {
				var key = keys[i], value = initialValues[key];
				_containerElem.find('#' + key).val(value);
			}

			_containerElem.find('.property-item-range input').each((i, e) => updateRangeOutput(e.id, $(e)));
			_containerElem.find('.property-item-material select').each((i, e) => {
				var thumbnail = _meta[e.id][$(e).val()].thumbnail;
				$(e).prev('img').attr('src', thumbnail);
			});
		},
		populate(data, initialValues = {}) {
			_meta = data.meta;
			_strings = data.strings;

			_containerElem.html(buildPropertiesHtml(data.properties));

			cacheCount();

			this.populateValues(initialValues);
			
		},
		setSelectedIndex: (index) => {
			listController.setSelectedIndex(index);
		},
		selectedIndex: () => listController.selectedIndex(),
		toggleVisibility: (name, visible) => {
			var items = _containerElem.find(`.property-item`);
			for(var i = 0; i < items.length; i++) {
				var item = $(items[i]);
				if(item.data('property-name') === name) {
					item.toggleClass('hidden', !visible);
				}
			}
		
			cacheCount();
		}
	};

	var listController = makeListWidgetController('property_list', {wrapAround: true}, {
		count: () => _cachedCount
	});

	_containerElem.on('change input', '.property-item-leaf', function(e) {
		e.stopPropagation();
		var name = e.target.id;
		var value = e.target.value;
		var type = $(this).data('property-type');

		if(type == 'range') {
			updateRangeOutput(name, $(e.target))
		}

		emitter.emit('propertyChange', {name: name, value: parseFloat(e.target.value)});	
	});

	_containerElem.on('click',  '.property-item-leaf', function(e) {
		if(e.button != 0)
			return;
		if($(e.target).is('input') || $(e.target).is('select'))
			return;

		console.log('blah');

		$selected = getSelected();
		var name = $selected.data('property-name');
		api.handleUiInput(0);
	});

	_containerElem.on('mouseenter mouseleave mousemove',  '.property-item-leaf', function(e) {
		_containerElem.find('.property-item-leaf').not('.hidden').removeClass('selected');
		$(this).addClass('selected')[0];
	});

	listController.on('selectedIndexChanged', function({index}) {
		$e = _containerElem.find('.property-item-leaf').not('.hidden')
		.removeClass('selected')
		.eq(index).addClass('selected');	

		if(!$e.length)
			return;

		$e[0].scrollIntoView(false);
	});

	return api;

	function cacheCount() {
		_cachedCount = _containerElem.find('.property-item-leaf').not('.hidden').length;
	}

	function updateRangeOutput(name, $range) {
		switch(_meta[name].type) {
			case 'int':
				$range.nextAll('output').text(parseInt($range.val()));
				break;
			default:
				$range.nextAll('output').text(parseFloat($range.val()).toFixed(2));
		}
	}

	function getSelected() {
		return _containerElem.find('.property-item-leaf.selected');
	}

	function navigateHorizontal(dir) {
		var $current = getSelected();
		if(!$current.length)
			return;

		var propertyType = $current.data('property-type');
		switch(propertyType) {
			case 'range':
				rangeElem = $current.find('input[type=range]').get(0);
				if(dir < 0)
					rangeElem.stepDown();
				else
					rangeElem.stepUp();
				$(rangeElem).trigger('change');
			break;
			case 'float':
				$elem = $current.find('input').first();
				var value = parseFloat($elem.val());
				if(dir < 0)
					value -= 0.01;
				else
					value += 0.01;
				$elem.val(value.toFixed(4));
				$elem.trigger('change');
			break;
			case 'material':
			case 'spinner':
				$nextOption = $option = $current.find('select option:selected');
				$option.prop('selected',false);
				if(dir < 0) {
					$nextOption = $option.prev('option');
					if(!$nextOption.length) {
						$nextOption = $current.find('select option').last();
					}
				}
				else {
					$nextOption = $option.next('option');
				}
				if($nextOption != $option) {

					$nextOption.prop('selected', true);
					$($current).find('select').trigger('change');
				}		
			break;
			case 'action':
			case 'static':
				return false;
		}

		return true;
	}

	function buildPropertyHtml(property) {
		var meta = _meta[property.meta];

		switch(property.type) {
			case 'spinner':	
			return `
				<select id="${property.name}">
					${meta.map(x => `<option value="${x.constructor === Object ? x.value : x}">${x.constructor === Object ? x.name : x}</option>`)};
				</select>
			`;
			case 'range':

			return `
				<input id="${property.name}" type="range" min="${meta.min}" max="${meta.max}" step="${meta.step}">
				<output id="${property.name}_value"></output>
			`;
			case 'float':
			return `
				<input id="${property.name}" type="text" value="0.0000" readonly>
			`;
			case 'action':
			return `
				<strong id="${property.name}">+</strong>
			`
			case 'material':
			return `
				<img id="${property.name}_material_preview" />
				<select id="${property.name}" style="display: none">
					${meta.map(x => `<option value="${x.constructor === Object ? x.value : x}">${x.constructor === Object ? x.name : x}</option>`)};
				</select>
			`
			case 'static':
			return `
				<span readonly id="${property.name}">${meta}</span>
			`;
			
		}
	}

	function buildPropertiesHtml(properties, html) {

		html = html || '';
		for(var i = 0; i < properties.length; i++) {

			var prop = properties[i];
			if(prop.values && prop.values.constructor === Array) {
				html += `
				<div class="property-item property-item-group" data-property-name="${prop.name}" id="${prop.name}_group">
					<div class="group-heading">${_strings[prop.name] || prop.name}</div>
					<div class="group-body">
					${buildPropertiesHtml(prop.values, '')}
					</div>
				</div>`;
			}
			else {
				
				html += `
				<div class="property-item property-item-leaf property-item-${prop.type}" data-property-name="${prop.name}" data-property-type="${prop.type}">
					<span class="property-name">${_strings[prop.name] || prop.name}</span>
					<span class="property-value">
					${buildPropertyHtml(prop)}
					</span>
				</div>
				`;
			}
		}
		return html;
	}
}