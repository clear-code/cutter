#!/bin/sh

export BASE_DIR="`dirname $0`"

if test x"$NO_MAKE" != x"yes"; then
    make -C $BASE_DIR/../ > /dev/null || exit 1
fi

export CUT_OUTPUT_MODULE_DIR=$BASE_DIR/../cutter/module/output/.libs
$BASE_DIR/../cutter/cutter --color=auto --test-case=/^test_.*/ --multi-thread \
    -s $BASE_DIR "$@" $BASE_DIR
