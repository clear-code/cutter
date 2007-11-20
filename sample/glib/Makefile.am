INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_LTLIBRARIES = 			\
		test_array.la		\
		test_asyncqueue.la	\
		test_atomic.la		\
		test_base64.la		\
		test_keyfile.la		\
		test_option.la		\
		test_regex.la		\
		test_thread.la		\
		test_utf8_pointer.la	\
		test_utf8_validate.la	\
		test_uri.la		\
		test_properties.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

test_array_la_SOURCES = array.c
test_asyncqueue_la_SOURCES = asyncqueue.c
test_atomic_la_SOURCES = atomic.c
test_base64_la_SOURCES = base64.c
test_keyfile_la_SOURCES = keyfile.c
test_option_la_SOURCES = option.c
test_regex_la_SOURCES = regex.c
test_thread_la_SOURCES = thread.c
test_uri_la_SOURCES = uri.c
test_utf8_pointer_la_SOURCES = utf8-pointer.c
test_utf8_validate_la_SOURCES = utf8-validate.c

test_properties_la_SOURCES = properties.c
