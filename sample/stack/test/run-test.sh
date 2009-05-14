#!/bin/sh

export BASE_DIR="`dirname $0`"

if test -z "$NO_MAKE"; then
    make -C $BASE_DIR/../ > /dev/null || exit 1
fi

if test -z "$CUTTER"; then
    CUTTER="`make -s -C $BASE_DIR echo-cutter`"
fi

case `uname` in
    CYGWIN*)
	PATH="$top_dir/src/.libs:$PATH"
	;;
    *)
	:
	;;
esac

$CUTTER -s $BASE_DIR "$@" $BASE_DIR
