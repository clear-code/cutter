INCLUDES =			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

AM_CFLAGS = $(CUTTER_CFLAGS)	\
	$(CAIRO_CFLAGS)

EXTRA_DIST = run-test.sh

if MAINTAINER_MODE
TESTS = run-test.sh
endif
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_PROGRAMS = cutter-cairo

noinst_LTLIBRARIES = 			\
		test_nil_surface.la

LIBS = $(CAIRO_LIBS)

AM_LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

cutter_cairo_SOURCES = cutter-cairo.c
cutter_cairo_LDFLAGS = $(AM_LDFLAGS) $(CAIRO_LDFLAGS)
cutter_cairo_LDADD = $(top_builddir)/cutter/libcutter.la

test_nil_surface_la_SOURCES = nil-surface.c

