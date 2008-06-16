INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 				\
		dummy_attribute_test.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

dummy_attribute_test_la_SOURCES = dummy-test-functions.c

