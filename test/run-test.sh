#!/bin/sh

make \
  && ../cutter/cutter "$@" .libs 
