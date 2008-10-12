#!/bin/sh

run()
{
    $@
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

run aclocal ${ACLOCAL_ARGS}
run libtoolize --copy --force
run autoheader
run automake --add-missing --foreign --copy
run autoconf
