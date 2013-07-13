AM_CPPFLAGS =			\
	$(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 		\
	with-prefix.la		\
	without-prefix.la	\
	all.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

with_prefix_la_SOURCES = with-prefix.c
without_prefix_la_SOURCES = without-prefix.c
all_la_SOURCES = all.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
