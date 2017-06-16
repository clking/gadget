(function() {
    window.agingClock = function(selector) {
	var e = document.body.querySelectorAll(selector);

	var now = Date.now();
	for (var i = 0 ; i < e.length ; ++i) {
	    var element = e[i];
	    element.started = Date.parse(element.textContent) || element.textContent || now;
	    element.setAttribute('data-started', element.started);

	    element.timer = setInterval(function(element, i) {
		var age = Math.round((Date.now() - element.started) / 1000);
		var string = '';
		var unit = [[86400, '+',], [3600, ':'], [60, ':']];

		for (var j = 0; j < unit.length ; ++j) {
		    var threshold = unit[j][0], suffix = unit[j][1];
		    if (age >= threshold) {
			var n = Math.floor(age / threshold);
			if (n < 10) n = '0' + n;
			age %= threshold;
			string += [n, suffix].join('');
		    }
		}
		if (age < 10) age = '0' + age;
		string += age;

		element.textContent = string;
	    }, 500, element, i);
	}
    }
})()
