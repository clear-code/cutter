#!/bin/sh

set -u

run()
{
  "$@"
  if test $? -ne 0; then
    echo "Failed $@"
    exit 1
  fi
}

# for old intltoolize
if [ ! -e config/po ]; then
  ln -s ../po config/po
fi

case $(uname -s) in
  Darwin)
    homebrew_aclocal=/usr/local/share/aclocal
    if [ -d $homebrew_aclocal ]; then
      ACLOCAL_OPTIONS="$ACLOCAL_OPTIONS -I $homebrew_aclocal"
    fi
    gettext_aclocal="$(echo /usr/local/Cellar/gettext/*/share/aclocal)"
    if [ -d $gettext_aclocal ]; then
      ACLOCAL_OPTIONS="$ACLOCAL_OPTIONS -I $gettext_aclocal"
    fi
    : ${LIBTOOLIZE=glibtoolize}
    ;;
  FreeBSD)
    ACLOCAL_OPTIONS="$ACLOCAL_OPTIONS -I /usr/local/share/aclocal/"
    ;;
esac

run ${INTLTOOLIZE:-intltoolize} --force --copy
run autoreconf --install
