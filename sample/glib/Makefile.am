INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_LTLIBRARIES = 			\
		test_base64.la		\
		test_keyfile.la		\
		test_option.la		\
		test_regex.la		\
		test_utf8_pointer.la	\
		test_utf8_validate.la	\
		test_uri.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

test_base64_la_SOURCES = test-base64.c
test_keyfile_la_SOURCES = test-keyfile.c
test_option_la_SOURCES = test-option.c
test_regex_la_SOURCES = test-regex.c
test_uri_la_SOURCES = test-uri.c
test_utf8_pointer_la_SOURCES = test-utf8-pointer.c
test_utf8_validate_la_SOURCES = test-utf8-validate.c

