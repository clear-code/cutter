INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_LTLIBRARIES = 				\
		test_glib_utf8_pointer.la	\
		test_glib_utf8_validate.la	\
		test_glib_uri.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

test_glib_uri_la_SOURCES = test-glib-uri.c
test_glib_utf8_pointer_la_SOURCES = test-glib-utf8-pointer.c
test_glib_utf8_validate_la_SOURCES = test-glib-utf8-validate.c

