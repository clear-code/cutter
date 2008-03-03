INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES =		\
		test1.la

LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

test1_la_SOURCES = test1.c
