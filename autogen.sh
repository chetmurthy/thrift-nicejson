#!/bin/sh -x

aclocal
autoheader
#libtoolize --copy --force
automake
autoconf

#autoreconf -i -f -v
