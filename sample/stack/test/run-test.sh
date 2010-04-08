#!/bin/sh

export BASE_DIR="`dirname $0`"
top_dir="$BASE_DIR/.."

if test -z "$NO_MAKE"; then
    make -C $top_dir > /dev/null || exit 1
fi

if test -z "$CUTTER"; then
    CUTTER="`make -s -C $BASE_DIR echo-cutter`"
fi

case `uname` in
    CYGWIN*)
	PATH="$top_dir/src/.libs:$PATH"
	;;
    Darwin)
	DYLD_LIBRARY_PATH="$top_dir/src/.libs:$DYLD_LIBRARY_PATH"
	export DYLD_LIBRARY_PATH
	;;
    *)
	:
	;;
esac

$CUTTER -s $BASE_DIR "$@" $BASE_DIR
