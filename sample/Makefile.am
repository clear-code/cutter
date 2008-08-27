SUBDIRS = glib

if HAVE_GTK
SUBDIRS += gtk+
endif

if HAVE_CAIRO
SUBDIRS += cairo
endif

stackdir = $(pkgdatadir)/stack

stack_SCRIPTS =			\
	stack/autogen.sh

stack_DATA =			\
	stack/Makefile.am	\
	stack/configure.ac

stack_srcdir = $(stackdir)/src
stack_src_DATA =		\
	stack/src/Makefile.am	\
	stack/src/stack.c	\
	stack/src/stack.h

stack_testdir = $(stackdir)/test

stack_test_SCRIPTS =		\
	stack/test/run-test.sh

stack_test_DATA =		\
	stack/test/Makefile.am	\
	stack/test/test-stack.c

stack_configdir = $(stackdir)/config
stack_config_DATA =		\
	stack/config/keep-me

EXTRA_DIST =			\
	$(stack_SCRIPTS)	\
	$(stack_DATA)		\
	$(stack_src_DATA)	\
	$(stack_test_SCRIPTS)	\
	$(stack_test_DATA)	\
	$(stack_config_DATA)
