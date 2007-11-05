INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

EXTRA_DIST = config.h.in

pkginclude_HEADERS = cut-assertion.h cut.h

bin_PROGRAMS = cutter

cutter_SOURCES =			\
		cut-assertion.h		\
		cut-test.h		\
		cut-test.c		\
		cut-context-private.h	\
		cut-context.h		\
		cut-context.c		\
		cut-test-container.h	\
		cut-test-container.c	\
		cut-test-case.h		\
		cut-test-case.c		\
		cut-test-suite.h	\
		cut-test-suite.c	\
		cut-loader.h		\
		cut-loader.c		\
		cut-repository.h	\
		cut-repository.c	\
		cut-main.c
cutter_LDADD =				\
	$(GLIB_LIBS)
