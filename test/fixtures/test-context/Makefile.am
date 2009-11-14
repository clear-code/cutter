INCLUDES =			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter	\
	-I$(top_srcdir)/gcutter

AM_CFLAGS =						\
	-DCUT_RELATIVE_PATH="\"fixtures/test-context\""	\
	$(CUTTER_CFLAGS)

noinst_LTLIBRARIES = 		\
	test-relative-path.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

test_relative_path_la_SOURCES = test-relative-path.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
