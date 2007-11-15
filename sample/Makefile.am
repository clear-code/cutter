SUBDIRS = glib

if HAVE_GTK
SUBDIRS += gtk+
endif

if HAVE_CAIRO
SUBDIRS += cairo
endif

dist_pkgdata_DATA = calc/Makefile.test

EXTRA_DIST = calc
dist-hook:
	rm -rf `find $(distdir)/calc -name .svn`
