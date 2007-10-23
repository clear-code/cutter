#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT(cutter, 0.0.2, kou@cozmixng.org)
AC_CONFIG_AUX_DIR([config])

AC_CONFIG_SRCDIR([cutter/cutter.c])
AM_CONFIG_HEADER([cutter/config.h])

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

#AC_SUBST(ac_aux_dir)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl **************************************************************
dnl Check for GLib.
dnl **************************************************************
AM_PATH_GLIB_2_0(2.14.0, [], [], [gmodule gobject])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [dlopen])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [ceil])

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

AC_CONFIG_FILES([Makefile
		 cutter/Makefile
                 sample/Makefile
                 sample/calc/Makefile.test
                 CUnit/Makefile
                 test/Makefile])

AC_OUTPUT
