INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)	\
	 -I$(top_srcdir)/cutter

AM_CXXFLAGS = 			\
	$(CUTTER_CFLAGS)	\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

lib_LTLIBRARIES = libcppcutter.la

cppcutter_public_headers =			\
	cppcut-assertions.h			\
	cppcut-assertions-helper.h

pkginclude_HEADERS = 	\
	cppcutter.h

cppcutter_includedir=$(pkgincludedir)/cppcutter
cppcutter_include_HEADERS = 		\
	$(cppcutter_public_headers)

libcppcutter_la_SOURCES =		\
	$(cppcutter_public_headers)	\
	cppcut-assertions-helper.cpp
