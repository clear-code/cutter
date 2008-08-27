INCLUDES =			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

AM_CFLAGS = 			\
	$(CAIRO_CFLAGS)		\
	$(CUTTER_CFLAGS)

EXTRA_DIST = run-test.sh

if MAINTAINER_MODE
TESTS = run-test.sh
endif
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_LTLIBRARIES = 			\
		test_nil_surface.la

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(CAIRO_LIBS)

AM_LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version		\
	-no-undefined

test_nil_surface_la_SOURCES = nil-surface.c

