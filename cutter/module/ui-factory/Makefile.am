INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

LDFLAGS =							\
  -rpath $(ui_factory_moduledir) -avoid-version -module		\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

ui_factory_module_LTLIBRARIES = console_factory.la

console_factory_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/UIFactory:Console\"
console_factory_la_SOURCES = \
	cut-ui-factory-console.c
console_factory_la_LIBADD = $(GLIB_LIBS)

if HAVE_GTK
ui_factory_module_LTLIBRARIES += gtk_factory.la
gtk_factory_la_CPPFLAGS =			\
	-DG_LOG_DOMAIN=\"Cut/UIFactory:Gtk\"	\
	$(GTK_CFLAGS)
gtk_factory_la_SOURCES = \
	cut-ui-factory-gtk.c
gtk_factory_la_LIBADD = $(GTK_LIBS)
endif
