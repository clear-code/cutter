SUBDIRS = glib

if HAVE_GTK
SUBDIRS += gtk+
endif

if HAVE_CAIRO
SUBDIRS += cairo
endif


stackdir=$(pkgdatadir)/stack
stack_DATA =			\
	stack/autogen.sh	\
	stack/Makefile.am	\
	stack/configure.ac

EXTRA_DIST = stack
dist-hook:
	rm -rf `find $(stackdir) -name .svn`
