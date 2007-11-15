INCLUDES = $(CUTTER_CFLAGS)	\
	$(CAIRO_CFLAGS)		\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_PROGRAMS = cutter-cairo

noinst_LTLIBRARIES = 			\
		test_nil_surface.la

LIBADD = $(CAIRO_LIBS)

LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

cutter_cairo_SOURCES = cutter-cairo.c
cutter_cairo_LDFLAGS = $(LDFLAGS) $(CAIRO_LDFLAGS)
cutter_cairo_LDADD = $(LIBADD) $(top_builddir)/cutter/libcutter.la

test_nil_surface_la_SOURCES = nil-surface.c
test_nil_surface_la_LDFLAGS = $(LDFLAGS)
test_nil_surface_la_LIBADD = $(LIBADD)
