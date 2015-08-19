// ==UserScript==
// @name        minecraft-zh.gamepedia.com.user.js
// @namespace   https://clking.github.io/
// @description automatically select language variant zh-tw
// @include     http://minecraft-zh.gamepedia.com/*
// @version     1
// @grant       none
// ==/UserScript==

(function() {
    var path = location.pathname;

    if (0 > path.indexOf('index.php')) {
        path = path.substring(1);
        location.href = '/index.php?variant=zh-tw&title=' + path;
    }

    var anchors = document.getElementsByTagName('a');
    for (var i = 0 ; i < anchors.length ; ++i) {
        var a = anchors[i];
        var url = a.href;
        var match = /(?:(https?:)?)\/\/(minecraft-zh\.gamepedia\.com)\/(.*)/.exec(url);

        if (!match)
            continue;
        var protocol, host, path;
        protocol = match[1];
        host = match[2];
        path = match[3];

        if (path.indexOf('?') > -1)
           continue;

        if ('Minecraft_Wiki' == path || /^[A-Z][A-Za-z_]*:/.test(path))
           continue;

        a.href = protocol + '//' + host + '/index.php?variant=zh-tw&title=' + path;
    }
})();
