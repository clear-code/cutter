AM_CPPFLAGS =					\
	 -I$(top_builddir)			\
	 -I$(top_srcdir)

AM_CFLAGS =					\
	$(GLIB_CFLAGS)				\
	$(CUTTER_CFLAGS)			\
	$(CPPCUTTER_CFLAGS)			\
	$(COVERAGE_CFLAGS)

AM_LDFLAGS =					\
	-avoid-version				\
	-module					\
	-no-undefined				\
	-export-dynamic				\
	$(LIBTOOL_EXPORT_OPTIONS)

LIBS =							\
	$(GLIB_LIBS)					\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/cppcutter/libcppcutter.la

loader_customizer_module_LTLIBRARIES =		\
	exception.la
loader_customizer_factory_module_LTLIBRARIES =	\
	exception-factory.la

exception_la_SOURCES =				\
	cut-exception-loader-customizer.c

exception_factory_la_SOURCES =				\
	cut-exception-loader-customizer-factory.c
