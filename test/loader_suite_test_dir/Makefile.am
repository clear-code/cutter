INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 				\
		suite_cuttest_loader_valid.la	\
		suite_cuttest_loader_invalid.la	\
		suite_cuttest_loader_none.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

suite_cuttest_loader_valid_la_SOURCES = suite-cuttest-loader-valid.c
suite_cuttest_loader_invalid_la_SOURCES = suite-cuttest-loader-invalid.c
suite_cuttest_loader_none_la_SOURCES = suite-cuttest-loader-none.c
