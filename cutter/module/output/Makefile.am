INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

LDFLAGS =							\
  -rpath $(output_moduledir) -avoid-version -module		\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

output_module_LTLIBRARIES = console.la

console_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/Output:Console\" 
console_la_SOURCES = \
	cut-output-console.c
console_la_LIBADD = $(GLIB_LIBS)

if HAVE_GTK
output_module_LTLIBRARIES += gtk.la
gtk_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/Output:Gtk\" \
		$(GTK_CFLAGS)
gtk_la_SOURCES = \
	cut-output-gtk.c
gtk_la_LIBADD = $(GTK_LIBS)
endif
