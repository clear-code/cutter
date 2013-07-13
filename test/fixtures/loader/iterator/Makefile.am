AM_CPPFLAGS =			\
	$(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES =			\
	success-iterated-test.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

success_iterated_test_la_SOURCES = success-iterated-test.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
