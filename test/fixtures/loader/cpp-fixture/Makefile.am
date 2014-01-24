AM_CPPFLAGS =					\
	$(CUTTER_CFLAGS)			\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter			\
	-I$(top_srcdir)/cppcutter

noinst_LTLIBRARIES =		\
	with-prefix.la		\
	without-prefix.la	\
	all.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/gcutter/libgcutter.la		\
	$(top_builddir)/cppcutter/libcppcutter.la	\
	$(GLIB_LIBS)					\
	$(GIO_LIBS)					\
	$(COVERAGE_LIBS)

with_prefix_la_SOURCES = with-prefix.cpp
without_prefix_la_SOURCES = without-prefix.cpp
all_la_SOURCES = all.cpp

echo-tests:
	@echo $(noinst_LTLIBRARIES)
