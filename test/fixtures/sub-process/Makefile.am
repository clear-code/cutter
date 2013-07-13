SUBDIRS = fixtures

AM_CPPFLAGS = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter	\
	-I$(top_srcdir)/gcutter

check_LTLIBRARIES = 		\
	test-invoker.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

test_invoker_la_SOURCES = test-invoker.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
