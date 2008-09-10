INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 		\
	module1-factory.la	\
	module2-factory.la	\
	module3-factory.la

AM_LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version		\
	-no-undefined

LIBS =						\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)

module1_factory_la_SOURCES = factory-module1.c
module2_factory_la_SOURCES = factory-module2.c
module3_factory_la_SOURCES = factory-module3.c
