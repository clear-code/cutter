INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

LDFLAGS =							\
  -rpath $(ui_moduledir) -avoid-version -module			\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

ui_module_LTLIBRARIES = console.la

console_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/UI:Console\"
console_la_SOURCES = \
	cut-ui-console.c
console_la_LIBADD = $(GLIB_LIBS)

check_DATA = console.so
console.so: .libs/console.so
	ln -s $< $@
CLEANFILES = console.so

if HAVE_GTK
ui_module_LTLIBRARIES += gtk.la
gtk_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/UI:Gtk\" \
		$(GTK_CFLAGS)
gtk_la_SOURCES = \
	cut-ui-gtk.c
gtk_la_LIBADD = $(GTK_LIBS)

check_DATA += gtk.so
gtk.so: .libs/gtk.so
	ln -s $< $@
CLEANFILES += gtk.so
endif
