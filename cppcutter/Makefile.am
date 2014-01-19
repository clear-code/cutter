AM_CPPFLAGS =			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)	\
	 -I$(top_srcdir)/cutter

AM_CXXFLAGS = 			\
	-DCPPCUT_BUILDING	\
	$(CUTTER_CFLAGS)	\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

lib_LTLIBRARIES = libcppcutter.la

cppcutter_public_headers =			\
	cppcut-assertions.h			\
	cppcut-assertions-helper.h		\
	cppcut-macros.h				\
	cppcut-message.h			\
	cppcut-test.h

pkginclude_HEADERS =	\
	cppcutter.h

cppcutter_includedir=$(pkgincludedir)/cppcutter
cppcutter_include_HEADERS =		\
	$(cppcutter_public_headers)

libcppcutter_sources =				\
	cppcut-assertions-helper.cpp		\
	cppcut-message.cpp			\
	cppcut-test.cpp

libcppcutter_la_SOURCES =		\
	$(cppcutter_public_headers)	\
	$(libcppcutter_sources)

libcppcutter_la_LDFLAGS =			\
	-version-info $(LT_VERSION_INFO)	\
	-no-undefined

libcppcutter_la_LIBADD =			\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

echo-public-headers:
	@for header in $(cppcutter_include_HEADERS); do			\
	  echo $(abs_srcdir)/$${header};				\
	done

echo-libcppcutter-sources:
	@echo $(libcppcutter_sources)
