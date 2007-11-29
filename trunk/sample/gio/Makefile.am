INCLUDES = $(CUTTER_CFLAGS)	\
	$(GIO_CFLAGS)		\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_LTLIBRARIES = 			\
		test_file.la

LIBADD = $(GIO_LIBS)

LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

test_file_la_SOURCES = file.c
test_file_la_LIBADD = $(LIBADD)

