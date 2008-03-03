INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

noinst_LTLIBRARIES = 			\
		factory_module1.la	\
		factory_module2.la	\
		factory_module3.la

LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

factory_module1_la_SOURCES = factory-module1.c
factory_module2_la_SOURCES = factory-module2.c
factory_module3_la_SOURCES = factory-module3.c

check_DATA = 			\
	factory_module1.so	\
	factory_module2.so	\
	factory_module3.so

factory_module1.so: .libs/factory_module1.so
	ln -s $< $@

factory_module2.so: .libs/factory_module2.so
	ln -s $< $@

factory_module3.so: .libs/factory_module3.so
	ln -s $< $@

CLEANFILES = 			\
	factory_module1.so	\
	factory_module2.so	\
	factory_module3.so

