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
  
  COVERAGE_CFLAGS=
  if test "x$ac_cv_enable_coverage" = "xyes"; then
    COVERAGE_CFLAGS="--coverage"
  fi
  AC_SUBST(COVERAGE_CFLAGS)
  AM_CONDITIONAL([ENABLE_COVERAGE], [test "x$ac_cv_enable_coverage" = "xyes"])
])

AC_DEFUN([AC_CHECK_CUTTER],
[
  PKG_CHECK_MODULES(CUTTER, cutter)
  AC_SUBST([CUTTER_CFLAGS])
  AC_SUBST([CUTTER_LIBS])

  _PKG_CONFIG(CUTTER, variable=cutter, cutter)
  CUTTER=$pkg_cv_CUTTER
  AC_SUBST([CUTTER])
])
