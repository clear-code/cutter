#!/bin/sh

export BASE_DIR="`dirname $0`"
$CUTTER --color=auto -s $BASE_DIR "$@" $BASE_DIR
