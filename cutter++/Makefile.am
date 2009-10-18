INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)	\
	 -I$(top_srcdir)/cutter

AM_CXXFLAGS = 			\
	$(CUTTER_CFLAGS)	\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

lib_LTLIBRARIES = libcutter++.la

cutter___public_headers =			\
	cut++-assertions.h			\
	cut++-assertions-helper.h

pkginclude_HEADERS = 	\
	cutter++.h

cutter___includedir=$(pkgincludedir)/cutter++
cutter___include_HEADERS = 		\
	$(cutter___public_headers)

libcutter___la_SOURCES =		\
	$(cutter___public_headers)
