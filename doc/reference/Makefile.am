MAINTAINERCLEANFILE = Makefile.in

# The name of the module.
DOC_MODULE=cutter

# The top-level SGML file.
DOC_MAIN_SGML_FILE=cutter-docs.sgml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS =							\
	--deprecated-guards="CUTTER_DISABLE_DEPRECATED"

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../..

# Used for dependencies
HFILE_GLOB =								\
	$(top_srcdir)/cutter/cutter.h					\
	$(top_srcdir)/cutter/cut-assertions.h				\
	$(top_srcdir)/cutter/cut-test-utils.h				\
	$(top_srcdir)/cutter/cut-multi-process.h			\
	$(top_srcdir)/cutter/cut-types.h				\
	$(top_srcdir)/cutter/cut-experimental.h				\
	$(top_srcdir)/cutter/cut-helper.h				\
	$(top_srcdir)/gcutter/gcutter.h					\
	$(top_srcdir)/gcutter/gcut-assertions.h				\
	$(top_srcdir)/gcutter/gcut-test-utils.h				\
	$(top_srcdir)/gcutter/gcut-egg.h				\
	$(top_srcdir)/gcutter/gcut-enum.h				\
	$(top_srcdir)/gcutter/gcut-error.h				\
	$(top_srcdir)/gcutter/gcut-hash-table.h				\
	$(top_srcdir)/gcutter/gcut-list.h				\
	$(top_srcdir)/gcutter/gcut-object.h				\
	$(top_srcdir)/gcutter/gcut-value-equal.h			\
	$(top_srcdir)/gcutter/gcut-inspect.h				\
	$(top_srcdir)/gcutter/gcut-data.h				\
	$(top_srcdir)/gdkcutter-pixbuf/gdkcutter-pixbuf.h		\
	$(top_srcdir)/gdkcutter-pixbuf/gdkcut-pixbuf.h			\
	$(top_srcdir)/gdkcutter-pixbuf/gdkcut-pixbuf-assertions.h
#CFILE_GLOB=$(top_srcdir)/cutter/*.c

# Headers to ignore
IGNORE_HFILES=			\
	cut-assertions-helper.h	\
	cut-gassertions-helper.h\
	cut-context.h		\
	cut-enum-types.h	\
	cut-hidden-definitions.h\
	cut-loader.h		\
	cut-macros.h		\
	cut-marshalers.h	\
	cut-module-impl.h	\
	cut-module.h		\
	cut-private.h		\
	cut-public.h		\
	cut-gpublic.h		\
	cut-repository.h	\
	cut-sub-process.h	\
	cut-sub-process-group.h	\
	cut-test-case.h		\
	cut-test-context.h	\
	cut-test-container.h	\
	cut-test-result.h	\
	cut-test-suite.h	\
	cut-test.h		\
	cut-utils.h		\
	cut-verbose-level.h	\
	config.h

INCLUDES =					\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter			\
	-I$(top_srcdir)/gdkcutter-pixbuf	\
	-I$(top_srcdir)				\
	-I$(top_builddir)/cutter		\
	$(GLIB_CFLAGS)

GTKDOC_LIBS =					\
	$(top_builddir)/cutter/libcutter.la	\
	$(GLIB_LIBS)
if HAVE_GDK_PIXBUF
GTKDOC_LIBS +=							\
	$(top_builddir)/gdkcutter-pixbuf/libgdkcutter-pixbuf.la	\
	$(GDK_PIXBUF_LIBS)
endif

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Images to copy into HTML directory
HTML_IMAGES =				\
	dark-circle.png			\
	nested-circle.png		\
	diff-dark-and-nested-circle.png

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
content_files =		\
	version.xml	\
	readme.xml	\
	news.xml	\
	tutorial.xml	\
	features.xml	\
	users.xml

# Extra options to supply to gtkdoc-fixxref
FIXXREF_OPTIONS=

CATALOGS=ja.po

# include common portion ...
include $(top_srcdir)/gtk-doc.make

generated_xml_files =	\
	readme.xml	\
	readme.xml.ja	\
	news.xml	\
	news.xml.ja	\
	tutorial.xml	\
	tutorial.xml.ja	\
	features.xml	\
	features.xml.ja	\
	users.xml	\
	users.xml.ja

CLEANFILES +=				\
	$(DOC_MODULE)-sections.txt	\
	$(generated_xml_files)

# Other files to distribute
EXTRA_DIST +=				\
	html-build.stamp		\
	version.xml.in			\
	$(generated_xml_files)		\
	sf-logo.html			\
	ja.po

$(DOC_MODULE).pot: sgml-build.stamp
	xml2po -k -o $(DOC_MODULE).pot				\
	  $(srcdir)/$(DOC_MAIN_SGML_FILE) $(srcdir)/xml/*.xml

update-po: Makefile sgml.stamp
	$(MAKE) $(DOC_MODULE).pot
	for catalog in $(CATALOGS); do		\
	  xml2po -k -u $$catalog		\
	    $(srcdir)/$(DOC_MAIN_SGML_FILE)	\
	    $(srcdir)/xml/*.xml;		\
	done

RD2REFENTRY_LIB = $(top_srcdir)/misc/rd2refentry-lib.rb
RD2 = rd2 -r $(RD2REFENTRY_LIB) --name=CUTTER
RD2_JA = $(RD2) --po=$(srcdir)/ja.po

news.xml: $(top_srcdir)/NEWS $(RD2REFENTRY_LIB) news.xml.ja
	$(RD2) $(top_srcdir)/NEWS > news.xml

news.xml.ja: $(top_srcdir)/NEWS.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2_JA) $(top_srcdir)/NEWS.ja > news.xml.ja

readme.xml: $(top_srcdir)/README $(RD2REFENTRY_LIB) readme.xml.ja
	$(RD2) $(top_srcdir)/README > readme.xml

readme.xml.ja: $(top_srcdir)/README.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2_JA) $(top_srcdir)/README.ja > readme.xml.ja

tutorial.xml: $(top_srcdir)/TUTORIAL $(RD2REFENTRY_LIB) tutorial.xml.ja
	$(RD2) $(top_srcdir)/TUTORIAL > tutorial.xml

tutorial.xml.ja: $(top_srcdir)/TUTORIAL.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2_JA) $(top_srcdir)/TUTORIAL.ja > tutorial.xml.ja

features.xml: $(top_srcdir)/FEATURES $(RD2REFENTRY_LIB) features.xml.ja
	$(RD2) $(top_srcdir)/FEATURES > features.xml

features.xml.ja: $(top_srcdir)/FEATURES.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2_JA) $(top_srcdir)/FEATURES.ja > features.xml.ja

users.xml: $(top_srcdir)/USERS $(RD2REFENTRY_LIB) users.xml.ja
	$(RD2) $(top_srcdir)/USERS > users.xml

users.xml.ja: $(top_srcdir)/USERS.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2_JA) $(top_srcdir)/USERS.ja > users.xml.ja

upload: docs
	rm -rf tmp/
	mkdir -p tmp/
	cp -rp html/ tmp/reference
	for catalog in $(CATALOGS); do					\
	  lang=`echo $$catalog | sed 's/.po$$//'`;			\
	  cp -rp $$lang/html/ tmp/reference/$$lang/;			\
	done
	chmod -R g+w tmp/reference
	ruby -i'' -pe '$$_.sub!(/<\/body>/u,				\
	                       	File.read("sf-logo.html") + "</body>")'	\
	  tmp/reference/*.html						\
	  tmp/reference/*/*.html
	rsync -avz tmp/reference $(SF_HTDOCS)/reference/
	ssh $(SF_HOST) chmod -R g+w $(SF_HTDOCS_PATH)/reference/
