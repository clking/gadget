#!/usr/bin/perl

use feature qw(say);

use AE;
use Time::HiRes;

say 'time(): ' . time;
say 'AE::now(): ' . AE::now;
say 'Time::HiRes::time(): ' . Time::HiRes::time;
