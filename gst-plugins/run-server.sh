#!/bin/sh

export BASE_DIR="`dirname $0`"

export CUT_UI_MODULE_DIR=$BASE_DIR/../module/ui/.libs
export CUT_UI_FACTORY_MODULE_DIR=$BASE_DIR/../module/ui/.libs
export CUT_REPORT_MODULE_DIR=$BASE_DIR/../module/report/.libs
export CUT_REPORT_FACTORY_MODULE_DIR=$BASE_DIR/../module/report/.libs
export CUT_STREAM_MODULE_DIR=$BASE_DIR/../module/stream/.libs
export CUT_STREAM_FACTORY_MODULE_DIR=$BASE_DIR/../module/stream/.libs

export GST_PLUGIN_PATH=$BASE_DIR/.libs
gst-launch-0.10 \
  cutter-test-runner test-directory=$BASE_DIR/test ! \
  tcpclientsink host=192.168.1.27 port=50000
