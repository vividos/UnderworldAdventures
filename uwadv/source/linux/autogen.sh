#! /bin/sh

autoheader
libtoolize --automake
aclocal
autoconf
#automake -a
automake --foreign

