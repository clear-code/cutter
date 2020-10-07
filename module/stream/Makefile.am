AM_CPPFLAGS = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS =			\
	$(GLIB_CFLAGS)		\
	$(COVERAGE_CFLAGS)

AM_LDFLAGS =							\
  -avoid-version -module					\
  -no-undefined -export-dynamic $(LIBTOOL_EXPORT_OPTIONS)

stream_module_LTLIBRARIES = 	\
	xml.la

stream_factory_module_LTLIBRARIES =	\
	xml_factory.la

LIBS =						\
	$(GLIB_LIBS)				\
	$(top_builddir)/cutter/libcutter.la

xml_la_SOURCES = cut-xml-stream.c

xml_factory_la_SOURCES = cut-xml-stream-factory.c

