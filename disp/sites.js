function registerSites() {
    site = [
	{ name: 'imgur.com', re: /http:\/\/imgur\.com\//, action: function(h) { var e = document.createElement('img'); e.src = h + '.jpg'; e.title = e.alt = h; return e; } },
        { name: 'inspic.com', re: /http:\/\/inspic\.com\//, action: function(h) { var e = document.createElement('iframe'); e.src = h; e.style.width = '100%'; e.style.height = '800px'; return e; } }
    ];
}
