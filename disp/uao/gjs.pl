#!/usr/bin/perl

open u2b, "<cp950-u2b.txt";
open b2u, "<uao250-b2u.txt";

%u2b = (); # e000-f8ff private use area, map to big5
%b2u = (); # map big5 to *REAL* unicode

while (<u2b>) {
    next if (/^#/);
    chomp;
    ($big5, $uni) = split / /;
    $u2b{lc($uni)} = $big5;
}
close u2b;

while (<b2u>) {
    next if (/^#/);
    chomp;
    ($big5, $uni) = split / /;
    $b2u{$big5} = lc($uni);
}

close b2u;

print "var uaoTable={";
@tabs = ();
chomp %u2b, %b2u;
foreach (keys %u2b) {
    $v = $b2u{$u2b{$_}};
    $v =~ s///g;
    $v =~ s/^0x//g;
    push @tabs, "$_:'\\u$v'" if ($v);
}
print join ",", sort @tabs;
print <<EOT;
};String.prototype.uao=function(){var _r='';for(var i=0;i<this.length;++i){var c=this.charCodeAt(i);if(c>=0xe000&&c<=0xf8ff&&typeof(uaoTable[c])!='undefined')_r+=uaoTable[c];else _r += this.charAt(i);}return _r;};
EOT
