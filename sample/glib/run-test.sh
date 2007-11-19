#!/bin/sh

export BASE_DIR="`dirname $0`"

if test x"$NO_MAKE" != x"yes"; then
    make -C $BASE_DIR/../../ > /dev/null || exit 1
fi

export CUT_RUNNER_MODULE_DIR=$BASE_DIR/../../cutter/module/runner/.libs
CUTTER=$BASE_DIR/../../cutter/cutter
if test x"$CUTTER_DEBUG" = x"yes"; then
    CUTTER="$BASE_DIR/../../libtool --mode=execute gdb --args $CUTTER"
fi
$CUTTER --color=auto --multi-thread -s $BASE_DIR "$@" $BASE_DIR
