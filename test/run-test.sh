#!/bin/sh

base_dir=`dirname $0`
make > /dev/null && \
  $base_dir/../cutter/cutter -v v -s $base_dir "$@" $base_dir/.libs
