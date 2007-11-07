#!/bin/sh

export BASE_DIR="`dirname $0`"

if test x"$NO_MAKE" != x"yes"; then
    make > /dev/null || exit 1
fi

$BASE_DIR/../cutter/cutter --color=auto --name=/test_.*/ -s $BASE_DIR "$@" $BASE_DIR
