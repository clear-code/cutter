INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS =			\
	$(GLIB_CFLAGS)		\
	$(COVERAGE_CFLAGS)

LDFLAGS =							\
  -avoid-version -module					\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

streamer_module_LTLIBRARIES = 	\
		xml.la

streamer_factory_module_LTLIBRARIES =	\
		xml_factory.la	

xml_la_CPPFLAGS =
xml_la_SOURCES = cut-xml-streamer.c
xml_la_LIBADD = $(GLIB_LIBS)

xml_factory_la_CPPFLAGS =
xml_factory_la_SOURCES = cut-xml-streamer-factory.c
xml_factory_la_LIBADD = $(GLIB_LIBS)

