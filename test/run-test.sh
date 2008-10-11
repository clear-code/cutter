#!/bin/sh

export BASE_DIR="`dirname $0`"
top_dir="$BASE_DIR/.."

if test x"$NO_MAKE" != x"yes"; then
    make -C $top_dir/ > /dev/null || exit 1
fi

if test -z "$CUTTER"; then
    CUTTER="$BASE_DIR/`make -s -C $BASE_DIR echo-cutter`"
fi

CUTTER_ARGS=
CUTTER_WRAPPER=
if test x"$CUTTER_DEBUG" = x"yes"; then
    CUTTER_WRAPPER="$BASE_DIR/../libtool --mode=execute gdb --args"
    CUTTER_ARGS="--keep-opening-modules"
elif test x"$CUTTER_CHECK_LEAK" = x"yes"; then
    CUTTER_WRAPPER="$BASE_DIR/../libtool --mode=execute valgrind "
    CUTTER_WRAPPER="$CUTTER_WRAPPER --leak-check=full --show-reachable=yes -v"
    CUTTER_ARGS="--keep-opening-modules"
fi

export CUTTER
export CUT_UI_MODULE_DIR=$top_dir/module/ui/.libs
export CUT_UI_FACTORY_MODULE_DIR=$top_dir/module/ui/.libs
export CUT_REPORT_MODULE_DIR=$top_dir/module/report/.libs
export CUT_REPORT_FACTORY_MODULE_DIR=$top_dir/module/report/.libs
export CUT_STREAM_MODULE_DIR=$top_dir/module/stream/.libs
export CUT_STREAM_FACTORY_MODULE_DIR=$top_dir/module/stream/.libs

CUTTER_ARGS="$CUTTER_ARGS -s $BASE_DIR --exclude-directory fixtures"
CUTTER_ARGS="$CUTTER_ARGS --exclude-directory lib"
if echo "$@" | grep -- --mode=analyze > /dev/null; then
    :
else
    CUTTER_ARGS="$CUTTER_ARGS --stream=xml --stream-log-directory $top_dir/log"
fi
if test x"$USE_GTK" = x"yes"; then
    CUTTER_ARGS="-u gtk $CUTTER_ARGS"
fi
$CUTTER_WRAPPER $CUTTER $CUTTER_ARGS "$@" $BASE_DIR
