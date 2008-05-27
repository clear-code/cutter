SUBDIRS = test

if HAVE_GSTREAMER
gst_plugins_LTLIBRARIES = libgstcuttertest.la

noinst_HEADERS = gst-cutter-test-runner.h

INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

libgstcuttertest_la_SOURCES =		\
	gst-cutter-test.c		\
	gst-cutter-test-runner.c	\
	gst-cutter-test-viewer.c

libgstcuttertest_la_CFLAGS = 		\
        $(GST_PLUGINS_BASE_CFLAGS)	\
	$(GST_BASE_CFLAGS) 		\
	$(GSTREAMER_CFLAGS)		\
	$(CUTTER_CFLAGS)

libgstcuttertest_la_LIBADD = 			\
	$(GST_BASE_LIBS) 			\
	$(GSTREAMER_LIBS) 			\
	$(top_builddir)/cutter/libcutter.la

libgstcuttertest_la_LDFLAGS =		\
	$(GST_PLUGIN_LDFLAGS)		\
	-avoid-version -module		\
	-export-dynamic 		\
	$(no_undefined)			\
	$(LIBTOOL_EXPORT_OPTIONS)
endif
