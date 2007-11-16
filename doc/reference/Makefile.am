MAINTAINERCLEANFILE = Makefile.in

# The name of the module.
DOC_MODULE=cutter

# The top-level SGML file.
DOC_MAIN_ORIGINAL_SGML_FILE=cutter-docs-original.sgml
DOC_MAIN_SGML_FILE=cutter-docs.sgml

$(DOC_MAIN_SGML_FILE): $(DOC_MAIN_ORIGINAL_SGML_FILE)
	cp -p $(DOC_MAIN_ORIGINAL_SGML_FILE) $(DOC_MAIN_SGML_FILE)

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS=

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../../cutter

# Used for dependencies
HFILE_GLOB=$(top_srcdir)/cutter/cut-assertions.h
#CFILE_GLOB=$(top_srcdir)/cutter/*.c

# Headers to ignore
IGNORE_HFILES=			\
	config.h

INCLUDES = \
	-I$(top_srcdir)/cutter 		\
	-I$(top_builddir)/cutter	\
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
	version.xml			\
	$(DOC_MAIN_ORIGINAL_SGML_FILE)	\
	$(DOC_MODULE).pot

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

# include common portion ...
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
EXTRA_DIST +=				\
	version.xml.in

$(DOC_MODULE).pot: sgml-build.stamp
	xml2po -k -o $(DOC_MODULE).pot \
	  $(srcdir)/$(DOC_MAIN_ORIGINAL_SGML_FILE) $(srcdir)/xml/*.xml

ja.po: $(DOC_MODULE).pot
	rm -f .xml2po.mo
	xml2po -k -u ja.po \
	  $(srcdir)/$(DOC_MAIN_ORIGINAL_SGML_FILE) $(srcdir)/xml/*.xml

sgml-ja.stamp: sgml-build.stamp ja.po
	for x in $(srcdir)/$(DOC_MAIN_SGML_FILE) $(srcdir)/xml/*.xml;	\
	do								\
	  mv $$x $$x.orig;						\
	  xml2po -k -p ja.po -l ja $$x.orig > $$x;			\
	  rm -f $$x.orig;						\
	done

html-build-ja.stamp: sgml-ja.stamp html-build.stamp
	rm -rf $(srcdir)/ja/html
	mkdir -p $(srcdir)/ja
	mv $(srcdir)/html $(srcdir)/ja/
	rm -f html-build.stamp
	rm -f sgml-build.stamp
	touch html-build-ja.stamp
