INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

noinst_LTLIBRARIES = 				\
		librepository_test_case1.la		\
		librepository_test_case2.la		\
		librepository_test_case3.la

LDFLAGS =			\
	-rpath $(libdir)	\
	-avoid-version

librepository_test_case1_la_SOURCES = test-case1.c
librepository_test_case2_la_SOURCES = test-case2.c
librepository_test_case3_la_SOURCES = test-case3.c

