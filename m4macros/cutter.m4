AC_DEFUN([AC_CHECK_ENABLE_COVERAGE],
[
  AC_MSG_CHECKING([for enabling coverage])
  AC_ARG_ENABLE([coverage],
                AS_HELP_STRING([--enable-coverage],
                               [Enable coverage]),
                [cutter_enable_coverage=$enableval],
                [cutter_enable_coverage=no])
  AC_MSG_RESULT($cutter_enable_coverage)
  cutter_enable_coverage_report_lcov=no
  if test "x$cutter_enable_coverage" != "xno"; then
    ltp_version_list="1.6 1.7 1.8 1.9 1.10 1.11"
    AC_PATH_TOOL(LCOV, lcov)
    AC_PATH_TOOL(GENHTML, genhtml)

    if test -x "$LCOV"; then
      AC_CACHE_CHECK([for ltp version],
                     cutter_cv_ltp_version,
                     [
        ltp_version=`$LCOV -v 2>/dev/null | $SED -e 's/^.* //'`
        cutter_cv_ltp_version="$ltp_version (NG)"
        for ltp_check_version in $ltp_version_list; do
          if test "$ltp_version" = "$ltp_check_version"; then
            cutter_cv_ltp_version="$ltp_check_version (ok)"
          fi
        done
      ])
    fi

    AC_MSG_CHECKING([for enabling coverage report by LCOV])
    case "$cutter_cv_ltp_version" in
      *\(ok\)*)
        cutter_enable_coverage_report_lcov=yes
        ;;
      *)
        cutter_enable_coverage_report_lcov=no
        ;;
    esac
    AC_MSG_RESULT($cutter_enable_coverage_report_lcov)
  fi
])

AC_DEFUN([AC_CHECK_COVERAGE],
[
  ac_check_coverage_makefile=$1
  if test -z "$ac_check_coverage_makefile"; then
    ac_check_coverage_makefile=Makefile
  fi
  AC_SUBST(ac_check_coverage_makefile)

  AC_CHECK_ENABLE_COVERAGE

  COVERAGE_CFLAGS=
  COVERAGE_LIBS=
  if test "$cutter_enable_coverage" = "yes"; then
    COVERAGE_CFLAGS="--coverage"
    COVERAGE_LIBS="-lgcov"
  fi
  AC_SUBST(COVERAGE_CFLAGS)
  AC_SUBST(COVERAGE_LIBS)
  AM_CONDITIONAL([ENABLE_COVERAGE], [test "$cutter_enable_coverage" = "yes"])
  AM_CONDITIONAL([ENABLE_COVERAGE_REPORT_LCOV],
                 [test "$cutter_enable_coverage_report_lcov" = "yes"])

  COVERAGE_INFO_FILE="coverage.info"
  AC_SUBST(COVERAGE_INFO_FILE)

  COVERAGE_REPORT_DIR="coverage"
  AC_SUBST(COVERAGE_REPORT_DIR)

  if test "$GENHTML_OPTIONS" = ""; then
    GENHTML_OPTIONS=""
  fi
  AC_SUBST(GENHTML_OPTIONS)

  if test "$cutter_enable_coverage_report_lcov" = "yes"; then
    AC_CONFIG_COMMANDS([coverage-report-lcov], [
      if test -e "$ac_check_coverage_makefile" && \
         grep -q '^coverage:' $ac_check_coverage_makefile; then
        : # do nothing
      else
        sed -e 's/^        /	/g' <<EOS >>$ac_check_coverage_makefile
.PHONY: coverage-clean coverage-report coverage coverage-force

coverage-clean:
	\$(LCOV) --compat-libtool --zerocounters --directory . \\
	  --output-file \$(COVERAGE_INFO_FILE)

coverage-report:
	\$(LCOV) --compat-libtool --directory . \\
	  --capture --output-file \$(COVERAGE_INFO_FILE)
	\$(LCOV) --compat-libtool --directory . \\
	  --extract \$(COVERAGE_INFO_FILE) "\`(cd '\$(top_srcdir)'; pwd)\`/*" \\
	  --output-file \$(COVERAGE_INFO_FILE)
	\$(GENHTML) --highlight --legend \\
	  --output-directory \$(COVERAGE_REPORT_DIR) \\
	  --prefix "\`(cd '\$(top_srcdir)'; pwd)\`" \\
	  \$(GENHTML_OPTIONS) \$(COVERAGE_INFO_FILE)

coverage: coverage-clean check coverage-report

coverage-force:
	\$(MAKE) \$(AM_MAKEFLAGS) coverage-clean
	\$(MAKE) \$(AM_MAKEFLAGS) check || :
	\$(MAKE) \$(AM_MAKEFLAGS) coverage-report
EOS
      fi
    ],
    [ac_check_coverage_makefile="$ac_check_coverage_makefile"])
  fi
])

AC_DEFUN([AC_CHECK_CUTTER],
[
  AC_ARG_WITH([cutter],
              AS_HELP_STRING([--with-cutter],
                             [Use Cutter (default: auto)]),
              [cutter_with_value=$withval],
              [cutter_with_value=auto])
  if test -z "$cutter_use_cutter"; then
    if test "x$cutter_with_value" = "xno"; then
      cutter_use_cutter=no
    else
      m4_ifdef([PKG_CHECK_MODULES], [
	PKG_CHECK_MODULES(CUTTER, cutter $1,
			  [cutter_use_cutter=yes],
			  [cutter_use_cutter=no])
        ],
        [cutter_use_cutter=no])
    fi
  fi
  if test "$cutter_use_cutter" != "no"; then
    _PKG_CONFIG(CUTTER, variable=cutter, cutter)
    CUTTER=$pkg_cv_CUTTER
  fi
  ac_cv_use_cutter="$cutter_use_cutter" # for backward compatibility
  AC_SUBST([CUTTER_CFLAGS])
  AC_SUBST([CUTTER_LIBS])
  AC_SUBST([CUTTER])
])

AC_DEFUN([AC_CHECK_GCUTTER],
[
  AC_CHECK_CUTTER($1)
  if test "$cutter_use_cutter" = "no"; then
    cutter_use_gcutter=no
  fi
  if test "x$cutter_use_gcutter" = "x"; then
    m4_ifdef([PKG_CHECK_MODULES], [
      PKG_CHECK_MODULES(GCUTTER, gcutter $1,
			[cutter_use_gcutter=yes],
			[cutter_use_gcutter=no])
      ],
      [cutter_use_gcutter=no])
  fi
  ac_cv_use_gcutter="$cutter_use_gcutter" # for backward compatibility
  AC_SUBST([GCUTTER_CFLAGS])
  AC_SUBST([GCUTTER_LIBS])
])

AC_DEFUN([AC_CHECK_CPPCUTTER],
[
  AC_CHECK_CUTTER($1)
  if test "$cutter_use_cutter" = "no"; then
    cutter_use_cppcutter=no
  fi
  if test "x$cutter_use_cppcutter" = "x"; then
    m4_ifdef([PKG_CHECK_MODULES], [
      PKG_CHECK_MODULES(CPPCUTTER, cppcutter $1,
			[cutter_use_cppcutter=yes],
			[cutter_use_cppcutter=no])
      ],
      [cutter_use_cppcutter=no])
  fi
  ac_cv_use_cppcutter="$cutter_use_cppcutter" # for backward compatibility
  AC_SUBST([CPPCUTTER_CFLAGS])
  AC_SUBST([CPPCUTTER_LIBS])
])

AC_DEFUN([AC_CHECK_GDKCUTTER_PIXBUF],
[
  AC_CHECK_GCUTTER($1)
  if test "$cutter_use_cutter" = "no"; then
    cutter_use_gdkcutter_pixbuf=no
  fi
  if test "x$cutter_use_gdkcutter_pixbuf" = "x"; then
    m4_ifdef([PKG_CHECK_MODULES], [
      PKG_CHECK_MODULES(GDKCUTTER_PIXBUF, gdkcutter-pixbuf $1,
			[cutter_use_gdkcutter_pixbuf=yes],
			[cutter_use_gdkcutter_pixbuf=no])
      ],
      [cutter_use_gdkcutter_pixbuf=no])
  fi
  ac_cv_use_gdkcutter_pixbuf="$cutter_use_gdkcutter_pixbuf" # for backward compatibility
  AC_SUBST([GDKCUTTER_PIXBUF_CFLAGS])
  AC_SUBST([GDKCUTTER_PIXBUF_LIBS])
])

AC_DEFUN([AC_CHECK_SOUPCUTTER],
[
  AC_CHECK_GCUTTER($1)
  if test "$cutter_use_cutter" = "no"; then
    cutter_use_soupcutter=no
  fi
  if test "$cutter_use_soupcutter" != "no"; then
    m4_ifdef([PKG_CHECK_MODULES], [
      PKG_CHECK_MODULES(SOUPCUTTER, soupcutter $1,
			[cutter_use_soupcutter=yes],
			[cutter_use_soupcutter=no])
      ],
      [cutter_use_soupcutter=no])
  fi
  ac_cv_use_soupcutter="$cutter_use_soupcutter" # for backward compatibility
  AC_SUBST([SOUPCUTTER_CFLAGS])
  AC_SUBST([SOUPCUTTER_LIBS])
])
