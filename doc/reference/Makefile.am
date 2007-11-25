MAINTAINERCLEANFILE = Makefile.in

# The name of the module.
DOC_MODULE=cutter

# The top-level SGML file.
DOC_MAIN_SGML_FILE=cutter-docs.sgml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS=

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../../cutter

# Used for dependencies
HFILE_GLOB=$(top_srcdir)/cutter/cutter.h \
	$(top_srcdir)/cutter/cut-assertions.h
#CFILE_GLOB=$(top_srcdir)/cutter/*.c

# Headers to ignore
IGNORE_HFILES=			\
	cut-assertions-helper.h	\
	cut-context.h		\
	cut-enum-types.h	\
	cut-hidden-definitions.h\
	cut-loader.h		\
	cut-marshalers.h	\
	cut-module-impl.h	\
	cut-module.h		\
	cut-private.h		\
	cut-public.h		\
	cut-repository.h	\
	cut-runner.h		\
	cut-test-case.h		\
	cut-test-context.h	\
	cut-test-container.h	\
	cut-test-result.h	\
	cut-test-suite.h	\
	cut-test.h		\
	cut-utils.h		\
	cut-verbose-level.h	\
	config.h

INCLUDES = \
	-I$(top_srcdir)/cutter 		\
	-I$(top_srcdir) 		\
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
	readme.xml			\
	news.xml			\
	tutorial.xml

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

CATALOGS=ja.po

# include common portion ...
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
EXTRA_DIST +=				\
	version.xml.in			\
	readme.xml			\
	readme.xml.ja			\
	news.xml			\
	news.xml.ja			\
	tutorial.xml			\
	tutorial.xml.ja			\
	sf-logo.html			\
	ja.po

$(DOC_MODULE).pot: sgml-build.stamp
	xml2po -k -o $(DOC_MODULE).pot \
	  $(srcdir)/$(DOC_MAIN_SGML_FILE) $(srcdir)/xml/*.xml

update-po: Makefile sgml.stamp
	$(MAKE) $(DOC_MODULE).pot
	for catalog in $(CATALOGS); do		\
	  xml2po -k -u $$catalog		\
	    $(srcdir)/$(DOC_MAIN_SGML_FILE)	\
	    $(srcdir)/xml/*.xml;		\
	done

RD2REFENTRY_LIB = $(top_srcdir)/misc/rd2refentry-lib.rb
news.xml: $(top_srcdir)/NEWS $(RD2REFENTRY_LIB) news.xml.ja
	rd2 -r $(RD2REFENTRY_LIB) $(top_srcdir)/NEWS > news.xml

news.xml.ja: $(top_srcdir)/NEWS.ja $(RD2REFENTRY_LIB)
	rd2 -r $(RD2REFENTRY_LIB) $(top_srcdir)/NEWS.ja > news.xml.ja

readme.xml: $(top_srcdir)/README $(RD2REFENTRY_LIB) readme.xml.ja
	rd2 -r $(RD2REFENTRY_LIB) $(top_srcdir)/README > readme.xml

readme.xml.ja: $(top_srcdir)/README.ja $(RD2REFENTRY_LIB)
	rd2 -r $(RD2REFENTRY_LIB) $(top_srcdir)/README.ja > readme.xml.ja

tutorial.xml: $(top_srcdir)/TUTORIAL $(RD2REFENTRY_LIB) tutorial.xml.ja
	rd2 -r $(RD2REFENTRY_LIB) $(top_srcdir)/TUTORIAL > tutorial.xml

tutorial.xml.ja: $(top_srcdir)/TUTORIAL.ja $(RD2REFENTRY_LIB)
	rd2 -r $(RD2REFENTRY_LIB) $(top_srcdir)/TUTORIAL.ja > tutorial.xml.ja

upload: docs
	ssh $(SF_HOST) rm -rf $(SF_HTDOCS_PATH)/reference
	scp -r $(srcdir)/html/ $(SF_HTDOCS)/reference/
	for catalog in $(CATALOGS); do					\
	  lang=`echo $$catalog | sed 's/.po$$//'`;			\
	  scp -r $(srcdir)/$$lang/html/ $(SF_HTDOCS)/reference/$$lang/;	\
	done
	ssh $(SF_HOST) chmod -R g+w $(SF_HTDOCS_PATH)/reference
	scp $(srcdir)/sf-logo.html $(SF_HOST):
	ssh $(SF_HOST) ruby -i ''					\
	   -pe "'\$$_.sub!(/<\\/body>/u,				\
	                    File.read(\"sf-logo.html\") + \"</body>\")'"\
	   '$(SF_HTDOCS_PATH)/reference/**/*.html'
