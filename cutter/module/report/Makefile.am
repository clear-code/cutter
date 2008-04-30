INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS =			\
	$(GLIB_CFLAGS)		\
	$(COVERAGE_CFLAGS)

LDFLAGS =							\
  -avoid-version -module					\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)

report_module_LTLIBRARIES = 	\
		xml.la		\
		pdf.la

report_factory_module_LTLIBRARIES =	\
		xml_factory.la		\
		pdf_factory.la

xml_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/Report:XML\"
xml_la_SOURCES = cut-report-xml.c
xml_la_LIBADD = $(GLIB_LIBS)

xml_factory_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/ReportFactory:XML\"
xml_factory_la_SOURCES = cut-report-factory-xml.c
xml_factory_la_LIBADD = $(GLIB_LIBS)

if PDF_REPORT_AVAILABLE
pdf_la_CPPFLAGS =				\
	-DG_LOG_DOMAIN=\"Cut/Report:PDF\"	\
	$(CAIRO_PDF_CFLAGS)			\
	$(PANGO_CAIRO_CFLAGS)
pdf_la_SOURCES = 		\
	cut-report-pdf.c 	\
	cut-cairo.c		\
	cut-cairo.h		\
	cut-cairo-pie-chart.c	\
	cut-cairo-pie-chart.h	\
	cut-cairo-chart-data.c	\
	cut-cairo-chart-data.h
pdf_la_LIBADD =			\
	$(GLIB_LIBS)		\
	$(CAIRO_PDF_LIBS)	\
	$(PANGO_CAIRO_LIBS)

pdf_factory_la_CPPFLAGS = -DG_LOG_DOMAIN=\"Cut/ReportFactory:PDF\"
pdf_factory_la_SOURCES = cut-report-factory-pdf.c
pdf_factory_la_LIBADD = $(GLIB_LIBS)
endif
