INCLUDES = 			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

AM_CFLAGS = $(CUTTER_CFLAGS)
if HAVE_GTK
AM_CFLAGS += $(GTK_CFLAGS)
endif

EXTRA_DIST = cuttest-assertions.h

noinst_LTLIBRARIES = 			\
	libcuttest_utils.la		\
	libcuttest_event_receiver.la

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

AM_LDFLAGS =			\
	-rpath `pwd`/.libs	\
	-avoid-version

libcuttest_utils_la_SOURCES = 		\
	cuttest-utils.c			\
	cuttest-utils.h

libcuttest_event_receiver_la_SOURCES = 		\
	cuttest-event-receiver.c		\
	cuttest-event-receiver.h
