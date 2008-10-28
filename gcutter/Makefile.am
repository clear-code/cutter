INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)	\
	 -I$(top_srcdir)/cutter

AM_CFLAGS = 			\
	$(CUTTER_CFLAGS)	\
	$(COVERAGE_CFLAGS)

EXTRA_DIST = gcut-marshalers.list
CLEANFILES = *.gcno *.gcda

marshal_sources =		\
	gcut-marshalers.c	\
	gcut-marshalers.h

MAINTAINERCLEANFILES =		\
	$(marshal_sources)

BUILT_SOURCES = $(marshal_sources)

noinst_LTLIBRARIES = libgcutter.la

gcutter_public_headers =		\
	gcut-assertions.h		\
	gcut-assertions-helper.h	\
	gcut-public.h			\
	gcut-test-utils.h		\
	gcut-value-equal.h		\
	gcut-list.h			\
	gcut-hash-table.h		\
	gcut-error.h			\
	gcut-enum.h			\
	gcut-types.h			\
	gcut-object.h			\
	gcut-io-channel-string.h	\
	gcut-egg.h			\
	gcut-io.h			\
	gcut-main.h

noinst_headers = 	\
	gbsearcharray.h

pkginclude_HEADERS = 	\
	gcutter.h

gcutterincludedir=$(pkgincludedir)/gcutter
gcutterinclude_HEADERS = 		\
	$(gcutter_public_headers)

libgcutter_la_SOURCES =			\
	$(gcutter_public_headers)	\
	$(noinst_headers)		\
	$(marshal_sources)		\
	gcut-value-equal.c		\
	gcut-list.c			\
	gcut-hash-table.c		\
	gcut-error.c			\
	gcut-enum.c			\
	gcut-assertions-helper.c	\
	gcut-object.c			\
	gcut-io-channel-string.c	\
	gcut-egg.c			\
	gcut-io.c			\
	gcut-main.c


gcut-marshalers.h : gcut-marshalers.list
	$(GLIB_GENMARSHAL) $(top_srcdir)/gcutter/gcut-marshalers.list --header --prefix=_gcut_marshal > $@

gcut-marshalers.c : gcut-marshalers.list
	$(GLIB_GENMARSHAL) $(top_srcdir)/gcutter/gcut-marshalers.list --header --body --prefix=_gcut_marshal > $@
