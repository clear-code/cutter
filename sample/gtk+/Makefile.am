INCLUDES = $(CUTTER_CFLAGS)	\
	$(GTK_CFLAGS)		\
	$(GTK_PRINT_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes

noinst_PROGRAMS = cutter-gtk

noinst_LTLIBRARIES = 			\
		test_builder.la		\
		test_floating.la

LIBADD = $(GTK_LIBS)

LDFLAGS =			\
	-rpath $(libdir)	\
	-module			\
	-avoid-version

cutter_gtk_SOURCES = cutter-gtk.c
cutter_gtk_LDFLAGS = $(LDFLAGS) $(GTK_LDFLAGS)
cutter_gtk_LDADD = $(LIBADD) $(top_builddir)/cutter/libcutter.la

test_builder_la_SOURCES = test-builder.c
test_builder_la_LDFLAGS = $(LDFLAGS) -export-dynamic
test_builder_la_LIBADD = $(LIBADD)

test_floating_la_SOURCES = test-floating.c
test_floating_la_LIBADD = $(LIBADD)

