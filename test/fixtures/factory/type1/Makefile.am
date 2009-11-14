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

module1_factory_la_SOURCES = module1-factory.c
module2_factory_la_SOURCES = module2-factory.c
module3_factory_la_SOURCES = module3-factory.c

echo-tests:
	@echo $(noinst_LTLIBRARIES)
