#!/bin/sh

make \
  && cutter/cutter --verbose=p --base=test test/.libs
