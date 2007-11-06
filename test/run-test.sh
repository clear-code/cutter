#!/bin/sh

base_dir=`dirname $0`
make > /dev/null && \
  $base_dir/../cutter/cutter -s $base_dir "$@" $base_dir/.libs
