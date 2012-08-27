if MAINTAINER_MODE
TESTS = run-test.sh
endif
TESTS_ENVIRONMENT = NO_MAKE=yes

EXTRA_DIST = run-test.sh

INCLUDES =			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

AM_CFLAGS =			\
	$(CUTTER_CFLAGS)	\
	$(GTK_CFLAGS)

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)				\
	$(GTK_LIBS)

AM_LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version		\
	-no-undefined

noinst_LTLIBRARIES = 			\
		suite_gtk_test.la	\
		test_builder.la		\
		test_floating.la

suite_gtk_test_la_SOURCES = suite_gtk_test.c
test_builder_la_SOURCES  = builder.c
test_floating_la_SOURCES = floating.c

