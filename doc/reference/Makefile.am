MAINTAINERCLEANFILE = Makefile.in

## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = 1.6

# The name of the module.
DOC_MODULE=cutter

# The top-level SGML file.
DOC_MAIN_SGML_FILE=cutter-docs.sgml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS=

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../../cutter

# Used for dependencies
HFILE_GLOB=$(top_srcdir)/cutter/cut-assertions.h
#CFILE_GLOB=$(top_srcdir)/cutter/*.c

# Headers to ignore
IGNORE_HFILES=			\
	glib-utils.h		\
	config.h

INCLUDES = \
	-I$(top_srcdir)/cutter 		\
	-I$(top_srcdir) 		\
	-I$(top_builddir)/cutter	\
	-I$(top_builddir)		\
	$(GLIB_CFLAGS)
GTKDOC_LIBS = \
	$(top_builddir)/cutter/libcutter.la \
	$(GLIB_LIBS)

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS=--sgml-mode --output-format=xml --ignore-files=trio

# Images to copy into HTML directory
HTML_IMAGES =

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
content_files =				\
	version.xml

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

# include common portion ...
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
EXTRA_DIST +=				\
	version.xml.in
