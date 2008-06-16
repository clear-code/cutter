INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 		\
		success_test.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

success_test_la_SOURCES = success-test.c

