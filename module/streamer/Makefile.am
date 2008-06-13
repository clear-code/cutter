INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS =			\
	$(GLIB_CFLAGS)		\
	$(COVERAGE_CFLAGS)

LDFLAGS =							\
  -avoid-version -module					\
  -no-undefined -export-dynamic $(LIBTOOL_EXPORT_OPTIONS)

streamer_module_LTLIBRARIES = 	\
		xml.la

streamer_factory_module_LTLIBRARIES =	\
		xml_factory.la

LIBS =						\
	$(GLIB_LIBS)				\
	$(top_builddir)/cutter/libcutter.la

xml_la_CPPFLAGS =
xml_la_SOURCES = cut-xml-streamer.c

xml_factory_la_CPPFLAGS =
xml_factory_la_SOURCES = cut-xml-streamer-factory.c

