AM_CPPFLAGS = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS =			\
	$(GLIB_CFLAGS)		\
	$(COVERAGE_CFLAGS)

AM_LDFLAGS =							\
  -avoid-version -module					\
  -no-undefined -export-dynamic $(LIBTOOL_EXPORT_OPTIONS)

report_module_LTLIBRARIES = 	\
	xml.la

report_factory_module_LTLIBRARIES =	\
	xml_factory.la

LIBS =						\
	$(GLIB_LIBS)				\
	$(top_builddir)/cutter/libcutter.la

xml_la_CPPFLAGS =
xml_la_SOURCES = cut-xml-report.c

xml_factory_la_CPPFLAGS =
xml_factory_la_SOURCES = cut-xml-report-factory.c

if PDF_REPORT_AVAILABLE
report_module_LTLIBRARIES += pdf.la
report_factory_module_LTLIBRARIES += pdf_factory.la

pdf_la_CFLAGS =			\
	$(CAIRO_PDF_CFLAGS)	\
	$(PANGO_CAIRO_CFLAGS)	\
	$(GOFFICE_CFLAGS)	\
	$(AM_CFLAGS)

pdf_la_SOURCES = 		\
	cut-pdf-report.c 	\
	cut-cairo.c		\
	cut-cairo.h		\
	cut-cairo-pie-chart.c	\
	cut-cairo-pie-chart.h	\
	cut-cairo-chart-data.c	\
	cut-cairo-chart-data.h

pdf_la_LIBADD =			\
	$(CAIRO_PDF_LIBS)	\
	$(PANGO_CAIRO_LIBS)	\
	$(GOFFICE_LIBS)		\
	-lm


pdf_factory_la_CFLAGS =		\
	$(GOFFICE_CFLAGS)	\
	$(AM_CFLAGS)

pdf_factory_la_LIBADD =		\
	$(GOFFICE_LIBS)

pdf_factory_la_SOURCES = cut-pdf-report-factory.c
endif
