#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

m4_define([cutter_version_major], [1])
m4_define([cutter_version_minor], [0])
m4_define([cutter_version_micro], [6])
m4_define([cutter_version],
          [cutter_version_major.cutter_version_minor.cutter_version_micro])

AC_INIT(cutter, [cutter_version], kou@cozmixng.org)
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4macros])

AC_CONFIG_SRCDIR([cutter/cut-main.c])
AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

PACKAGE_TITLE=Cutter
AC_SUBST(PACKAGE_TITLE)

CUTTER_VERSION_MAJOR=cutter_version_major
CUTTER_VERSION_MINOR=cutter_version_minor
CUTTER_VERSION_MICRO=cutter_version_micro
CUTTER_VERSION=cutter_version
AC_SUBST(CUTTER_VERSION_MAJOR)
AC_SUBST(CUTTER_VERSION_MINOR)
AC_SUBST(CUTTER_VERSION_MICRO)
AC_SUBST(CUTTER_VERSION)

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

AC_CHECK_HEADER(mach-o/loader.h,
                [AC_DEFINE(HAVE_MACH_O_LOADER_H, 1,
                           [Define to 1 if you have <mach-o/loader.h>.])])

dnl **************************************************************
dnl Check for MinGW
dnl **************************************************************

AC_ARG_ENABLE([mingw-debug],
              AS_HELP_STRING([--enable-mingw-debug],
	                     [Enable MinGW debug]))
case $host_os in
mingw*)
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

dnl **************************************************************
dnl Check for GdkPixbuf
dnl **************************************************************
PKG_CHECK_MODULES(GDK_PIXBUF, gdk-pixbuf-2.0,
                  [gdk_pixbuf_exists=yes], [gdk_pixbuf_exists=no])
AC_SUBST(GDK_PIXBUF_CFLAGS)
AC_SUBST(GDK_PIXBUF_LIBS)

if test "$gdk_pixbuf_exists" = "yes"; then
  AC_DEFINE(HAVE_GDK_PIXBUF, 1, [Define to 1 if you have gdk-pixbuf installed])
fi
AM_CONDITIONAL([HAVE_GDK_PIXBUF], [test "$gdk_pixbuf_exists" = "yes"])

# Checks for libraries.
AC_MSG_CHECKING([whether to try to use BFD])
AC_ARG_ENABLE([bfd],
              AS_HELP_STRING([--disable-bfd],
                             [Disable BFD support]),
              [ac_cv_enable_bfd=$enableval],
              [ac_cv_enable_bfd=yes])
AC_MSG_RESULT([$ac_cv_enable_bfd])

bfd_available=no
need_environ_declaration=no
BFD_LIBS=
if test x"$ac_cv_enable_bfd" = "xyes"; then
  AC_CHECK_HEADERS([bfd.h], [have_bfd_h=yes], [have_bfd_h=no])

  if test "$have_bfd_h" = "yes"; then
    AC_CHECK_LIB(bfd, xstrerror, [have_libbfd_so=yes], [have_libbfd_so=no])
    if test "$have_libbfd_so" = "yes"; then
      BFD_LIBS="-lbfd"
    else
      original_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -shared"
      AC_CHECK_LIB(bfd, bfd_init, [have_libbfd=yes], [have_libbfd=no])
      AC_CHECK_LIB(iberty, xstrerror, [have_libiberty=yes], [have_libiberty=no])
      if test "$have_libbfd" = "yes" -a "$have_libiberty" = "yes"; then
        BFD_LIBS="-lbfd -liberty"
        AC_CHECK_DECL(environ, [], [need_environ_declaration=yes])
      fi
      LDFLAGS="$original_LDFLAGS"
    fi
  fi

  AC_MSG_CHECKING([whether BFD available])
  if test "$BFD_LIBS" != ""; then
    bfd_available=yes
    AC_DEFINE(HAVE_LIBBFD, 1, [Define to 1 if you have the BFD library.])
    if test "$need_environ_declaration" = "yes"; then
      AC_DEFINE(NEED_ENVIRON_DECLARATION, 1,
                [Define to 1 if your system needs environ variable declaration.])
    fi
  fi
  AC_MSG_RESULT([$bfd_available])
fi
AC_SUBST(BFD_LIBS)
AM_CONDITIONAL([HAVE_LIBBFD], [test "$bfd_available" = "yes"])

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
dnl Configure for Stream modules.
dnl **************************************************************

stream_moduledir="\${moduledir}/stream"
AC_SUBST(stream_moduledir)

stream_factory_moduledir="\${moduledir}/factory/stream"
AC_SUBST(stream_factory_moduledir)

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

  AC_MSG_CHECKING(for GTK+ is built with debug mode)
  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  CFLAGS="$CFLAGS $GTK_CFLAGS"
  LIBS="$CFLAGS $GTK_LIBS"
  AC_RUN_IFELSE([
#include <stdlib.h>
#include <gtk/gtk.h>

int
main (int argc, char **argv)
{
    GOptionGroup *option_group;
    GOptionContext *option_context;
    gchar *help;
    gboolean debug_mode = FALSE;

    option_group = gtk_get_option_group(FALSE);
    option_context = g_option_context_new(NULL);

    help = g_option_context_get_help(option_context, FALSE, option_group);

    if (help && g_regex_match_simple("--gtk-debug", help, 0, 0)) {
        debug_mode = TRUE;
    }
    g_option_context_free(option_context);

    return debug_mode ? EXIT_SUCCESS : EXIT_FAILURE;
}
    ],
    [AC_DEFINE(HAVE_GTK_ENABLE_DEBUG, 1,
               [Define to 1 if you have GTK+ installed and built with debug mode])
     AC_MSG_RESULT(yes)],
    [AC_MSG_RESULT(no)])
  CFLAGS="$ac_save_CFLAGS"
  LIBS="$ac_save_LIBS"
fi
AM_CONDITIONAL([HAVE_GTK], [test -z "$no_gtk"])

dnl **************************************************************
dnl Check for GOffice
dnl **************************************************************
PKG_CHECK_MODULES(GOFFICE, libgoffice-0.6,
                  [goffice_exists=yes], [goffice_exists=no])
AC_SUBST(GOFFICE_CFLAGS)
AC_SUBST(GOFFICE_LIBS)

if test "$goffice_exists" = "yes"; then
  AC_DEFINE(HAVE_GOFFICE, 1, [Define to 1 if you have goffice installed])
fi
AM_CONDITIONAL([HAVE_GOFFICE], [test "$goffice_exists" = "yes"])

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

AC_PROG_INTLTOOL([0.35.5])

AM_GLIB_GNU_GETTEXT

GETTEXT_PACKAGE=cutter
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext package])

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

dnl **************************************************************
dnl Cutter related variables.
dnl **************************************************************

CUTTER="\$(top_builddir)/cutter/cutter"
AC_SUBST(CUTTER)

CUTTER_CFLAGS="$GLIB_CFLAGS -DG_DISABLE_DEPRECATED -DUI_MODULEDIR=\\\"\"\$(ui_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DUI_FACTORY_MODULEDIR=\\\"\"\$(ui_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DREPORT_MODULEDIR=\\\"\"\$(report_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DREPORT_FACTORY_MODULEDIR=\\\"\"\$(report_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DSTREAM_MODULEDIR=\\\"\"\$(stream_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DSTREAM_FACTORY_MODULEDIR=\\\"\"\$(stream_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DFACTORY_MODULEDIR=\\\"\"\$(factory_moduledir)\"\\\""
AC_SUBST(CUTTER_CFLAGS)

dnl **************************************************************
dnl cutter/cut-features.h
dnl **************************************************************

AC_CONFIG_COMMANDS([cutter/cut-features.h-start],
[
  cat > cutter/cut-features.h <<\_______EOF
/* cut-features.h
 *
 * This is a generated file.  Please modify 'configure.ac'
 */

#ifndef __CUT_FEATURES_H__
#define __CUT_FEATURES_H__

#define CUT_SUPPORT_GLIB 1
_______EOF
])

  if test "$gdk_pixbuf_exists" = "yes"; then
AC_CONFIG_COMMANDS([cutter/cut-features.h-gdk-pixbuf-enable],
[
    echo "#define CUT_SUPPORT_GDK_PIXBUF 1" >> cutter/cut-features.h
])
  else
AC_CONFIG_COMMANDS([cutter/cut-features.h-gdk-pixbuf-disable],
[
    echo "/* #undef CUT_SUPPORT_GDK_PIXBUF */" >> cutter/cut-features.h
])
  fi

AC_CONFIG_COMMANDS([cutter/cut-features.h-end],
[
  cat >> cutter/cut-features.h <<\_______EOF

#endif
_______EOF
])

AC_CONFIG_FILES([Makefile
		 cutter.pc
		 gcutter.pc
		 libcutter.pc
		 gdkcutter-pixbuf.pc
		 m4macros/Makefile
		 gcutter/Makefile
		 gdkcutter-pixbuf/Makefile
		 cutter/cut-version.h
		 cutter/Makefile
		 module/Makefile
		 module/ui/Makefile
		 module/report/Makefile
		 module/stream/Makefile
		 gst-plugins/Makefile
		 gst-plugins/test/Makefile
		 sample/Makefile
		 sample/glib/Makefile
		 sample/gtk+/Makefile
		 sample/cairo/Makefile
                 test/Makefile
		 test/fixtures/Makefile
		 test/fixtures/assertions/Makefile
		 test/fixtures/assertions/sub/Makefile
		 test/fixtures/data/Makefile
		 test/fixtures/factory/Makefile
		 test/fixtures/factory/type1/Makefile
		 test/fixtures/factory/type2/Makefile
		 test/fixtures/loader/Makefile
		 test/fixtures/loader/attribute/Makefile
		 test/fixtures/loader/fixture/Makefile
		 test/fixtures/loader/iterator/Makefile
		 test/fixtures/loader/module/Makefile
		 test/fixtures/loader/suite/Makefile
		 test/fixtures/loader/test/Makefile
		 test/fixtures/pipeline/Makefile
		 test/fixtures/pipeline/error/Makefile
		 test/fixtures/pipeline/failure/Makefile
		 test/fixtures/pipeline/omission/Makefile
		 test/fixtures/pipeline/pending/Makefile
		 test/fixtures/pipeline/success/Makefile
		 test/fixtures/pipeline/setup-error/Makefile
		 test/fixtures/sub-process/Makefile
		 test/fixtures/sub-process/fixtures/Makefile
		 test/fixtures/sub-process/fixtures/normal/Makefile
		 test/fixtures/sub-process-group/Makefile
		 test/fixtures/sub-process-group/fixtures/Makefile
		 test/fixtures/sub-process-group/fixtures/normal/Makefile
		 test/fixtures/repository/Makefile
		 test/fixtures/xml-stream/Makefile
		 test/fixtures/file-stream-reader/Makefile
		 test/fixtures/pixbuf/Makefile
		 test/fixtures/path/Makefile
		 test/fixtures/path/parent/Makefile
		 test/fixtures/path/parent/sub/Makefile
		 test/fixtures/test-context/Makefile
		 test/lib/Makefile
		 test/cutter/Makefile
		 test/gcutter/Makefile
		 test/gdkcutter-pixbuf/Makefile
		 po/Makefile.in
		 misc/Makefile
		 doc/Makefile
		 doc/reference/version.xml
		 doc/reference/Makefile
		 html/Makefile])

AC_OUTPUT
