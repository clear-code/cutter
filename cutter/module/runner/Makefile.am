INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

LDFLAGS =							\
  -rpath $(output_moduledir) -avoid-version -module		\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

runner_module_LTLIBRARIES =

if HAVE_GTK
runner_module_LTLIBRARIES += gtk.la
gtk_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/Output:Gtk\" \
		$(GTK_CFLAGS)
gtk_la_SOURCES = \
	cut-runner-gtk.c
gtk_la_LIBADD = $(GTK_LIBS)
endif
