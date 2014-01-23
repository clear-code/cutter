AM_CPPFLAGS =			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)	\
	 -I$(top_srcdir)/cutter	\
	 -I$(top_srcdir)/gcutter

AM_CFLAGS =			\
	$(CUTTER_CFLAGS)	\
	$(LIBSOUP_CFLAGS)	\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

if HAVE_LIBSOUP
lib_LTLIBRARIES = libsoupcutter.la

pkginclude_HEADERS =		\
	soupcutter.h

soupcutter_includedir=$(pkgincludedir)/soupcutter
soupcutter_include_HEADERS =		\
	$(soupcutter_public_headers)
endif

soupcutter_public_headers =			\
	soupcut-assertions.h			\
	soupcut-assertions-helper.h		\
	soupcut-client.h			\
	soupcut-server.h

libsoupcutter_la_SOURCES =			\
	$(gcutter_public_headers)		\
	soupcut-assertions-helper.c		\
	soupcut-client.c			\
	soupcut-server.c

libsoupcutter_la_LDFLAGS =		\
	-version-info $(LT_VERSION_INFO)	\
	-no-undefined

libsoupcutter_la_LIBADD =			\
	$(top_builddir)/cutter/libcutter.la	\
	$(LIBSOUP_LIBS)

echo-public-headers:
	@for header in $(soupcutter_include_HEADERS); do	\
	  echo $(abs_srcdir)/$${header};			\
	done
