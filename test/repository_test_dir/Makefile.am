INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

noinst_LTLIBRARIES = 				\
		libtest_case1.la		\
		libtest_case2.la		\
		libtest_case3.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-avoid-version

libtest_case1_la_SOURCES = test_case1.c
libtest_case2_la_SOURCES = test_case2.c
libtest_case3_la_SOURCES = test_case3.c

