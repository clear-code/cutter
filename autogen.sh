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
    if brew --version > /dev/null 2>&1; then
      ACLOCAL_OPTIONS="$ACLOCAL_OPTIONS -I $(brew --prefix)/share/aclocal"
      if brew --prefix gettext > /dev/null 2>&1; then
        ACLOCAL_OPTIONS="$ACLOCAL_OPTIONS -I $(brew --prefix gettext)/share/aclocal"
      fi
      : ${LIBTOOLIZE=glibtoolize}
    fi
    ;;
  FreeBSD)
    ACLOCAL_OPTIONS="$ACLOCAL_OPTIONS -I /usr/local/share/aclocal/"
    ;;
esac

run ${INTLTOOLIZE:-intltoolize} --force --copy
run autoreconf --install
