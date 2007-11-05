INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh

noinst_LTLIBRARIES = 				\
		libtest_glib_uri.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-avoid-version

libtest_glib_uri_la_SOURCES = test_glib_uri.c

