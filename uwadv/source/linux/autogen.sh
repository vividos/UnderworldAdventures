#! /bin/sh

autoheader
libtoolize --automake
aclocal
autoconf
automake --copy --force --add-missing

