// ==UserScript==
// @name        zh-tw wikipedia
// @namespace   https://gist.github.com/clking
// @description automatically select language variant zh-tw
// @include     https://zh.wikipedia.org/*
// @version     1
// @grant       none
// ==/UserScript==

(function() {
    var path = location.pathname;
    if (0 >  path.indexOf('/zh-tw/') && !path.endsWith('.php')) {
        path = path.substring(1).split('/');
        path[0] = '/zh-tw';
        location.href = path.join('/');

        return;
    }

    var anchors = document.getElementsByTagName('a');
    for (var i = 0 ; i < anchors.length ; ++i) {
        var a = anchors[i];
        var url = a.href;

        var match = /(?:(https?:)?)\/\/(zh\.wikipedia\.org)\/([^\/]+)\/(.*)/.exec(url);
        if (!match)
            continue;

        var protocol, host, variant, path;
        protocol = match[1];
        host = match[2];
        variant = match[3];
        path = match[4];

        if ('zh-tw' === variant)
            continue;

        if (/^[A-Z][A-Za-z_]*:/.test(path))
           continue;

        a.href = protocol + '//' + host + '/zh-tw/' + path;
    }
})();
