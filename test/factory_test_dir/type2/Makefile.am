INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 			\
		factory_module1.la	\
		factory_module2.la	\
		factory_module3.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

factory_module1_la_SOURCES = factory-module1.c
factory_module2_la_SOURCES = factory-module2.c
factory_module3_la_SOURCES = factory-module2.c
