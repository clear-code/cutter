INCLUDES = $(CUTTER_CFLAGS)	\
	$(GTK_CFLAGS)		\
	$(GTK_PRINT_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_LTLIBRARIES = 			\
		test_builder.la

LIBADD = $(GTK_LIBS)

LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

test_builder_la_SOURCES = test-builder.c
test_builder_la_LDFLAGS = $(LDFLAGS) -export-dynamic
test_builder_la_LIBADD = $(LIBADD)

