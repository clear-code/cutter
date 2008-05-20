#!/bin/sh

export BASE_DIR="`dirname $0`"

if test x"$NO_MAKE" != x"yes"; then
    make -C $BASE_DIR/../../ > /dev/null || exit 1
fi

export CUT_UI_MODULE_DIR=$BASE_DIR/../../cutter/module/ui/.libs
export CUT_UI_FACTORY_MODULE_DIR=$BASE_DIR/../../cutter/module/ui/.libs
export CUT_REPORT_MODULE_DIR=$BASE_DIR/../../cutter/module/report/.libs
export CUT_REPORT_FACTORY_MODULE_DIR=$BASE_DIR/../../cutter/module/report/.libs
export CUT_STREAMER_MODULE_DIR=$BASE_DIR/../../cutter/module/streamer/.libs
export CUT_STREAMER_FACTORY_MODULE_DIR=$BASE_DIR/../../cutter/module/streamer/.libs

$BASE_DIR/cutter-cairo \
    --color=auto --multi-thread -s $BASE_DIR "$@" $BASE_DIR
