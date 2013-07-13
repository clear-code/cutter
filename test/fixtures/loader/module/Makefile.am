AM_CPPFLAGS = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES =		\
	test1.la		\
	test2.la		\
	cannot-load-module.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

test1_la_SOURCES = test1.c
test2_la_SOURCES = test2.c

cannot_load_module_la_SOURCES = cannot-load-module.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
