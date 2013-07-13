AM_CPPFLAGS = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

check_LTLIBRARIES = 		\
	test-deep.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

test_deep_la_SOURCES = test-deep.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
