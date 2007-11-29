INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)

noinst_LTLIBRARIES = 				\
		repository_test_case1.la		\
		repository_test_case2.la		\
		repository_test_case3.la

LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

repository_test_case1_la_SOURCES = test-case1.c
repository_test_case2_la_SOURCES = test-case2.c
repository_test_case3_la_SOURCES = test-case3.c

