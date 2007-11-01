INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

EXTRA_DIST = run-test.sh

noinst_LTLIBRARIES = 			\
		libtest_triv.la		\
		libtest_sample.la

libtest_triv_la_SOURCES = test_triv.c
libtest_triv_la_LDFLAGS = -rpath $(libdir) 

libtest_sample_la_SOURCES = test_sample.c
libtest_sample_la_LDFLAGS = -rpath $(libdir) 
