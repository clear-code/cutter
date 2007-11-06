SUBDIRS = loader_test_dir

INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh

noinst_LTLIBRARIES = 				\
		libtest_cut_assertion.la	\
		libtest_cut_test.la		\
		libtest_cut_test_case.la	\
		libtest_cut_test_suite.la	\
		libtest_cut_loader.la		\
		libtest_cut_repository.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-avoid-version

libtest_cut_assertion_la_SOURCES  = test_cut_assertion.c
libtest_cut_test_la_SOURCES       = test_cut_test.c
libtest_cut_test_case_la_SOURCES  = test_cut_test_case.c
libtest_cut_test_suite_la_SOURCES = test_cut_test_suite.c
libtest_cut_loader_la_SOURCES     = test_cut_loader.c
libtest_cut_repository_la_SOURCES = test_cut_repository.c

