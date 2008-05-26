INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS =			\
	$(GLIB_CFLAGS)		\
	$(COVERAGE_CFLAGS)

LDFLAGS =							\
  -avoid-version -module					\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

ui_module_LTLIBRARIES = console.la
ui_factory_module_LTLIBRARIES = console_factory.la

console_la_CPPFLAGS =
console_la_SOURCES = \
	cut-console-ui.c
console_la_LIBADD = $(GLIB_LIBS)			\
		$(top_builddir)/cutter/libcutter.la

console_factory_la_CPPFLAGS =
console_factory_la_SOURCES = \
	cut-console-ui-factory.c
console_factory_la_LIBADD = $(GLIB_LIBS)

if HAVE_GTK
ui_module_LTLIBRARIES += gtk.la
ui_factory_module_LTLIBRARIES += gtk_factory.la

gtk_la_CPPFLAGS = \
		$(GTK_CFLAGS)
gtk_la_SOURCES = \
	cut-gtk-ui.c
gtk_la_LIBADD = $(GTK_LIBS)

gtk_factory_la_CPPFLAGS =			\
	$(GTK_CFLAGS)
gtk_factory_la_SOURCES = \
	cut-gtk-ui-factory.c
gtk_factory_la_LIBADD = $(GTK_LIBS)			\
		$(top_builddir)/cutter/libcutter.la
endif
