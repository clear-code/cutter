SUBDIRS = test

if HAVE_GSTREAMER
gst_plugins_LTLIBRARIES = libgstcuttertest.la
endif

noinst_HEADERS = 			\
	gst-cutter-test-runner.h	\
	gst-cutter-console-output.h	\
	gst-cutter-server.h

INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

libgstcuttertest_la_SOURCES =		\
	gst-cutter-test.c		\
	gst-cutter-test-runner.c	\
	gst-cutter-console-output.c	\
	gst-cutter-server.c

libgstcuttertest_la_CFLAGS = 		\
	$(GSTREAMER_CFLAGS)		\
	$(CUTTER_CFLAGS)

libgstcuttertest_la_LIBADD = 			\
	$(GSTREAMER_LIBS) 			\
	$(top_builddir)/cutter/libcutter.la

libgstcuttertest_la_LDFLAGS =		\
	-avoid-version -module		\
	-export-dynamic 		\
	-no-undefined			\
	$(LIBTOOL_EXPORT_OPTIONS)
