AM_CPPFLAGS = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)	\
	 -I$(top_srcdir)/cutter

AM_CFLAGS = 			\
	$(CUTTER_CFLAGS)	\
	$(GDK_PIXBUF_CFLAGS)	\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

if HAVE_GDK_PIXBUF
lib_LTLIBRARIES = libgdkcutter-pixbuf.la

pkginclude_HEADERS = 		\
	gdkcutter-pixbuf.h

gdkcutter_pixbuf_includedir=$(pkgincludedir)/gdkcutter-pixbuf
gdkcutter_pixbuf_include_HEADERS = 		\
	$(gdkcutter_pixbuf_public_headers)
endif

gdkcutter_pixbuf_public_headers =		\
	gdkcut-pixbuf-assertions.h		\
	gdkcut-pixbuf-assertions-helper.h	\
	gdkcut-pixbuf.h

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

echo-public-headers:
	@for header in $(gdkcutter_pixbuf_include_HEADERS); do	\
	  echo $(abs_srcdir)/$${header};			\
	done
