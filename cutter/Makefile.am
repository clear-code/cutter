INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

EXTRA_DIST = config.h.in

pkginclude_HEADERS = cutter.h assersions.h

bin_PROGRAMS = cutter cutter2

cutter_SOURCES =			\
		compile.c compile.h	\
		config.h 		\
		info.c info.h 		\
		load.c load.h 		\
		run.c run.h 		\
		show.c show.h 		\
		world.c world.h 	\
		cutter.h		\
		main.c
cutter_LDADD =				\
	$(GLIB_LIBS)

cutter2_SOURCES =			\
		cut-test.h		\
		cut-test.c		\
		cut-test-container.h	\
		cut-test-container.c	\
		cut-test-case.h		\
		cut-test-case.c		\
		cut-test-suite.h	\
		cut-test-suite.c	\
		cut-test-loader.h	\
		cut-test-loader.c	\
		cut-test-repository.h	\
		cut-test-repository.c	\
		cut-main.c
cutter2_LDADD =				\
	$(GLIB_LIBS)
