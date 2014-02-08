#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

m4_define([cutter_version_major], [1])
m4_define([cutter_version_minor], [2])
m4_define([cutter_version_micro], [4])
m4_define([cutter_version],
          [cutter_version_major.cutter_version_minor.cutter_version_micro])

AC_INIT(cutter, [cutter_version], kou@clear-code.com)
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([acmacros])

AC_CONFIG_SRCDIR([cutter/cut-main.c])
AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

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
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# For debug
AC_ARG_ENABLE(debug,
  [AS_HELP_STRING([--enable-debug],
                  [use debug flags (default=no)])],
  [cutter_debug="$enableval"],
  [cutter_debug="no"])
if test "x$cutter_debug" != "xno"; then
  cutter_debug="yes"
  if test "$CLANG" = "yes"; then
    CFLAGS="$CFLAGS -O0 -g"
    CXXFLAGS="$CXXFLAGS -O0 -g"
  elif test "$GCC" = "yes"; then
    CFLAGS="$CFLAGS -O0 -g3"
    CXXFLAGS="$CXXFLAGS -O0 -g3"
  fi
fi

AC_DEFUN([CHECK_CFLAG], [
  AC_MSG_CHECKING([if gcc supports $1])
  old_CFLAGS=$CFLAGS
  flag=`echo '$1' | sed -e 's,^-Wno-,-W,'`
  CFLAGS="$CFLAGS $flag -Werror"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [check_cflag=yes],
    [check_cflag=no])
  CFLAGS="$old_CFLAGS"
  if test "x$check_cflag" = "xyes"; then
    CFLAGS="$CFLAGS $1"
  fi
  AC_MSG_RESULT([$check_cflag])
])

AC_DEFUN([CHECK_CXXFLAG], [
  AC_MSG_CHECKING([if g++ supports $1])
  old_CXXFLAGS=$CXXFLAGS
  flag=`echo '$1' | sed -e 's,^-Wno-,-W,'`
  CXXFLAGS="$CXXFLAGS $flag -Werror"
  AC_LANG_PUSH([C++])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [check_cxxflag=yes],
    [check_cxxflag=no])
  AC_LANG_POP([C++])
  CXXFLAGS="$old_CXXFLAGS"
  if test "x$check_cxxflag" = "xyes"; then
    CXXFLAGS="$CXXFLAGS $1"
  fi
  AC_MSG_RESULT([$check_cxxflag])
])

AC_DEFUN([CHECK_BUILD_FLAG], [
  CHECK_CFLAG([$1])
  CHECK_CXXFLAG([$1])
])

CHECK_BUILD_FLAG([-Wall])
CHECK_BUILD_FLAG([-Wpointer-arith])
CHECK_BUILD_FLAG([-Wcast-align])
CHECK_CFLAG([-Wmissing-declarations])
CHECK_CFLAG([-Wmissing-prototypes])
CHECK_CFLAG([-fexceptions])

# Checks for headers
AC_HEADER_SYS_WAIT

AC_CHECK_HEADERS(io.h)
AC_CHECK_HEADERS(windows.h)
AC_CHECK_HEADERS(mach-o/loader.h)
AC_CHECK_HEADERS(elf.h)
AC_CHECK_HEADERS(stdint.h, [have_stdint_h=yes], [have_stdint_h=no])
AC_CHECK_HEADERS(inttypes.h, [have_inttypes_h=yes], [have_inttypes_h=no])
AC_CHECK_HEADERS(winsock2.h, [have_winsock2_h=yes], [have_winsock2_h=no])
AC_CHECK_HEADERS(sys/socket.h, [have_sys_socket_h=yes], [have_sys_socket_h=no])
AC_CHECK_HEADERS(sys/un.h, [have_sys_un_h=yes], [have_sys_un_h=no])

if test "$have_stdint_h" = "yes" -o "$have_inttypes_h" = "yes"; then
   have_c99_stdint_types=yes
   AC_DEFINE(HAVE_C99_STDINT_TYPES, 1,
             [Define to 1 if you have <stdint.h> or <inttypes.h>])
else
   have_c99_stdint_types=no
fi

AC_CHECK_FUNCS([fabs], [], [AC_CHECK_LIB([m], [fabs])])

GPG_UID=m4_include(gpg_uid)
AC_SUBST(GPG_UID)

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

WINDOWS_LIBS=

if test "$have_winsock2_h" = "yes"; then
  WINDOWS_LIBS="-lws2_32"
fi
AC_SUBST(WINDOWS_LIBS)

dnl **************************************************************
dnl Check for GLib.
dnl **************************************************************
GLIB_REQUIRED=2.12.3
AC_SUBST(GLIB_REQUIRED)
AM_PATH_GLIB_2_0($GLIB_REQUIRED,
                 [],
                 [AC_MSG_ERROR([GLib >= $GLIB_REQUIRED required.])],
                 [gmodule gobject gthread])
if test "$glib_config_major_version.$glib_config_minor_version" = "2.12"; then
  glib_2_12=yes
else
  glib_2_12=no
fi
AM_CONDITIONAL([GLIB_2_12], [test "$glib_2_12" = "yes"])

AC_MSG_CHECKING([whether to enable deprecated GLib API])
AC_ARG_ENABLE([deprecated-glib-api],
              AS_HELP_STRING([--disable-deprecated-glib-api],
                             [Disable deprecated GLib API]),
              [enable_deprecated_glib_api=$enableval],
              [enable_deprecated_glib_api=yes])
AC_MSG_RESULT([$enable_deprecated_glib_api])

dnl **************************************************************
dnl Check for GIO
dnl **************************************************************
GIO_REQUIRED=2.16.0
if test "$os_win32" = "no"; then
  GIO_PACKAGE=gio-unix-2.0
else
  GIO_PACKAGE=gio-2.0
fi

PKG_CHECK_MODULES(GIO, $GIO_PACKAGE >= $GIO_REQUIRED,
                  [gio_exists=yes], [gio_exists=no])
AM_CONDITIONAL([HAVE_GIO], [test "$gio_exists" = "yes"])

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
              AS_HELP_STRING([--enable-bfd],
                             [Enable BFD detection]),
              [enable_bfd=$enableval],
              [enable_bfd=no])
AC_MSG_RESULT([$enable_bfd])

bfd_available=no
need_environ_declaration=no
BFD_LIBS=
if test x"$enable_bfd" = "xyes"; then
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
dnl Configure for factory modules.
dnl **************************************************************

factory_moduledir="\${moduledir}/factory"
AC_SUBST(factory_moduledir)

dnl **************************************************************
dnl Configure for UI modules.
dnl **************************************************************

ui_moduledir="\${moduledir}/ui"
AC_SUBST(ui_moduledir)

dnl **************************************************************
dnl Configure for UI factory modules.
dnl **************************************************************

ui_factory_moduledir="\${factory_moduledir}/ui"
AC_SUBST(ui_factory_moduledir)

dnl **************************************************************
dnl Configure for UI data.
dnl **************************************************************

ui_datadir="\${pkgdatadir}/ui"
AC_SUBST(ui_datadir)

dnl **************************************************************
dnl Configure for Report modules.
dnl **************************************************************

report_moduledir="\${moduledir}/report"
AC_SUBST(report_moduledir)

report_factory_moduledir="\${factory_moduledir}/report"
AC_SUBST(report_factory_moduledir)

dnl **************************************************************
dnl Configure for Stream modules.
dnl **************************************************************

stream_moduledir="\${moduledir}/stream"
AC_SUBST(stream_moduledir)

stream_factory_moduledir="\${factory_moduledir}/stream"
AC_SUBST(stream_factory_moduledir)

dnl **************************************************************
dnl Configure for LoaderCustomizer modules.
dnl **************************************************************

loader_customizer_moduledir="\${moduledir}/loader-customizer"
AC_SUBST(loader_customizer_moduledir)

loader_customizer_factory_moduledir="\${factory_moduledir}/loader-customizer"
AC_SUBST(loader_customizer_factory_moduledir)

dnl **************************************************************
dnl Check for GTK+
dnl **************************************************************
m4_ifdef([AM_PATH_GTK_2_0],
         [AM_PATH_GTK_2_0(2.12.0, [gtk_exists="yes"], [gtk_exists="no"], [])],
         [gtk_exists="no"])

if test "$gtk_exists" != "no"; then
  AC_DEFINE(HAVE_GTK, 1, [Define to 1 if you have GTK+ installed])

  AC_MSG_CHECKING(for GTK+ is built with debug mode)
  if test "$cross_compiling" = "yes"; then
    AC_MSG_RESULT(no (cross compiling; assumed no))
  else
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $GTK_CFLAGS"
    LIBS="$CFLAGS $GTK_LIBS"
    AC_RUN_IFELSE([AC_LANG_PROGRAM([
#include <stdlib.h>
#include <gtk/gtk.h>
                                   ],
      [
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
      ])],
      [AC_DEFINE(HAVE_GTK_ENABLE_DEBUG, 1,
		 [Define to 1 if you have GTK+ installed and built with debug mode])
       AC_MSG_RESULT(yes)],
      [AC_MSG_RESULT(no)])
    CFLAGS="$ac_save_CFLAGS"
    LIBS="$ac_save_LIBS"
  fi
fi
AM_CONDITIONAL([HAVE_GTK], [test "$gtk_exists" = "yes"])

dnl **************************************************************
dnl Check for GOffice
dnl **************************************************************
AC_MSG_CHECKING([whether to try to use GOffice])
AC_ARG_ENABLE([goffice],
              AS_HELP_STRING([--disable-goffice],
                             [Disable GOffice support]),
              [enable_goffice=$enableval],
              [enable_goffice=yes])
AC_MSG_RESULT([$enable_goffice])

goffice_exists=no
if test "$enable_goffice" = "yes"; then
  PKG_CHECK_MODULES(GOFFICE, libgoffice-0.8,
		    [goffice_exists=yes], [goffice_exists=no])
  AC_SUBST(GOFFICE_CFLAGS)
  AC_SUBST(GOFFICE_LIBS)
fi

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
                             [Disable GStreamer support]),
              [enable_gstreamer=$enableval],
              [enable_gstreamer=no]) dnl TODO: enable me again against GStreamer 1.0.0 or later
AC_MSG_RESULT([$enable_gstreamer])

gstreamer_exists=no
if test "x$enable_gstreamer" = "xyes"; then
  PKG_CHECK_MODULES(GSTREAMER,
                    gstreamer-1.0 >= 1.0.0 gstreamer-base-1.0 >= 1.0.0,
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
      gst_pluginsdir=`pkg-config --variable=pluginsdir gstreamer-1.0`
    fi
    AC_MSG_RESULT([$gst_pluginsdir])
    AC_SUBST(gst_pluginsdir)
  fi
fi
AM_CONDITIONAL([HAVE_GSTREAMER], [test $gstreamer_exists = "yes"])

dnl **************************************************************
dnl Check for libsoup
dnl **************************************************************
AC_MSG_CHECKING([whether to try to use libsoup])
AC_ARG_ENABLE([libsoup],
              AS_HELP_STRING([--disable-libsoup],
                             [Disable libsoup support]),
              [enable_libsoup=$enableval],
              [enable_libsoup=yes])
AC_MSG_RESULT([$enable_libsoup])

libsoup_exists=no
if test "x$enable_libsoup" = "xyes"; then
  PKG_CHECK_MODULES(LIBSOUP,
                    libsoup-2.4,
		    [libsoup_exists=yes],
                    [libsoup_exists=no])
  AC_SUBST([LIBSOUP_CFLAGS])
  AC_SUBST([LIBSOUP_LIBS])
  if test "$libsoup_exists" = "yes"; then
    AC_DEFINE(HAVE_LIBSOUP, 1, [Define to 1 if you have libsoup installed])
  fi
fi
AM_CONDITIONAL([HAVE_LIBSOUP], [test $libsoup_exists = "yes"])

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

m4_ifdef([GTK_DOC_CHECK],
         [GTK_DOC_CHECK([1.8])],
         [enable_gtk_doc=no
          AM_CONDITIONAL([GTK_DOC_USE_LIBTOOL], [false])
          AM_CONDITIONAL([ENABLE_GTK_DOC], [false])])

dnl **************************************************************
dnl Checks for Ruby
dnl **************************************************************
ruby_available="no"
AC_ARG_WITH([ruby],
            AS_HELP_STRING([--with-ruby=PATH],
                           [Ruby interpreter path (default: auto-detect)]),
            [RUBY="$withval"])

if test "$RUBY" = "no"; then
  : # ignore
elif test "$RUBY" = ""; then
  AC_PATH_PROG(RUBY, ruby, ruby-not-found)
else
  AC_CHECK_FILE([$RUBY],
                [ruby_available="yes"],
                [AC_MSG_ERROR([$RUBY is not found.])])
fi

dnl **************************************************************
dnl Checks for RD2
dnl **************************************************************
rd2_available="no"
AC_ARG_WITH([rd2],
            AS_HELP_STRING([--with-rd2=PATH],
                           [rd2 command path. (default: auto-detect)]),
            [RD2="$withval"])
if test "$RD2" = "no"; then
  : # ignore
elif test "$RD2" = ""; then
  AC_PATH_PROG(RD2, rd2, rd2-not-found)
  if test x"$RD2" != x"rd2-not-found"; then
    rd2_available=yes
  fi
else
  AC_CHECK_FILE([$RD2],
                [rd2_available="yes"],
                [AC_MSG_ERROR([$RD2 is not found.])])
fi

if test "$rd2_available" = "yes" -o -f "$srcdir/doc/man-build.stamp"; then
  man_available=yes
else
  man_available=no
fi
AM_CONDITIONAL([MAN_AVAILABLE], [test "$man_available" = "yes"])

dnl **************************************************************
dnl Check for gettext.
dnl **************************************************************

AC_PROG_INTLTOOL([0.35.0])

AM_GLIB_GNU_GETTEXT

GETTEXT_PACKAGE=cutter
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext package])

dnl **************************************************************
dnl Check for fallback browser.
dnl **************************************************************

AC_ARG_WITH([fallback-browser],
            AS_HELP_STRING([--with-fallback-browser=BROWSER],
                           [Use BRWOSER for fallback browser to show URL
                            (default: auto)]),
            [FALLBACK_BROWSER="$withval"],
            [FALLBACK_BROWSER="yes"])

if test x"$FALLBACK_BROWSER" = x"no"; then
  FALLBACK_BROWSER=
elif test x"$FALLBACK_BROWSER" = x"yes"; then
  if test "$os_win32" = "yes"; then
    FALLBACK_BROWSER="explorer"
  else
    AC_PATH_PROGS(FALLBACK_BROWSER,
                  [xdg-open www-browser firefox iceweasel chrome chromium],
                  [])
  fi
else
  if test "$cross_compiling" = "yes"; then
    ac_cv_fallback_browser_available="yes"
  else
    AC_CHECK_FILE([$FALLBACK_BROWSER],
                  [],
                  [AC_MSG_WARN([$FALLBACK_BROWSER is not found.])
                   FALLBACK_BROWSER=])
  fi
fi
AC_SUBST(FALLBACK_BROWSER)

if test x"$FALLBACK_BROWSER" != x""; then
  AC_DEFINE_UNQUOTED(CUT_FALLBACK_BROWSER, "$FALLBACK_BROWSER",
                     [Fallback browser])
fi

dnl **************************************************************
dnl Check for SF.net.
dnl **************************************************************

SF_PROJECT_ID=cutter
AC_SUBST(SF_PROJECT_ID)
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

SF_WEB_HOST="web.sourceforge.net"
SF_FRS_HOST="frs.sourceforge.net"
if test -n "$SF_USER"; then
   SF_WEB_HOST="$SF_USER,$SF_PROJECT_ID@$SF_WEB_HOST"
   SF_FRS_HOST="$SF_USER,$SF_PROJECT_ID@$SF_FRS_HOST"
fi
AC_SUBST(SF_WEB_HOST)
AC_SUBST(SF_FRS_HOST)

SF_PROJECT_ID_HEAD_1=`echo $SF_PROJECT_ID | sed -e 's/^\(.\).*/\1/'`
SF_PROJECT_ID_HEAD_2=`echo $SF_PROJECT_ID | sed -e 's/^\(..\).*/\1/'`
SF_HTDOCS_PATH="/home/groups/${SF_PROJECT_ID_HEAD_1}/${SF_PROJECT_ID_HEAD_2}/${SF_PROJECT_ID}/htdocs"
AC_SUBST(SF_HTDOCS_PATH)
SF_HTDOCS="$SF_WEB_HOST:$SF_HTDOCS_PATH"
AC_SUBST(SF_HTDOCS)

SF_FRS_PATH="/home/frs/project/${SF_PROJECT_ID_HEAD_1}/${SF_PROJECT_ID_HEAD_2}/${SF_PROJECT_ID}"
AC_SUBST(SF_FRS_PATH)
SF_FRS="$SF_FRS_HOST:$SF_FRS_PATH"
AC_SUBST(SF_FRS)

dnl **************************************************************
dnl Cutter related variables.
dnl **************************************************************

icons_dir="\$(pkgdatadir)/icons"
AC_SUBST(icons_dir)

CUTTER="\$(top_builddir)/cutter/cutter\$(EXEEXT)"
AC_SUBST(CUTTER)

CUTTER_CFLAGS="$GLIB_CFLAGS "
if test "$enable_deprecated_glib_api" != "yes"; then
  CUTTER_CFLAGS="$CUTTER_CFLAGS -DG_DISABLE_DEPRECATED"
fi
CUTTER_CFLAGS="$CUTTER_CFLAGS $GIO_CFLAGS"
CUTTER_CFLAGS="$CUTTER_CFLAGS -DUI_MODULE_DIR=\\\"\"\$(ui_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DUI_FACTORY_MODULE_DIR=\\\"\"\$(ui_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DUI_DATA_DIR=\\\"\"\$(ui_datadir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DREPORT_MODULE_DIR=\\\"\"\$(report_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DREPORT_FACTORY_MODULE_DIR=\\\"\"\$(report_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DSTREAM_MODULE_DIR=\\\"\"\$(stream_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DSTREAM_FACTORY_MODULE_DIR=\\\"\"\$(stream_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DLOADER_CUSTOMIZER_MODULE_DIR=\\\"\"\$(loader_customizer_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DLOADER_CUSTOMIZER_FACTORY_MODULE_DIR=\\\"\"\$(loader_customizer_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DFACTORY_MODULE_DIR=\\\"\"\$(factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DICONS_DIR=\\\"\"\$(iconsdir)\"\\\""
AC_SUBST(CUTTER_CFLAGS)

dnl **************************************************************
dnl cutter/cut-config.h
dnl **************************************************************

AC_CONFIG_COMMANDS([cutter/cut-config.h-start],
[
  cat > cutter/cut-config.h <<\_______EOF
/* cut-config.h
 *
 * This is a generated file.  Please modify 'configure.ac'
 */

#ifndef __CUT_CONFIG_H__
#define __CUT_CONFIG_H__

_______EOF
])

if test "$have_stdint_h" = "yes"; then
AC_CONFIG_COMMANDS([cutter/cut-config.h-have-stdint.h],
[
    echo "#define CUT_HAVE_STDINT_H 1" >> cutter/cut-config.h
])
else
AC_CONFIG_COMMANDS([cutter/cut-config.h-not-have-stdint.h],
[
    echo "/* #undef CUT_HAVE_STDINT_H */" >> cutter/cut-config.h
])
fi

if test "$have_inttypes_h" = "yes"; then
AC_CONFIG_COMMANDS([cutter/cut-config.h-have-inttypes.h],
[
    echo "#define CUT_HAVE_INTTYPES_H 1" >> cutter/cut-config.h
])
else
AC_CONFIG_COMMANDS([cutter/cut-config.h-not-have-inttypes.h],
[
    echo "/* #undef CUT_HAVE_INTTYPES_H */" >> cutter/cut-config.h
])
fi

if test "$have_winsock2_h" = "yes"; then
AC_CONFIG_COMMANDS([cutter/cut-config.h-have-winsock2.h],
[
    echo "#define CUT_HAVE_WINSOCK2_H 1" >> cutter/cut-config.h
])
else
AC_CONFIG_COMMANDS([cutter/cut-config.h-not-have-winsock2.h],
[
    echo "/* #undef CUT_HAVE_WINSOCK2_H */" >> cutter/cut-config.h
])
fi

if test "$have_sys_socket_h" = "yes"; then
AC_CONFIG_COMMANDS([cutter/cut-config.h-have-sys-socket.h],
[
    echo "#define CUT_HAVE_SYS_SOCKET_H 1" >> cutter/cut-config.h
])
else
AC_CONFIG_COMMANDS([cutter/cut-config.h-not-have-sys-socket.h],
[
    echo "/* #socketdef CUT_HAVE_SYS_SOCKET_H */" >> cutter/cut-config.h
])
fi

if test "$have_sys_un_h" = "yes"; then
AC_CONFIG_COMMANDS([cutter/cut-config.h-have-sys-un.h],
[
    echo "#define CUT_HAVE_SYS_UN_H 1" >> cutter/cut-config.h
])
else
AC_CONFIG_COMMANDS([cutter/cut-config.h-not-have-sys-un.h],
[
    echo "/* #undef CUT_HAVE_SYS_UN_H */" >> cutter/cut-config.h
])
fi

AC_CONFIG_COMMANDS([cutter/cut-config.h-end],
[
  cat >> cutter/cut-config.h <<\_______EOF

#endif
_______EOF
])

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

/**
 * SECTION: cut-features
 * @title: Available features
 * @short_description: Available features in the installed Cutter.
 *
 * There are some macros to check a feature is available in the
 * installed Cutter.
 */

/**
 * CUT_SUPPORT_GLIB:
 *
 * Shows GLib support is available. That is, we can use &lt;gcutter.h&gt;.
 * It is always defined.
 */
#define CUT_SUPPORT_GLIB 1
_______EOF
])

AC_CONFIG_COMMANDS([cutter/cut-features.h-gdk-pixbuf-header],
[
  cat <<\_______EOF >> cutter/cut-features.h

/**
 * CUT_SUPPORT_GDK_PIXBUF:
 *
 * Shows GdkPixbuf support is available.
 * That is, we can use &lt;gdkcutter-pixbuf.h&gt;.
 */
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

AC_CONFIG_COMMANDS([cutter/cut-features.h-gio-header],
[
  cat <<\_______EOF >> cutter/cut-features.h

/**
 * CUT_SUPPORT_GIO:
 *
 * Shows GIO support is available.
 */
_______EOF
])
  if test "$gio_exists" = "yes"; then
AC_CONFIG_COMMANDS([cutter/cut-features.h-gio-enable],
[
    echo "#define CUT_SUPPORT_GIO 1" >> cutter/cut-features.h
])
  else
AC_CONFIG_COMMANDS([cutter/cut-features.h-gio-disable],
[
    echo "/* #undef CUT_SUPPORT_GIO */" >> cutter/cut-features.h
])
  fi

AC_CONFIG_COMMANDS([cutter/cut-features.h-libsoup-header],
[
  cat <<\_______EOF >> cutter/cut-features.h

/**
 * CUT_SUPPORT_LIBSOUP:
 *
 * Shows LibSoup support is available.
 */
_______EOF
])
  if test "$libsoup_exists" = "yes"; then
AC_CONFIG_COMMANDS([cutter/cut-features.h-libsoup-enable],
[
    echo "#define CUT_SUPPORT_LIBSOUP 1" >> cutter/cut-features.h
])
  else
AC_CONFIG_COMMANDS([cutter/cut-features.h-libsoup-disable],
[
    echo "/* #undef CUT_SUPPORT_LIBSOUP */" >> cutter/cut-features.h
])
  fi

AC_CONFIG_COMMANDS([cutter/cut-features.h-have-c99-stdint-types-header],
[
  cat <<\_______EOF >> cutter/cut-features.h

/**
 * CUT_SUPPORT_C99_STDINT_TYPES:
 *
 * Shows C99 stdint types support is available.
 * That is, we can use assertions that use C99 stdint types.
 * e.g. cut_assert_equal_int_least8().
 */
_______EOF
])
  if test "$have_c99_stdint_types" = "yes"; then
AC_CONFIG_COMMANDS([cutter/cut-features.h-have-c99-stdint-types],
[
    echo "#define CUT_SUPPORT_C99_STDINT_TYPES 1" >> cutter/cut-features.h
])
  else
AC_CONFIG_COMMANDS([cutter/cut-features.h-not-have-c99-stdint-types],
[
    echo "/* #undef CUT_SUPPORT_C99_STDINT_TYPES */" >> cutter/cut-features.h
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
		 cppcutter.pc
		 libcutter.pc
		 gdkcutter-pixbuf.pc
		 soupcutter.pc
		 m4macros/Makefile
		 glib-compatible/Makefile
		 glib-compatible/pcre/Makefile
		 gcutter/Makefile
		 gdkcutter-pixbuf/Makefile
		 soupcutter/Makefile
		 cutter/cut-version.h
		 cutter/Makefile
		 cppcutter/Makefile
		 module/Makefile
		 module/ui/Makefile
		 module/report/Makefile
		 module/stream/Makefile
		 module/loader-customizer/Makefile
		 gst-plugins/Makefile
		 gst-plugins/test/Makefile
		 data/Makefile
		 data/icons/Makefile
		 data/icons/kinotan/Makefile
		 sample/Makefile
		 sample/glib/Makefile
		 sample/gtk+/Makefile
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
		 test/fixtures/loader/cpp/Makefile
		 test/fixtures/loader/cpp-fixture/Makefile
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
		 test/fixtures/test-directory/Makefile
		 test/fixtures/thread/Makefile
		 test/fixtures/thread/failure/Makefile
		 test/lib/Makefile
		 test/lib/cuttest-echo/Makefile
		 test/cutter/Makefile
		 test/gcutter/Makefile
		 test/cppcutter/Makefile
		 test/gdkcutter-pixbuf/Makefile
		 test/soupcutter/Makefile
		 po/Makefile.in
		 misc/Makefile
		 license/Makefile
		 doc/Makefile
		 doc/reference/version.xml
		 doc/reference/Makefile
		 html/Makefile
		 vcc/Makefile
		 packages/Makefile
		 packages/rpm/Makefile
		 packages/rpm/centos/Makefile
		 packages/rpm/centos/cutter.spec
		 packages/apt/Makefile
		 packages/yum/Makefile])

AC_OUTPUT

echo
echo "Configure Result:"
echo
echo " Options:"
echo "  coverage                         : $cutter_enable_coverage"
echo "  coverage report by LCOV          : $cutter_enable_coverage_report_lcov"
echo
echo " Libraries:"
echo "  GLib <= 2.12                     : $glib_2_12"
echo "  GIO                              : $gio_exists"
echo "  GdkPixbuf                        : $gdk_pixbuf_exists"
echo "  GTK+                             : $gtk_exists"
echo "  GOffice                          : $goffice_exists"
echo "  GStreamer                        : $gstreamer_exists"
echo "  libsoup                          : $libsoup_exists"
echo "  cairo                            : $cairo_exists"
echo "  cairo-pdf                        : $cairo_pdf_exists"
echo "  pango-cairo                      : $pango_cairo_exists"
echo "  BFD                              : $bfd_available"
echo
echo " Paths:"
echo "  modules directory                : $moduledir"
echo "  UI modules directory             : $ui_moduledir"
echo "  UI factory modules directory     : $ui_factory_moduledir"
echo "  report modules directory         : $report_moduledir"
echo "  report factory modules directory : $report_factory_moduledir"
echo "  stream modules directory         : $stream_moduledir"
echo "  stream factory modules directory : $stream_factory_moduledir"
echo
echo "  GStreamer plugins directory      : $gst_pluginsdir"
echo
echo " Tools:"
echo "  GTK-Doc                          : $enable_gtk_doc"
echo "  Ruby                             : $RUBY"
echo "  rd2                              : $RD2"
if test "$cutter_enable_coverage" = "yes"; then
echo "  lcov                             : $LCOV"
echo "  genhtml                          : $GENHTML"
fi
