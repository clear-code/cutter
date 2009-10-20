AC_DEFUN([AC_CHECK_COVERAGE],
[
  dnl **************************************************************
  dnl Configure for coverage.
  dnl **************************************************************

  AC_ARG_ENABLE([coverage],
                AS_HELP_STRING([--enable-coverage],
                               [Enable coverage]),
                [ac_cv_enable_coverage=$enableval],
                [ac_cv_enable_coverage=no])
  if test "x$GCC" != "xyes"; then
    ac_cv_enable_coverage=no
  fi

  cutter_has_lcov=no
  ltp_version_list="1.6 1.7"
  AC_CHECK_PROG(LCOV, lcov, lcov)
  AC_CHECK_PROG(GENHTML, genhtml, genhtml)

  if test "$LCOV"; then
    AC_CACHE_CHECK([for ltp version], cutter_cv_ltp_version, [
      cutter_cv_ltp_version=invalid
      ltp_version=`$LCOV -v 2>/dev/null | $SED -e 's/^.* //'`
      for ltp_check_version in $ltp_version_list; do
        if test "$ltp_version" = "$ltp_check_version"; then
          cutter_cv_ltp_version="$ltp_check_version (ok)"
        fi
      done
    ])
  fi

  case $cutter_cv_ltp_version in
    ""|invalid[)]
      ;;
    *)
      cutter_has_lcov=yes
      ;;
  esac

  if test "x$cutter_has_lcov" != "xyes"; then
    ac_cv_enable_coverage=no
  fi

  COVERAGE_CFLAGS=
  if test "x$ac_cv_enable_coverage" = "xyes"; then
    COVERAGE_CFLAGS="--coverage"
  fi
  AC_SUBST(COVERAGE_CFLAGS)
  AM_CONDITIONAL([ENABLE_COVERAGE], [test "x$ac_cv_enable_coverage" = "xyes"])

  COVERAGE_INFO_FILE="coverage.info"
  AC_SUBST(COVERAGE_INFO_FILE)

  COVERAGE_REPORT_DIR="coverage"
  AC_SUBST(COVERAGE_REPORT_DIR)

  if test "$GENHTML_OPTIONS" = ""; then
    GENHTML_OPTIONS=""
  fi
  AC_SUBST(GENHTML_OPTIONS)

  if test "x$ac_cv_enable_coverage" = "xyes"; then
    AC_CONFIG_COMMANDS([coverage], [
      if grep '^coverage:' Makefile > /dev/null; then
        : # do nothing
      else
        cat >>Makefile <<EOS

coverage-clean:
	\$(LCOV) --compat-libtool --zerocounters --directory . \\
	  --output-file \$(COVERAGE_INFO_FILE)

coverage: coverage-clean check
	\$(LCOV) --compat-libtool --directory . \\
	  --capture --output-file \$(COVERAGE_INFO_FILE)
	\$(LCOV) --compat-libtool --directory . \\
	  --extract \$(COVERAGE_INFO_FILE) "\`(cd '\$(top_srcdir)'; pwd)\`/*" \\
	  --output-file \$(COVERAGE_INFO_FILE)
	\$(GENHTML) --highlight --legend \\
	  --output-directory \$(COVERAGE_REPORT_DIR) \\
	  --prefix "\`(cd '\$(top_srcdir)'; pwd)\`" \\
	  \$(GENHTML_OPTIONS) \$(COVERAGE_INFO_FILE)
EOS
      fi
    ])
  fi
])

AC_DEFUN([AC_CHECK_CUTTER],
[
  AC_ARG_WITH([cutter],
              AS_HELP_STRING([--with-cutter],
                             [Use Cutter (default: auto)]),
              [ac_cv_with_cutter=$withval],
              [ac_cv_with_cutter=auto])
  AC_CACHE_CHECK([whether to use Cutter],
                 [ac_cv_with_cutter],
                 [ac_cv_with_cutter=auto])
  AC_CACHE_VAL([ac_cv_use_cutter],
               [ac_cv_use_cutter=$ac_cv_with_cutter
                if test "$ac_cv_use_cutter" != "no"; then
                  PKG_CHECK_MODULES(CUTTER, cutter $1,
                                    [], [ac_cv_use_cutter=no])
                fi

                if test "$ac_cv_use_cutter" != "no"; then
                  _PKG_CONFIG(CUTTER, variable=cutter, cutter)
                  CUTTER=$pkg_cv_CUTTER
                fi])
  AC_SUBST([CUTTER_CFLAGS])
  AC_SUBST([CUTTER_LIBS])
  AC_SUBST([CUTTER])
])

AC_DEFUN([AC_CHECK_GCUTTER],
[
  AC_CHECK_CUTTER($1)
  AC_CACHE_VAL([ac_cv_use_gcutter],
               [ac_cv_use_gcutter=no
		if test "$ac_cv_use_cutter" != "no"; then
		  PKG_CHECK_MODULES(GCUTTER, gcutter $1,
                                    [ac_cv_use_gcutter=yes], [:])
		fi])
  AC_SUBST([GCUTTER_CFLAGS])
  AC_SUBST([GCUTTER_LIBS])
])

AC_DEFUN([AC_CHECK_CPPCUTTER],
[
  AC_CHECK_CUTTER($1)
  AC_CACHE_VAL([ac_cv_use_cppcutter],
               [ac_cv_use_cppcutter=no
		if test "$ac_cv_use_cutter" != "no"; then
		  PKG_CHECK_MODULES(CPPCUTTER, cppcutter $1,
                                    [ac_cv_use_cppcutter=yes], [:])
		fi])
  AC_SUBST([CPPCUTTER_CFLAGS])
  AC_SUBST([CPPCUTTER_LIBS])
])

AC_DEFUN([AC_CHECK_GDKCUTTER_PIXBUF],
[
  AC_CHECK_GCUTTER($1)
  AC_CACHE_VAL([ac_cv_use_gdkcutter_pixbuf],
               [ac_cv_use_gdkcutter_pixbuf=no
		if test "$ac_cv_use_cutter" != "no"; then
		  PKG_CHECK_MODULES(GDKCUTTER_PIXBUF, gdkcutter-pixbuf $1,
				    [ac_cv_use_gdkcutter_pixbuf=yes], [:])
		fi])
  AC_SUBST([GDKCUTTER_PIXBUF_CFLAGS])
  AC_SUBST([GDKCUTTER_PIXBUF_LIBS])
])

AC_DEFUN([AC_CHECK_SOUPCUTTER],
[
  AC_CHECK_GCUTTER($1)
  AC_CACHE_VAL([ac_cv_use_soupcutter],
	       [ac_cv_use_soupcutter=no
		if test "$ac_cv_use_cutter" != "no"; then
		  PKG_CHECK_MODULES(SOUPCUTTER, soupcutter $1,
				    [ac_cv_use_soupcutter=yes], [:])
		fi])
  AC_SUBST([SOUPCUTTER_CFLAGS])
  AC_SUBST([SOUPCUTTER_LIBS])
])
