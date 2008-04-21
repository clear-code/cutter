#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT(cutter, 0.7.0, kou@cozmixng.org)
AC_CONFIG_AUX_DIR([config])

AC_CONFIG_SRCDIR([cutter/cut-main.c])
AM_CONFIG_HEADER([cutter/config.h])

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

PACKAGE_TITLE=Cutter
AC_SUBST(PACKAGE_TITLE)

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
fi

dnl **************************************************************
dnl Check for GLib.
dnl **************************************************************
GLIB_REQUIRED=2.14.0
AC_SUBST(GLIB_REQUIRED)
AM_PATH_GLIB_2_0($GLIB_REQUIRED,
                 [],
                 [AC_MSG_ERROR([GLib >= $GLIB_REQUIRED required.])],
                 [gmodule gobject gthread])

# Checks for libraries.
AC_ARG_ENABLE([bfd],
              AS_HELP_STRING([--disable-bfd],
                             [Disable BFD support]),
              [ac_cv_enable_bfd=$enableval],
              [ac_cv_enable_bfd=yes])
AC_CACHE_CHECK([whether to try to use BFD],
               [ac_cv_enable_bfd],
               [ac_cv_enable_bfd=yes])

if test x"$ac_cv_enable_bfd" = "xyes"; then
  AC_CHECK_LIB(bfd, bfd_init)
fi

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
dnl Configure for factory modules.
dnl **************************************************************

factory_moduledir="\${moduledir}/factory"
AC_SUBST(factory_moduledir)

dnl **************************************************************
dnl Check for GTK+
dnl **************************************************************
AM_PATH_GTK_2_0(2.12.0, [], [], [])
PKG_CHECK_MODULES(GTK_PRINT, gtk+-unix-print-2.0,
                  [gtk_print=yes], [gtk_print=no])
AM_CONDITIONAL([HAVE_GTK], [test "x$no_gtk" = x -a "x$gtk_print" = xyes])

dnl **************************************************************
dnl Check for Cairo
dnl **************************************************************
cairo_exists=no
PKG_CHECK_MODULES(CAIRO, cairo,
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

SF_REPOS="https://cutter.svn.sourceforge.net/svnroot/cutter"
AC_SUBST(SF_REPOS)

CUTTER="\$(top_builddir)/cutter/cutter\$(EXEEXT)"
AC_SUBST(CUTTER)

CUTTER_CFLAGS="$GLIB_CFLAGS -DUI_MODULEDIR=\\\"\"\$(ui_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DUI_FACTORY_MODULEDIR=\\\"\"\$(ui_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DREPORT_MODULEDIR=\\\"\"\$(report_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DREPORT_FACTORY_MODULEDIR=\\\"\"\$(report_factory_moduledir)\"\\\""
CUTTER_CFLAGS="$CUTTER_CFLAGS -DFACTORY_MODULEDIR=\\\"\"\$(factory_moduledir)\"\\\""
AC_SUBST(CUTTER_CFLAGS)

AC_CONFIG_FILES([Makefile
		 cutter.pc
		 gcutter.pc
		 libcutter.pc
		 cutter/Makefile
		 cutter/module/Makefile
		 cutter/module/ui/Makefile
		 cutter/module/report/Makefile
		 sample/Makefile
		 sample/glib/Makefile
		 sample/gtk+/Makefile
		 sample/cairo/Makefile
                 test/Makefile
		 test/loader_test_dir/Makefile
		 test/repository_test_dir/Makefile
		 test/attribute_test_dir/Makefile
		 test/module_test_dir/Makefile
		 test/factory_test_dir/Makefile
		 test/factory_test_dir/type1/Makefile
		 test/factory_test_dir/type2/Makefile
		 po/Makefile.in
		 misc/Makefile
		 doc/Makefile
		 doc/reference/version.xml
		 doc/reference/Makefile
		 html/Makefile])

AC_OUTPUT
