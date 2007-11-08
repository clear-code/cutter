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
AM_PATH_GLIB_2_0(2.14.0, [], [], [gmodule gobject gthread])

# Checks for libraries.
AC_CHECK_LIB(bfd, bfd_init, [], [AC_ERROR("-lbfd not found")])

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h dlfcn.h direct.h fnmatch.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_REPLACE_FNMATCH
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gettimeofday strerror fnmatch])

CUTTER_CFLAGS="\$(GLIB_CFLAGS)"
AC_SUBST(CUTTER_CFLAGS)

AC_CONFIG_FILES([Makefile
		 cutter.pc
		 cutter/Makefile
                 sample/Makefile
                 sample/calc/Makefile.test
		 sample/glib/Makefile
                 test/Makefile
		 test/loader_test_dir/Makefile
		 test/repository_test_dir/Makefile])

AC_OUTPUT
