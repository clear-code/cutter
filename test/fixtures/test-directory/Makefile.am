INCLUDES =			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter	\
	-I$(top_srcdir)/gcutter	\
	-I$(top_srcdir)/test/lib


AM_CFLAGS =						\
	$(CUTTER_CFLAGS)

check_LTLIBRARIES = 		\
	test-directory.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/test/lib/libcuttest-utils.la	\
	$(GLIB_LIBS)

test_directory_la_SOURCES = test-directory.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
