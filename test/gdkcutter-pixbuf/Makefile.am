AM_CPPFLAGS = 					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter			\
	-I$(top_srcdir)/gdkcutter-pixbuf

AM_CFLAGS = $(CUTTER_CFLAGS)
AM_CFLAGS += $(GDK_PIXBUF_CFLAGS)

CLEANFILES = *.gcno *.gcda

if HAVE_GDK_PIXBUF
check_LTLIBRARIES = 				\
	test-gdkcut-pixbuf-assertions.la	\
	test-gdkcut-pixbuf.la
endif

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version		\
	-no-undefined

LIBS =								\
	$(top_builddir)/cutter/libcutter.la			\
	$(top_builddir)/gdkcutter-pixbuf/libgdkcutter-pixbuf.la	\
	$(top_builddir)/test/lib/libcuttest-utils.la		\
	$(GLIB_LIBS)						\
	$(GDK_PIXBUF_LIBS)

test_gdkcut_pixbuf_assertions_la_SOURCES = test-gdkcut-pixbuf-assertions.c
test_gdkcut_pixbuf_la_SOURCES = test-gdkcut-pixbuf.c
