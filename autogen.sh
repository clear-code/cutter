#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

run()
{
    $@
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

run libtoolize --copy --force
run gtkdocize --copy
run aclocal
run autoheader
run automake --add-missing --foreign --copy
run autoconf
