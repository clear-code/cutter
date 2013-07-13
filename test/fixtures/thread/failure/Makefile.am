AM_CPPFLAGS =			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter	\
	-I$(top_srcdir)/gcutter

AM_CFLAGS =						\
	-DCUT_RELATIVE_PATH="\"fixtures/thread\""	\
	$(CUTTER_CFLAGS)

check_LTLIBRARIES = 		\
	stub-failure.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

stub_failure_la_SOURCES = stub-failure.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
