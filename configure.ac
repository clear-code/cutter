#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT(cutter, 1.0.2, kou@cozmixng.org)
AC_CONFIG_AUX_DIR([config])

AC_CONFIG_SRCDIR([cutter/cut-main.c])
AM_CONFIG_HEADER([cutter/config.h])

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

PACKAGE_TITLE=Cutter
AC_SUBST(PACKAGE_TITLE)

AC_CANONICAL_HOST

AC_MSG_CHECKING([for native Win32])
case "$host" in
  *-*-mingw*)
    os_win32=yes
    ;;
  *)
    os_win32=no
    ;;
esac
AC_MSG_RESULT([$os_win32])

AC_MSG_CHECKING([for some Win32 platform])
case "$host" in
  *-*-mingw*|*-*-cygwin*)
    platform_win32=yes
    ;;
  *)
    platform_win32=no
    ;;
esac
AC_MSG_RESULT([$platform_win32])

AM_CONDITIONAL(OS_WIN32, test "$os_win32" = "yes")
AM_CONDITIONAL(PLATFORM_WIN32, test "$platform_win32" = "yes")

AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

LT_CURRENT=1
LT_REVISION=0
LT_AGE=1
LT_VERSION_INFO="$LT_CURRENT:$LT_REVISION:$LT_AGE"
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)
AC_SUBST(LT_VERSION_INFO)

LT_CURRENT_MINUS_AGE=`expr $LT_CURRENT - $LT_AGE`
AC_SUBST(LT_CURRENT_MINUS_AGE)

LIBTOOL_EXPORT_OPTIONS='-export-symbols-regex "^[[^_]].*"'
AC_SUBST(LIBTOOL_EXPORT_OPTIONS)


# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

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
fi

# Checks for headers
AC_HEADER_SYS_WAIT
AC_CHECK_HEADER(io.h,
                [AC_DEFINE(HAVE_IO_H, 1, [Define to 1 if you have <io.h>.])])

dnl **************************************************************
dnl Check for MinGW
dnl **************************************************************

AC_ARG_ENABLE([mingw-debug],
              AS_HELP_STRING([--enable-mingw-debug],
	                     [Enable MinGW debug]))
case $host_os in
mingw*)
  if test "x$enable_mingw_debug" != "xno"; then
    LDFLAGS="$LDFLAGS -mwindows"
  fi
  AC_CHECK_TOOL(WINDRES, windres)
  ;;
esac
AM_CONDITIONAL([HAVE_WINDRES], [test x"$WINDRES" != x])

dnl **************************************************************
dnl Check for GLib.
dnl **************************************************************
GLIB_REQUIRED=2.16.0
AC_SUBST(GLIB_REQUIRED)
AM_PATH_GLIB_2_0($GLIB_REQUIRED,
                 [],
                 [AC_MSG_ERROR([GLib >= $GLIB_REQUIRED required.])],
                 [gmodule gobject gthread])

# Checks for libraries.
AC_MSG_CHECKING([whether to try to use BFD])
AC_ARG_ENABLE([bfd],
              AS_HELP_STRING([--disable-bfd],
                             [Disable BFD support]),
              [ac_cv_enable_bfd=$enableval],
              [ac_cv_enable_bfd=yes])
AC_MSG_RESULT([$ac_cv_enable_bfd])

if test x"$ac_cv_enable_bfd" = "xyes"; then
  AC_CHECK_LIB(bfd, bfd_init)
fi

dnl **************************************************************
dnl for coverage
dnl **************************************************************
AC_CHECK_COVERAGE

dnl **************************************************************
dnl Configure for maintainer mode.
dnl **************************************************************

AM_MAINTAINER_MODE
AM_CONDITIONAL([MAINTAINER_MODE],
               [test "x$USE_MAINTAINER_MODE" = "xyes"])

dnl **************************************************************
dnl Configure for modules.
dnl **************************************************************

moduledir="\${libdir}/\$(PACKAGE)/module"
AC_SUBST(moduledir)

dnl **************************************************************
dnl Configure for UI modules.
dnl **************************************************************

ui_moduledir="\${moduledir}/ui"
AC_SUBST(ui_moduledir)

dnl **************************************************************
dnl Configure for ui factory modules.
dnl **************************************************************

ui_factory_moduledir="\${moduledir}/factory/ui"
AC_SUBST(ui_factory_moduledir)

dnl **************************************************************
dnl Configure for Report modules.
dnl **************************************************************

report_moduledir="\${moduledir}/report"
AC_SUBST(report_moduledir)

report_factory_moduledir="\${moduledir}/factory/report"
AC_SUBST(report_factory_moduledir)

dnl **************************************************************
dnl Configure for Streamer modules.
dnl **************************************************************

streamer_moduledir="\${moduledir}/streamer"
AC_SUBST(streamer_moduledir)

streamer_factory_moduledir="\${moduledir}/factory/streamer"
AC_SUBST(streamer_factory_moduledir)

dnl **************************************************************
dnl Configure for factory modules.
dnl **************************************************************

factory_moduledir="\${moduledir}/factory"
AC_SUBST(factory_moduledir)

dnl **************************************************************
dnl Check for GTK+
dnl **************************************************************
AM_PATH_GTK_2_0(2.12.0, [], [], [])

if test -z "$no_gtk"; then
  AC_DEFINE(HAVE_GTK, 1, [Define to 1 if you have GTK+ installed])
fi
AM_CONDITIONAL([HAVE_GTK], [test -z "$no_gtk"])

dnl **************************************************************
dnl Check for GStreamer
dnl **************************************************************
AC_MSG_CHECKING([whether to try to use GStreamer])
AC_ARG_ENABLE([gstreamer],
              AS_HELP_STRING([--disable-gstreamer],
                             [Disable BFD support]),
              [ac_cv_enable_gstreamer=$enableval],
              [ac_cv_enable_gstreamer=yes])
AC_MSG_RESULT([$ac_cv_enable_gstreamer])

gstreamer_exists=no
if test "$ac_cv_enable_gstreamer" = "yes"; then
  PKG_CHECK_MODULES(GSTREAMER,
                    gstreamer-0.10 >= 0.10.0 gstreamer-base-0.10 >= 0.10.0,
		    [gstreamer_exists=yes], [gstreamer_exists=no])
  AC_SUBST([GSTREAMER_CFLAGS])
  AC_SUBST([GSTREAMER_LIBS])
  if test x"$gstreamer_exists" = "xyes"; then
    AC_DEFINE(HAVE_GSTREAMER, 1, [Define to 1 if you have gstreamer installed])
    AC_MSG_CHECKING([where is GStreamer plugin dir])
    AC_ARG_WITH([gst-pluginsdir],
		AS_HELP_STRING([--with-gst-pluginsdir=DIR],
			       [Use DIR for GStreamer plugin directory
			       (default: auto)]),
		[gst_pluginsdir="$withval"],
		[gst_pluginsdir="auto"])
    if test "$gst_pluginsdir" = "auto"; then
      gst_pluginsdir=`pkg-config --variable=pluginsdir gstreamer-0.10`
    fi
    AC_MSG_RESULT([$gst_pluginsdir])
    AC_SUBST(gst_pluginsdir)
  fi
fi
AM_CONDITIONAL([HAVE_GSTREAMER], [test $gstreamer_exists = "yes"])

dnl **************************************************************
dnl Check for Cairo
dnl **************************************************************
cairo_exists=no
PKG_CHECK_MODULES(CAIRO, cairo >= 1.4,
		  [cairo_exists=yes], [cairo_exists=no])
AC_SUBST([CAIRO_CFLAGS])
AC_SUBST([CAIRO_LIBS])
if test x"$cairo_exists" = "xyes"; then
  AC_DEFINE(HAVE_CAIRO, 1, [Define to 1 if you have cairo installed])
fi
AM_CONDITIONAL([HAVE_CAIRO], [test $cairo_exists = "yes"])

dnl **************************************************************
dnl Check for Cairo PDF backend
dnl **************************************************************
cairo_pdf_exists=no
PKG_CHECK_MODULES(CAIRO_PDF, cairo-pdf,
		  [cairo_pdf_exists=yes], [cairo_pdf_exists=no])
AC_SUBST([CAIRO_PDF_CFLAGS])
AC_SUBST([CAIRO_PDF_LIBS])
if test x"$cairo_pdf_exists" = "xyes"; then
  AC_DEFINE(HAVE_CAIRO_PDF, 1, [Define to 1 if you have cairo PDF backend installed])
fi
AM_CONDITIONAL([HAVE_CAIRO_PDF], [test $cairo_pdf_exists = "yes"])

dnl **************************************************************
dnl Check for Pango cairo support
dnl **************************************************************
pango_cairo_exists=no
PKG_CHECK_MODULES(PANGO_CAIRO, pangocairo,
		  [pango_cairo_exists=yes], [pango_cairo_exists=no])
AC_SUBST([PANGO_CAIRO_CFLAGS])
AC_SUBST([PANGO_CAIRO_LIBS])
if test x"$pango_cairo_exists" = "xyes"; then
  AC_DEFINE(HAVE_PANGO_CAIRO, 1, [Define to 1 if you have Pango cairo supprot installed])
fi
AM_CONDITIONAL([HAVE_PANGO_CAIRO], [test $pango_cairo_exists = "yes"])

dnl **************************************************************
dnl Check for PDF report
dnl **************************************************************
AM_CONDITIONAL([PDF_REPORT_AVAILABLE],
               [test $pango_cairo_exists = "yes" -a $cairo_pdf_exists = "yes"])

dnl **************************************************************
dnl Checks for gtk-doc
dnl **************************************************************

GTK_DOC_CHECK([1.8])

dnl **************************************************************
dnl Check for gettext.
dnl **************************************************************

GETTEXT_PACKAGE=cutter
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext package])

ALL_LINGUAS="ja"
AM_GLIB_GNU_GETTEXT

dnl **************************************************************
dnl Check for SF.net.
dnl **************************************************************

SF_PROJECT_NAME=Cutter
AC_SUBST(SF_PROJECT_NAME)

AC_ARG_WITH([sf-user],
            AS_HELP_STRING([--with-sf-user=NAME],
                           [Use NAME to access sf.net]),
            [SF_USER="$withval"])
AC_SUBST(SF_USER)

SF_USER_OPTION=""
if test -n "$SF_USER"; then
   SF_USER_OPTION="--username $SF_USER"
fi
AC_SUBST(SF_USER_OPTION)

SF_HOST="shell.sourceforge.net"
if test -n "$SF_USER"; then
   SF_HOST="$SF_USER@$SF_HOST"
fi
AC_SUBST(SF_HOST)

SF_HTDOCS_PATH="/home/groups/c/cu/cutter/htdocs"
AC_SUBST(SF_HTDOCS_PATH)

SF_HTDOCS="$SF_HOST:$SF_HTDOCS_PATH"
AC_SUBST(SF_HTDOCS)

SF_REPOS="https://cutter.svn.sourceforge.net/svnroot/cutter/cutter"
AC_SUBST(SF_REPOS)

CUTTER="\$(top_builddir)/cutter/cutter\$(EXEEXT)"
AC_SUBST(CUTTER)

CUTTER_CFLAGS="$GLIB_CFLAGS -DG_DISABLE_DEPRECATED -DUI_MODULEDIR=\\\"\"\$(ui_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DUI_FACTORY_MODULEDIR=\\\"\"\$(ui_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DREPORT_MODULEDIR=\\\"\"\$(report_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DREPORT_FACTORY_MODULEDIR=\\\"\"\$(report_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DSTREAMER_MODULEDIR=\\\"\"\$(streamer_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DSTREAMER_FACTORY_MODULEDIR=\\\"\"\$(streamer_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DFACTORY_MODULEDIR=\\\"\"\$(factory_moduledir)\"\\\""
AC_SUBST(CUTTER_CFLAGS)

AC_CONFIG_FILES([Makefile
		 cutter.pc
		 gcutter.pc
		 libcutter.pc
		 m4macros/Makefile
		 cutter/Makefile
		 module/Makefile
		 module/ui/Makefile
		 module/report/Makefile
		 module/streamer/Makefile
		 gst-plugins/Makefile
		 gst-plugins/test/Makefile
		 sample/Makefile
		 sample/glib/Makefile
		 sample/gtk+/Makefile
		 sample/cairo/Makefile
                 test/Makefile
		 test/loader_test_dir/Makefile
		 test/loader_suite_test_dir/Makefile
		 test/repository_test_dir/Makefile
		 test/attribute_test_dir/Makefile
		 test/module_test_dir/Makefile
		 test/factory_test_dir/Makefile
		 test/factory_test_dir/type1/Makefile
		 test/factory_test_dir/type2/Makefile
		 test/pipeline_test_dir/Makefile
		 test/pipeline_test_dir/error_test/Makefile
		 test/pipeline_test_dir/success_test/Makefile
		 po/Makefile.in
		 misc/Makefile
		 doc/Makefile
		 doc/reference/version.xml
		 doc/reference/Makefile
		 html/Makefile])

AC_OUTPUT
