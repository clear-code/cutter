INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

noinst_LTLIBRARIES = 				\
		libdummy_loader_test.la

libdummy_loader_test_la_SOURCES = dummy_test_functions.c
libdummy_loader_test_la_LDFLAGS = -rpath $(libdir) 

