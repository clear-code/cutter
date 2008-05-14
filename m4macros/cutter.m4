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
  ltp_version_list="1.6"
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

  if test "x$ac_cv_enable_coverage" = "xyes"; then
    AC_CONFIG_COMMANDS([coverage], [
      if grep -v coverage Makefile >/dev/null; then
        cat >>Makefile <<EOS

coverage-clean:
	\$(LCOV) --compat-libtool --zerocounters -d . -o coverage.info

coverage: coverage-clean check
	\$(LCOV) --compat-libtool -d . -c -o coverage.info && \\
	  \$(GENHTML) --highlight --legend -o coverage coverage.info
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
              [ac_cv_use_cutter=$withval],
              [ac_cv_use_cutter=auto])
  AC_CACHE_CHECK([whether to use Cutter],
                 [ac_cv_use_cutter], [ac_cv_use_cutter=auto])
  if test "$ac_cv_use_cutter" != "no"; then
    PKG_CHECK_MODULES(CUTTER, cutter, [], [ac_cv_use_cutter=no])
    AC_SUBST([CUTTER_CFLAGS])
    AC_SUBST([CUTTER_LIBS])
  fi

  if test "$ac_cv_use_cutter" != "no"; then
    _PKG_CONFIG(CUTTER, variable=cutter, cutter)
    CUTTER=$pkg_cv_CUTTER
    AC_SUBST([CUTTER])
  fi
])

AC_DEFUN([AC_CHECK_GCUTTER],
[
  AC_REQUIRE([AC_CHECK_CUTTER])
  if test "$ac_cv_use_cutter" != "no"; then
    PKG_CHECK_MODULES(GCUTTER, gcutter, [], [:])
    AC_SUBST([GCUTTER_CFLAGS])
    AC_SUBST([GCUTTER_LIBS])
  fi
])
