if HAVE_GSTREAMER
gst_plugins_LTLIBRARIES = libgstcutter.la

noinst_HEADERS = gst-cutter-src.h

INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

libgstcutter_la_SOURCES =	\
	gst-cutter.c		\
	gst-cutter-src.c

libgstcutter_la_CFLAGS = 		\
        $(GST_PLUGINS_BASE_CFLAGS)	\
	$(GST_BASE_CFLAGS) 		\
	$(GSTREAMER_CFLAGS)		\
	$(CUTTER_CFLAGS)

libgstcutter_la_LIBADD = 	\
	$(GST_BASE_LIBS) 	\
	$(GSTREAMER_LIBS) 	\
	$(CUTTER_LIBS)

libgstcutter_la_LDFLAGS =		\
	$(GST_PLUGIN_LDFLAGS)		\
	-avoid-version -module		\
	-export-dynamic 		\
	$(no_undefined)			\
	$(LIBTOOL_EXPORT_OPTIONS)
endif
