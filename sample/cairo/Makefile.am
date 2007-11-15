INCLUDES = $(CUTTER_CFLAGS)	\
	$(CAIRO_CFLAGS)		\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

#noinst_PROGRAMS = cutter-gtk

noinst_LTLIBRARIES = 			\
		test_nil_surface.la

LIBADD = $(CAIRO_LIBS)

LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

test_nil_surface_la_SOURCES = nil-surface.c
test_nil_surface_la_LDFLAGS = $(LDFLAGS)
test_nil_surface_la_LIBADD = $(LIBADD)
