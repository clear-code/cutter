#!/bin/sh

export BASE_DIR="`dirname $0`"
make > /dev/null && \
  $BASE_DIR/../cutter/cutter -s $BASE_DIR "$@" $BASE_DIR
