INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 				\
		suite-cuttest-loader-valid.la	\
		suite-cuttest-loader-invalid.la	\
		suite-cuttest-loader-none.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

suite_cuttest_loader_valid_la_SOURCES = suite-cuttest-loader-valid.c
suite_cuttest_loader_invalid_la_SOURCES = suite-cuttest-loader-invalid.c
suite_cuttest_loader_none_la_SOURCES = suite-cuttest-loader-none.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
