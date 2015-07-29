function disp() {
    var doku = document.getElementById('doku'),
	st = document.getElementById('srctext');
    doku.removeChild(doku.firstChild);
    if (st.style.display == 'block' || '' == st.style.display) {
	doku.appendChild(document.createTextNode('散'));
	st.style.display = 'none';
	parseInput(st.value);
    } else {
	doku.appendChild(document.createTextNode('毒'));
	st.style.display = 'block';
	clearOutput();
    }
}

function parseInput(val) {
    val = val.replace(/\x1b/g, '');
    var lines = val.split('\n');
    var textContent = '';
    var re, matches, ln, res = [
	{
	    pattern: /^(\[.*m)?\s*作者:?\s*(.*)\s*(看板|站內)\s*(.*)\s*$/,
	    handler: function(matches) {setAuthor(matches[2]);setBoardName(matches[4]);}
	},
	{
	    pattern: /^(\[.*m)?\s*標題:?\s*(.*)\s*$/,
	    handler: function(matches) {setTitle(matches[2]);}
	},
	{
	    pattern: /^(\[.*m)?\s*時間:?\s*(.*)\s*$/,
	    handler: function(matches) {setTime(matches[2]);}
	},
	{
	    pattern: /^(\[36m)?[-─=－＝]{3,}(\[m)?/,
	    handler: function() {}
	}
    ];
    var div, span;
    re = /https?:\/\/[a-zA-Z0-9\-\.]+\.[a-zA-Z0-9\-]*\/[~\w\-%&\?\.#\/\+=]*/i;
    do {
        ln = lines.shift();
	ln = ln.replace(/h\s*t\s*t\s*p(s?):\/\//g, 'http$1://');
	var matched = false;
	for (var i = 0 ; i < res.length ; ++i) {
	    if (res[i].pattern.test(ln)) {
		matches = res[i].pattern.exec(ln);
		res[i].handler(matches);
		matched = true;
		break;
	    }
	}
	if (matched)
	    continue;
        div = document.createElement('div');
        while (re.test(ln)) {
            matches = re.exec(ln);
            var href = matches[0];
            var index = ln.indexOf(href);
            span = document.createElement('span');
	    textContent = '';
            if (index > 0) {
                textContent = ln.substring(0, index);
                ln = ln.substring(index + href.length);
            } else if (index == 0)
                ln = ln.substring(index + href.length);
            span.appendChild(ripColor(textContent));
            div.appendChild(span);
            var ch;
            if (/\.(jpg|jpeg|png|gif)/.test(href)) {
                ch = document.createElement('img');
                ch.src = href;
                ch.title = href;
                ch.alt = href;
            } else {
		var linkNode = null;
		for (var i = 0 ; i < site.length ; ++i)
		    if (site[i].re.test(href))
			linkNode = site[i].action(href);
		if (linkNode == null)
		    linkNode = ripColor(decodeURIComponent(href), true, true);
                var ch = document.createElement('a');
                ch.href = href;
                ch.target = '_blank';
                ch.appendChild(linkNode);
            }
            div.appendChild(ch);
        }
        span = document.createElement('span');
        span.appendChild(ripColor(ln));
        div.appendChild(span);
        bd.appendChild(div);
    } while (lines.length > 0);
    setTimeout(zoom, 800);

    hd.style.display = 'block';
    bd.style.display = 'block';
}

function ripColor(ln, noRoot, ignoreStyle) {
    var root, span;
    root = document.createElement('span');
    var runs = 0;
    while (ccre.test(ln)) {
        matches = ccre.exec(ln);
        var index = ln.indexOf(matches[0]),
	    whole = matches[0],
	    bbscolor = matches[1],
	    preText = '';
	if (index > 0) {
	    preText = ln.substring(0, index);
	    ln = ln.substring(index + whole.length);
	} else if (index == 0) {
	    ln = ln.substring(index + whole.length);
	}
	if (preText.length > 0) {
	    span = document.createElement('span');
	    span.appendChild(document.createTextNode(preText.uao()));
	    if (!ignoreStyle) {
		for (var e in lastStyle) {
		    switch (e) {
			case 'foreground':
			    span.style.color = lastStyle.foreground;
			    break;
			case 'background':
			    span.style.backgroundColor = lastStyle.background;
			    break;
		    };
		}
	    }
	    root.appendChild(span);
	}
	if (bbscolor.length > 0) {
	    if (bbscolor.indexOf(';') > -1) {
		bbscolor = bbscolor.split(';');
	    } else
		bbscolor = [bbscolor];
	    for (var i = 0 ; i < bbscolor.length ; ++i) {
		bbc = parseInt(bbscolor[i]);
		if (bbc >= 30 && bbc <= 47) {
		    if (bbc >= 40) {
			lastStyle.background = cmap.nm[bbc % 10];
		    } else {
			lastStyle.foreground = lastStyle.highlight ? cmap.hl[bbc % 10] : cmap.nm[bbc % 10];
		    }
		} else {
		    switch (bbc) {
			case 1:
			    lastStyle.highlight = true;
			    break;
			default:
			    lastStyle = {};
		    };
		}
	    }
	} else {
	    lastStyle = {};
	}
    }
    if (ln.length > 0) {
	span = document.createElement('span');
	if (!ignoreStyle) {
	    for (var e in lastStyle) {
		switch (e) {
		    case 'foreground':
			span.style.color = lastStyle.foreground;
			break;
		    case 'background':
			span.style.backgroundColor = lastStyle.background;
			break;
		};
	    }
	}
	span.appendChild(document.createTextNode(ln.uao()));
        root.appendChild(span);
    }

    return noRoot ? root.firstChild : root;
}

function clearOutput() {
    hd.style.display = 'none';
    bd.style.display = 'none';
    while (bd.childNodes.length > 0)
        bd.removeChild(bd.firstChild);
    setAuthor('');
    setBoardName('');
    setTitle('');
    setTime('');
}

// page header mutators {{{
function setHeading(id, val) {
    var e = document.getElementById(id);
    if (typeof(e) == 'undefined' || e == null)
        return;
    if (e.childNodes.length > 0)
        e.removeChild(e.firstChild);
    e.appendChild(document.createTextNode(val.replace(new RegExp(ccre.source, 'g'), '').uao()));
}
function setAuthor(val) {
    setHeading('author', val);
}
function setBoardName(val) {
    setHeading('board', val);
}
function setTitle(val) {
    document.title = TITLE;
    if (val.length > 0)
	document.title += ' - ' + ripColor(val, true).firstChild.data;
    setHeading('atitle', val);
}
function setTime(val) {
    setHeading('atime', val);
} // }}}

// event handlers {{{
function clickOnImage(e) {
    var img = e.target;
    var mw = img.style.maxWidth;
    img.style.maxWidth = mw == '100%' || mw == '' ? 'none' : '100%';
}

function onhover(e) {
    var img = e.target;
    img.style.maxWidth = 'none';
}

function offhover(e) {
    var img = e.target;
    setTimeout(function() { img.style.maxWidth = '100%'; }, 800);
}

function zoom() {
    var checked = shrink.checked;
    var images = bd.getElementsByTagName('img');
    for (var i = 0 ; i < images.length ; ++i) {
        if (checked) {
	    images[i].style.maxWidth = 'none';
	    if (images[i].clientWidth > images[i].parentNode.clientWidth) {
		images[i].style.maxWidth = '100%';
		// images[i].onmouseover = onhover;
		// images[i].onmouseout = offhover;
		images[i].onclick = clickOnImage;
		images[i].style.cursor = 'pointer';
	    }
        } else {
            images[i].style.maxWidth = 'none';
            images[i].onmouseover = function() {};
        }
    }
    scrollTo(0, 0);
} // }}}

function init(e) {
    hd = document.getElementById('heading');
    bd = document.getElementById('content');
    srctext = document.getElementById('srctext');
    doku = document.getElementById('doku');
    shrink = document.getElementById('shrink');

    ccode = String.fromCharCode(0x1b);
    ccre = /\[([\d;]*)m/;
    cmap = {
	nm: ['black', '#7f0000', '#007f00', '#7f7f00', '#00007f', '#7f007f', '#007f7f', '#c0c0c0'],
	hl: ['#7f7f7f', '#ff0000', '#00ff00', '#ffff00', '#0000ff', '#ff00ff', '#00ffff', 'white']
    };
    lastStyle = {};

    registerSites();

    var rdr = false;
    if (navigator.userAgent.indexOf('Gecko') < 0) {
	rdr = true;
	if (window.confirm('本製毒程式在 Mozilla Firefox 以外的瀏覽器（尤其是天殺的 IE）上使用可能會發生不可預期的錯誤，甚至會導致霹靂星球爆炸。\n請問您確定要繼續使用嗎？'))
	    if (window.confirm('想清楚哦，你的決定可能會讓屁力星球爆炸而屁屁貓們逃不出來哦！\n確定？'))
		if (window.confirm('踏碼的，我都這樣警告你了你還要讓屁力星球爆炸嗎？')) {
		    alert('口亨，隨便你好了，我才不管你會怎麼樣呢(甩頭)');
		    rdr = false;
		}
    }
	    
    if (rdr)
	window.location = 'http://tw.yahoo.com/';

    TITLE = '好毒';
}

var hd, bd, srctext, doku, shrink;
var ccode, ccre, cmap, lastStyle; // color code pattern and mapping
var site;
var TITLE;
