INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

LDFLAGS =							\
  -rpath $(ui_factory_moduledir) -avoid-version -module	\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

ui_factory_module_LTLIBRARIES = console.la

console_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/RunnerFactory:Console\"
console_la_SOURCES = \
	cut-runner-factory-console.c
console_la_LIBADD = $(GLIB_LIBS)

if HAVE_GTK
ui_factory_module_LTLIBRARIES += gtk.la
gtk_la_CPPFLAGS =					\
	-DG_LOG_DOMAIN=\"Cut/RunnerFactory:Gtk\"	\
	$(GTK_CFLAGS)
gtk_la_SOURCES = \
	cut-runner-factory-gtk.c
gtk_la_LIBADD = $(GTK_LIBS)
endif
