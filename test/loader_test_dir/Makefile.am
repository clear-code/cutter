INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

noinst_LTLIBRARIES = 				\
		libdummy_loader_test.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-avoid-version

libdummy_loader_test_la_SOURCES = dummy-test-functions.c

