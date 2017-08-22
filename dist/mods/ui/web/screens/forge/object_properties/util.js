function Emitter() {
	this.callbacks = {};
};

Emitter.prototype = {
	on: function(type, callback) {
		if(!this.callbacks[type])
			this.callbacks[type] = [];
		this.callbacks[type].push(callback);
	},
	off: function(type, callback) {
		this.callbacks = this.callbacks[type].filter(c => c != callback);
	},
	emit: function(type, data) {
		var cbs = this.callbacks[type];
		if(!cbs)
			return;
		for(var i = 0; i < cbs.length; i++)
			cbs[i](data);
	}
};