#!/bin/sh

run()
{
    $@
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

run aclocal $ACLOCAL_OPTIONS
run libtoolize --copy --force
run glib-gettextize --force --copy
#run gtkdocize --copy
run autoheader
run automake --add-missing --foreign --copy
run autoconf
