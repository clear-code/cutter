INCLUDES = 			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter	\
	-I$(top_srcdir)/gcutter

AM_CFLAGS = $(CUTTER_CFLAGS)
if HAVE_GTK
AM_CFLAGS += $(GTK_CFLAGS)
endif

EXTRA_DIST = cuttest-assertions.h

noinst_LTLIBRARIES = 			\
	libcuttest-utils.la		\
	libcuttest-event-receiver.la

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

AM_LDFLAGS =                    \
        -rpath `pwd`/.libs      \
        -avoid-version          \
        -no-undefined

libcuttest_utils_la_SOURCES = 		\
	cuttest-utils.c			\
	cuttest-utils.h			\
	cuttest-assertions.c		\
	cuttest-assertions.h

libcuttest_event_receiver_la_SOURCES = 		\
	cuttest-event-receiver.c		\
	cuttest-event-receiver.h
