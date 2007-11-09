SUBDIRS = loader_test_dir repository_test_dir

INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_LTLIBRARIES = 				\
		libcuttest_utils.la		\
		libtest_cut_assertions.la	\
		libtest_cut_test.la		\
		libtest_cut_test_result.la	\
		libtest_cut_test_case.la	\
		libtest_cut_test_suite.la	\
		libtest_cut_loader.la		\
		libtest_cut_repository.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-avoid-version

libcuttest_utils_la_SOURCES = 		\
			cuttest-utils.c	\
			cuttest-utils.h

libtest_cut_assertions_la_SOURCES  = test-cut-assertions.c
libtest_cut_test_la_SOURCES        = test-cut-test.c
libtest_cut_test_result_la_SOURCES = test-cut-test-result.c
libtest_cut_test_case_la_SOURCES   = test-cut-test-case.c
libtest_cut_test_suite_la_SOURCES  = test-cut-test-suite.c
libtest_cut_test_suite_la_LIBADD   = libcuttest_utils.la
libtest_cut_loader_la_SOURCES      = test-cut-loader.c
libtest_cut_loader_la_LIBADD       = libcuttest_utils.la
libtest_cut_repository_la_SOURCES  = test-cut-repository.c
libtest_cut_repository_la_LIBADD   = libcuttest_utils.la

