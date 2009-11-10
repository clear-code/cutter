INCLUDES = 			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter	\
	-I$(top_srcdir)/gcutter

AM_CFLAGS =				\
	-DCUT_RELATIVE_PATH="\"lib\""	\
	$(CUTTER_CFLAGS)
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

libcuttest_utils_public_headers =		\
	cuttest-utils.h				\
	cuttest-assertions.h			\
	cuttest-enum.h

libcuttest_utils_la_SOURCES =			\
	$(libcuttest_utils_public_headers)	\
	cuttest-utils.c				\
	cuttest-assertions.c			\
	cuttest-enum.c

libcuttest_event_receiver_public_headers =	\
	cuttest-event-receiver.h

libcuttest_event_receiver_la_SOURCES = 		\
	cuttest-event-receiver.c

echo-cuttest-utils-public-headers:
	@for header in $(libcuttest_utils_public_headers); do		\
	  echo $(abs_srcdir)/$${header};				\
	done

echo-cuttest-event-receiver-public-headers:
	@for header in $(libcuttest_event_receiver_public_headers); do	\
	  echo $(abs_srcdir)/$${header};				\
	done
