INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS = 			\
	$(CUTTER_CFLAGS)	\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

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
	gcut-enum.h

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
	gcut-value-equal.c		\
	gcut-list.c			\
	gcut-hash-table.c		\
	gcut-error.c			\
	gcut-enum.c			\
	gcut-assertions-helper.c
