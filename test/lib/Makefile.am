INCLUDES = 			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

AM_CFLAGS = $(CUTTER_CFLAGS)
if HAVE_GTK
AM_CFLAGS += $(GTK_CFLAGS)
endif

EXTRA_DIST = cuttest-assertions.h

noinst_LTLIBRARIES = 		\
	libcuttest_utils.la

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

# We just need to add -rpath option to make .so. We use
# $(libdir) here but we can use other absolute path like
# $(abs_builddir).  $(abs_builddir) is more reasonable
# absolute path but it's not supported automake < 1.10. :<
AM_LDFLAGS =			\
	-rpath $(libdir)

libcuttest_utils_la_SOURCES = 		\
	cuttest-utils.c			\
	cuttest-utils.h
