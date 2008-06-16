INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES =		\
		test1.la	\
		test2.la	\
		cannot_load_module.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

test1_la_SOURCES = test1.c
test2_la_SOURCES = test2.c

cannot_load_module_la_SOURCES = cannot_load_module.c
