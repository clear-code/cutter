#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT(cutter, 0.1.0, kou@cozmixng.org)
AC_CONFIG_AUX_DIR([config])

AC_CONFIG_SRCDIR([cutter/cut-main.c])
AM_CONFIG_HEADER([cutter/config.h])

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

#AC_SUBST(ac_aux_dir)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wmissing-declarations[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wmissing-declarations" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wmissing-prototypes[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wmissing-prototypes" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wpointer-arith[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wpointer-arith" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wcast-align[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wcast-align" ;;
  esac

  if test "x$enable_ansi" = "xyes"; then
    case " $CFLAGS " in
    *[\ \	]-ansi[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -ansi" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-pedantic[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -pedantic" ;;
    esac
  fi
fi

dnl **************************************************************
dnl Check for GLib.
dnl **************************************************************
GLIB_REQUIRED=2.14.0
AC_SUBST(GLIB_REQUIRED)
AM_PATH_GLIB_2_0($GLIB_REQUIRED, [], [], [gmodule gobject gthread])

# Checks for libraries.
AC_CHECK_LIB(bfd, bfd_init)

AC_ARG_ENABLE([static-binary],
              AS_HELP_STRING([--enable-static-binary],
	                     [Create static binary]))
AM_CONDITIONAL([ENABLE_STATIC_BINARY], [test "$enable_static_binary" = "yes"])

CUTTER_CFLAGS="\$(GLIB_CFLAGS)"
AC_SUBST(CUTTER_CFLAGS)

dnl **************************************************************
dnl Check for GTK+
dnl **************************************************************
AM_PATH_GTK_2_0(2.12.0, [], [], [])
PKG_CHECK_MODULES(GTK_PRINT, gtk+-unix-print-2.0,
                  [gtk_print=yes], [gtk_print=no])
AM_CONDITIONAL([HAVE_GTK], [test "x$no_gtk" = x -a "x$gtk_print" = xyes])

AC_CONFIG_FILES([Makefile
		 cutter.pc
		 libcutter.pc
		 cutter/Makefile
                 sample/Makefile
                 sample/calc/Makefile.test
		 sample/glib/Makefile
		 sample/gtk+/Makefile
                 test/Makefile
		 test/loader_test_dir/Makefile
		 test/repository_test_dir/Makefile])

AC_OUTPUT
