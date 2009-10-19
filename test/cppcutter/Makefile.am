INCLUDES = 					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter			\
	-I$(top_srcdir)/cppcutter		\
	-I$(top_srcdir)/test/lib

AM_CXXFLAGS = $(CUTTER_CFLAGS)

CLEANFILES = *.gcno *.gcda

noinst_LTLIBRARIES = 			\
	test-cppcut-assertions.la

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/cppcutter/libcppcutter.la	\
	$(top_builddir)/test/lib/libcuttest-utils.la

test_cppcut_assertions_la_SOURCES	= test-cppcut-assertions.cpp
