if HAVE_GSTREAMER
INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 			\
		dummy_loader_test.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

dummy_loader_test_la_SOURCES = dummy-test-functions.c
dummy_loader_test_la_LIBADD = $(top_builddir)/cutter/libcutter.la
endif
