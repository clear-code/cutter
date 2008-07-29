#!/bin/sh

export BASE_DIR="`dirname $0`"

if test x"$NO_MAKE" != x"yes"; then
    make -C $BASE_DIR/../ > /dev/null || exit 1
fi

export CUT_UI_MODULE_DIR=$BASE_DIR/../module/ui/.libs
export CUT_UI_FACTORY_MODULE_DIR=$BASE_DIR/../module/ui/.libs
export CUT_REPORT_MODULE_DIR=$BASE_DIR/../module/report/.libs
export CUT_REPORT_FACTORY_MODULE_DIR=$BASE_DIR/../module/report/.libs
export CUT_STREAM_MODULE_DIR=$BASE_DIR/../module/stream/.libs
export CUT_STREAM_FACTORY_MODULE_DIR=$BASE_DIR/../module/stream/.libs

export GST_PLUGIN_PATH=$BASE_DIR/.libs
gst-launch-0.10 \
  cutter-test-runner test-directory=$BASE_DIR/test ! \
  cutter-console-output verbose-level=v use-color=true

