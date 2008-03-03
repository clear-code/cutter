INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 			\
		module1_factory.la	\
		module2_factory.la	\
		module3_factory.la

LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

module1_factory_la_SOURCES = factory-module1.c
module2_factory_la_SOURCES = factory-module2.c
module3_factory_la_SOURCES = factory-module3.c

check_DATA = 			\
	module1_factory.so	\
	module2_factory.so	\
	module3_factory.so

module1_factory.so: .libs/module1_factory.so
	ln -s $< $@

module2_factory.so: .libs/module2_factory.so
	ln -s $< $@

module3_factory.so: .libs/module3_factory.so
	ln -s $< $@

CLEANFILES = 			\
	module1_factory.so	\
	module2_factory.so	\
	module3_factory.so

