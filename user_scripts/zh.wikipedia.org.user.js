// ==UserScript==
// @name        zh-tw wikipedia
// @namespace   https://gist.github.com/clking
// @description automatically select language variant zh-tw
// @include     https://zh.wikipedia.org/*
// @version     1
// @grant       none
// ==/UserScript==

(function(){
    var path = location.pathname;
    if (-1 < path.indexOf('/zh-tw/'))
        return;

    path = path.substring(1).split('/');
    path[0] = '/zh-tw';
    location.href = path.join('/');
})();
