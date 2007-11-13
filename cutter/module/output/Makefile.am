INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

LDFLAGS =							\
  -rpath $(output_moduledir) -avoid-version -module		\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

output_module_LTLIBRARIES = console.la

console_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/Outputr:Console\" 
console_la_SOURCES = \
	cut-output-console.c

console_la_LIBADD = $(GLIB_LIBS)
