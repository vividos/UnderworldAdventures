#! /bin/sh

autoheader
libtoolize --automake
aclocal
autoconf
automake --add-missing
automake

