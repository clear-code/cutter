INCLUDES = 					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter			\
	-I$(top_srcdir)/cutter++		\
	-I$(top_srcdir)/test/lib

AM_CXXFLAGS = $(CUTTER_CFLAGS)

CLEANFILES = *.gcno *.gcda

noinst_LTLIBRARIES = 			\
	test-cut++-assertions.la

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/cutter++/libcutter++.la		\
	$(top_builddir)/test/lib/libcuttest-utils.la

test_cut___assertions_la_SOURCES	= test-cut++-assertions.cpp
