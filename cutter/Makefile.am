INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

EXTRA_DIST = config.h.in

pkginclude_HEADERS = cutter.h assersions.h

bin_PROGRAMS = cutter

cutter_SOURCES =			\
		cut-test-container.h	\
		cut-test-container.c	\
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
