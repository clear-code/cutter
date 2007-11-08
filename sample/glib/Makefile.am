INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_LTLIBRARIES = 				\
		test_glib_uri.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

test_glib_uri_la_SOURCES = test_glib_uri.c

