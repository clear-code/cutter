INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 		\
		error_test.la

LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

error_test_la_SOURCES = error-test.c

