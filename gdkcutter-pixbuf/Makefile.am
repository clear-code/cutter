INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS = 			\
	$(CUTTER_CFLAGS)	\
	$(GDK_PIXBUF_CFLAGS)	\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

if HAVE_GDK_PIXBUF
lib_LTLIBRARIES = libgdkcutter-pixbuf.la
endif

gdkcutter_pixbuf_public_headers =		\
	gdkcut-pixbuf-assertions.h		\
	gdkcut-pixbuf-assertions-helper.h	\
	gdkcut-pixbuf.h

pkginclude_HEADERS = 		\
	gdkcutter-pixbuf.h

gdkcutter_pixbuf_includedir=$(includedir)/gdkcutter-pixbuf
gdkcutter_pixbuf_include_HEADERS = 		\
	$(gdkcutter_pixbuf_public_headers)

libgdkcutter_pixbuf_la_SOURCES =		\
	$(gcutter_public_headers)		\
	gdkcut-pixbuf-assertions-helper.c	\
	gdkcut-pixbuf.c

libgdkcutter_pixbuf_la_LDFLAGS =		\
	-version-info $(LT_VERSION_INFO)	\
	-no-undefined

libgdkcutter_pixbuf_la_LIBADD =			\
	$(top_builddir)/cutter/libcutter.la	\
	$(GDK_PIXBUF_LIBS)
