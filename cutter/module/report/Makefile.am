INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

LDFLAGS =							\
  -rpath $(ui_moduledir) -avoid-version -module			\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

report_module_LTLIBRARIES = xml.la

xml_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/Report:XML\"
xml_la_SOURCES = \
	cut-report-xml.c
xml_la_LIBADD = $(GLIB_LIBS)

