INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS = 					\
	$(CUTTER_CFLAGS)			\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

noinst_LTLIBRARIES = libgcutter.la

gcutter_public_headers =			\
		gcut-assertions.h		\
		gcut-assertions-helper.h	\
		gcut-public.h			\
		gcut-test-utils.h		\
		gcut-value-equal.h		\
		gcut-list.h			\
		gcut-hash.h

noinst_headers = 			\
		gbsearcharray.h

cutterincludedir=$(includedir)/cutter
cutterinclude_HEADERS = 		\
	gcutter.h

gcuttersubincludedir=$(includedir)/cutter/gcutter
gcuttersubinclude_HEADERS = 		\
	$(gcutter_public_headers)

libgcutter_la_SOURCES =			\
	$(gcutter_public_headers)	\
	$(noinst_headers)		\
	gcut-value-equal.c		\
	gcut-list.c			\
	gcut-hash.c
