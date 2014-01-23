AM_CPPFLAGS =					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter			\
	-I$(top_srcdir)/cppcutter		\
	-I$(top_srcdir)/test/lib

AM_CXXFLAGS =					\
	$(CUTTER_CFLAGS)			\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

check_LTLIBRARIES =			\
	test-cppcut.la

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/cppcutter/libcppcutter.la	\
	$(top_builddir)/test/lib/libcuttest-utils.la	\
	$(GLIB_LIBS)					\
	$(COVERAGE_LIBS)

test_cppcut_la_SOURCES =			\
	test-cppcut-assertions.cpp		\
	test-cppcut-test.cpp			\
	test-cppcut-iterated-test.cpp
