INCLUDES =			\
	$(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 		\
	setup-error-test.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

setup_error_test_la_SOURCES = setup-error-test.c

