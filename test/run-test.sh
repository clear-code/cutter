#!/bin/sh

base_dir=`dirname $0`
make > /dev/null && \
  $base_dir/../cutter/cutter "$@" $base_dir/.libs
