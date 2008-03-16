INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

LDFLAGS =							\
  -avoid-version -module					\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

ui_module_LTLIBRARIES = console.la
ui_factory_module_LTLIBRARIES = console_factory.la

console_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/UI:Console\"
console_la_SOURCES = \
	cut-ui-console.c
console_la_LIBADD = $(GLIB_LIBS)

console_factory_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/UIFactory:Console\"
console_factory_la_SOURCES = \
	cut-ui-factory-console.c
console_factory_la_LIBADD = $(GLIB_LIBS)

check_DATA = console.so
console.so: .libs/console.so
	ln -s $< $@
CLEANFILES = console.so

if HAVE_GTK
ui_module_LTLIBRARIES += gtk.la
ui_factory_module_LTLIBRARIES += gtk_factory.la

gtk_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/UI:Gtk\" \
		$(GTK_CFLAGS)
gtk_la_SOURCES = \
	cut-ui-gtk.c
gtk_la_LIBADD = $(GTK_LIBS)

gtk_factory_la_CPPFLAGS =			\
	-DG_LOG_DOMAIN=\"Cut/UIFactory:Gtk\"	\
	$(GTK_CFLAGS)
gtk_factory_la_SOURCES = \
	cut-ui-factory-gtk.c
gtk_factory_la_LIBADD = $(GTK_LIBS)

check_DATA += gtk.so
gtk.so: .libs/gtk.so
	ln -s $< $@
CLEANFILES += gtk.so
endif
