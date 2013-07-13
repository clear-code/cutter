SUBDIRS =					\
	pcre

AM_CPPFLAGS =					\
	 -I$(top_srcdir)

AM_CFLAGS = 					\
	$(GLIB_CFLAGS)

LIBS =						\
	$(GLIB_LIBS)

noinst_LTLIBRARIES = libglib-compatible.la

libglib_compatible_la_SOURCES =			\
	glib-compatible.c			\
	glib-compatible.h

if GLIB_2_12
AM_CFLAGS +=					\
	-DSIZEOF_CHAR=1

libglib_compatible_la_SOURCES +=		\
	gregex.c				\
	gregex.h				\
	glibintl.h				\
	gsequence.c				\
	gsequence.h				\
	gstring.c				\
	gstring.h				\
	guniprop.c				\
	gunicode.h				\
	gscripttable.h

libglib_compatible_la_LIBADD =				\
	$(top_builddir)/glib-compatible/pcre/libpcre.la
endif
