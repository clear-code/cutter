SUBDIRS = glib

if HAVE_GTK
SUBDIRS += gtk+
endif

if HAVE_CAIRO
SUBDIRS += cairo
endif

stackdir = $(pkgdatadir)/stack
stack_DATA =			\
	stack/autogen.sh	\
	stack/Makefile.am	\
	stack/configure.ac

stack_srcdir = $(pkgdatadir)/stack/src
stack_srcDATA =			\
	stack/src/Makefile.am	\
	stack/src/stack.c	\
	stack/src/stack.h

stack_testdir = $(pkgdatadir)/stack/test
stack_testDATA =		\
	stack/test/Makefile.am	\
	stack/test/run-test.sh	\
	stack/test/test-stack.c

EXTRA_DIST = $(stack_DATA) $(stack_srcDATA) $(stack_testDATA)
