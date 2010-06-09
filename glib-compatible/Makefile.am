SUBDIRS =					\
	pcre

INCLUDES = 			\
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
	$(PCRE_CFLAGS)

LIBS +=						\
	$(PCRE_LIBS)

libglib_compatible_la_SOURCES +=		\
	gregex.c				\
	gregex.h				\
	glibintl.h				\
	gsequence.c				\
	gsequence.h				\
	gstring.c				\
	gstring.h
endif
