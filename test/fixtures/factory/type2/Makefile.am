INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 			\
		factory-module1.la	\
		factory-module2.la	\
		factory-module3.la

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

echo-tests:
	@echo $(noinst_LTLIBRARIES)
