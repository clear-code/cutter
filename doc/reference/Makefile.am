MAINTAINERCLEANFILE = Makefile.in

# The name of the module.
DOC_MODULE=cutter

# The top-level SGML file.
DOC_MAIN_SGML_FILE=cutter-docs.sgml

SOURCE_DIR_OPTIONS=					\
	--source-dir="$(top_srcdir)/gcutter"		\
	--source-dir="$(top_srcdir)/cppcutter"		\
	--source-dir="$(top_srcdir)/gdkcutter-pixbuf"	\
	--source-dir="$(top_srcdir)/soupcutter"		\
	`test "$(top_builddir)" != "$(top_srcdir)" &&	\
	   echo --source-dir="$(top_builddir)/cutter"`

GTKDOC_CFLAGS =						\
	$(GLIB_CFLAGS)

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS =						\
	--deprecated-guards="CUTTER_DISABLE_DEPRECATED"	\
	--rebuild-types					\
	$(SOURCE_DIR_OPTIONS)

# Extra options to supply to gtkdoc-scangobj
SCANGOBJ_OPTIONS =					\
	--types="$(srcdir)/$(DOC_MODULE).types"

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../../cutter

# Used for dependencies
HFILE_GLOB =								\
	$(top_builddir)/cutter/cut-version.h				\
	$(top_srcdir)/cutter/cut-assertions.h				\
	$(top_srcdir)/cutter/cut-experimental.h				\
	$(top_srcdir)/cutter/cut-helper.h				\
	$(top_srcdir)/cutter/cut-multi-process.h			\
	$(top_srcdir)/cutter/cut-test-utils.h				\
	$(top_srcdir)/cutter/cut-types.h				\
	$(top_srcdir)/cutter/cut-features.h				\
	$(top_srcdir)/cutter/cutter.h					\
	$(top_srcdir)/gcutter/gcut-assertions.h				\
	$(top_srcdir)/gcutter/gcut-data.h				\
	$(top_srcdir)/gcutter/gcut-egg.h				\
	$(top_srcdir)/gcutter/gcut-enum.h				\
	$(top_srcdir)/gcutter/gcut-error.h				\
	$(top_srcdir)/gcutter/gcut-hash-table.h				\
	$(top_srcdir)/gcutter/gcut-inspect.h				\
	$(top_srcdir)/gcutter/gcut-list.h				\
	$(top_srcdir)/gcutter/gcut-object.h				\
	$(top_srcdir)/gcutter/gcut-process.h				\
	$(top_srcdir)/gcutter/gcut-test-utils.h				\
	$(top_srcdir)/gcutter/gcut-value-equal.h			\
	$(top_srcdir)/gcutter/gcutter.h					\
	$(top_srcdir)/cppcutter/cppcut-assertions.h			\
	$(top_srcdir)/cppcutter/cppcutter.h				\
	$(top_srcdir)/gdkcutter-pixbuf/gdkcut-pixbuf-assertions.h	\
	$(top_srcdir)/gdkcutter-pixbuf/gdkcut-pixbuf.h			\
	$(top_srcdir)/gdkcutter-pixbuf/gdkcutter-pixbuf.h		\
	$(top_srcdir)/soupcutter/soupcut-assertions.h			\
	$(top_srcdir)/soupcutter/soupcut-client.h			\
	$(top_srcdir)/soupcutter/soupcut-server.h			\
	$(top_srcdir)/soupcutter/soupcutter.h

#CFILE_GLOB=$(top_srcdir)/cutter/*.c

# Headers to ignore
IGNORE_HFILES=					\
	cut-analyzer.h				\
	cut-assertions-helper.h			\
	cut-backtrace-entry.h			\
	cut-context.h				\
	cut-contractor.h			\
	cut-crash-backtrace.h			\
	cut-diff.h				\
	cut-diff-writer.h			\
	cut-differ.h				\
	cut-colorize-differ.h			\
	cut-readable-differ.h			\
	cut-unified-differ.h			\
	cut-string-differ.h			\
	cut-elf-loader.h			\
	cut-enum-types.h			\
	cut-factory-builder.h			\
	cut-file-stream-reader.h		\
	cut-gassertions-helper.h		\
	cut-gpublic.h				\
	cut-hidden-definitions.h		\
	cut-iterated-test.h			\
	cut-listener-utils.h			\
	cut-listener.h				\
	cut-loader.h				\
	cut-mach-o-loader.h			\
	cut-macros.h				\
	cut-main.h				\
	cut-marshalers.h			\
	cut-module-factory-utils.h		\
	cut-module-factory.h			\
	cut-module-impl.h			\
	cut-module.h				\
	cut-pe-loader.h				\
	cut-pipeline.h				\
	cut-private.h				\
	cut-process.h				\
	cut-public.h				\
	cut-report-factory-builder.h		\
	cut-report.h				\
	cut-repository.h			\
	cut-run-context.h			\
	cut-runner.h				\
	cut-sequence-matcher.h			\
	cut-stream-factory-builder.h		\
	cut-stream-parser.h			\
	cut-stream-reader.h			\
	cut-stream.h				\
	cut-sub-process-group.h			\
	cut-sub-process-group.h			\
	cut-sub-process.h			\
	cut-test-case.h				\
	cut-test-container.h			\
	cut-test-context.h			\
	cut-test-data.h				\
	cut-test-iterator.h			\
	cut-test-result.h			\
	cut-test-runner.h			\
	cut-test-suite.h			\
	cut-test-utils-helper.h			\
	cut-test.h				\
	cut-ui-factory-builder.h		\
	cut-ui.h				\
	cut-utils.h				\
	cut-verbose-level.h			\
	gcut-dynamic-data.h			\
	cppcut-assertions-helper.h		\
	cppcut-message.h			\
	gbsearcharray.h

AM_CPPFLAGS =					\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter			\
	-I$(top_srcdir)/cppcutter		\
	-I$(top_srcdir)/gdkcutter-pixbuf	\
	-I$(top_srcdir)/soupcutter		\
	-I$(top_srcdir)				\
	-I$(top_builddir)/cutter		\
	$(GLIB_CFLAGS)

GTKDOC_LIBS =						\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/cppcutter/libcppcutter.la	\
	$(GLIB_LIBS)
if HAVE_GDK_PIXBUF
GTKDOC_LIBS +=							\
	$(top_builddir)/gdkcutter-pixbuf/libgdkcutter-pixbuf.la	\
	$(GDK_PIXBUF_LIBS)
endif
if HAVE_LIBSOUP
GTKDOC_LIBS +=						\
	$(top_builddir)/soupcutter/libsoupcutter.la	\
	$(LIBSOUP_LIBS)
endif

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS =						\
	--sgml-mode					\
	$(SOURCE_DIR_OPTIONS)

# Images to copy into HTML directory
HTML_IMAGES =				\
	dark-circle.png			\
	nested-circle.png		\
	diff-dark-and-nested-circle.png	\
	colorized-diff.png

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
content_files =					\
	version.xml				\
	readme.xml				\
	news.xml				\
	tutorial.xml				\
	features.xml				\
	users.xml				\
	cutter.xml				\
	cut-diff.xml				\
	install-to-debian.xml			\
	install-to-ubuntu.xml			\
	install-to-fedora.xml			\
	install-to-centos.xml			\
	install-to-freebsd.xml			\
	install-to-solaris.xml			\
	install-to-mac-os-x.xml			\
	install-to-cygwin.xml			\
	install-to-others.xml			\
	travis-ci-integration.xml		\
	release.xml

# Extra options to supply to gtkdoc-fixxref
FIXXREF_OPTIONS=

CATALOGS=ja.po

# include common portion ...
include $(top_srcdir)/gtk-doc.make

generated_xml_files =				\
	readme.xml				\
	readme.xml.ja				\
	news.xml				\
	news.xml.ja				\
	tutorial.xml				\
	tutorial.xml.ja				\
	features.xml				\
	features.xml.ja				\
	users.xml				\
	users.xml.ja				\
	cutter.xml				\
	cut-diff.xml.ja				\
	install-to-debian.xml			\
	install-to-debian.xml.ja		\
	install-to-ubuntu.xml			\
	install-to-ubuntu.xml.ja		\
	install-to-fedora.xml			\
	install-to-fedora.xml.ja		\
	install-to-centos.xml			\
	install-to-centos.xml.ja		\
	install-to-freebsd.xml			\
	install-to-freebsd.xml.ja		\
	install-to-solaris.xml			\
	install-to-solaris.xml.ja		\
	install-to-mac-os-x.xml			\
	install-to-mac-os-x.xml.ja		\
	install-to-cygwin.xml			\
	install-to-cygwin.xml.ja		\
	install-to-others.xml			\
	install-to-others.xml.ja		\
	travis-ci-integration.xml		\
	travis-ci-integration.xml.ja		\
	release.xml				\
	release.xml.ja

CLEANFILES +=				\
	$(DOC_MODULE)-sections.txt	\
	$(generated_xml_files)

# Other files to distribute
EXTRA_DIST +=				\
	html-build.stamp		\
	version.xml.in			\
	$(generated_xml_files)		\
	sf-logo.html			\
	overrides.txt			\
	ja.po

$(DOC_MODULE)-overrides.txt: $(srcdir)/overrides.txt
	cp $< $@


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

RD2LIB_DIR = $(top_srcdir)/misc
RD2REFENTRY_LIB_FILE = rd2refentry-lib.rb
RD2REFENTRY_LIB = $(RD2LIB_DIR)/$(RD2REFENTRY_LIB_FILE)
RD2DOCBOOK = $(RUBY) -Eutf-8 -I $(RD2LIB_DIR) $(RD2) -r $(RD2REFENTRY_LIB_FILE) --name=CUTTER
RD2DOCBOOK_JA = $(RD2DOCBOOK) --po=$(srcdir)/ja.po

news.xml: $(top_srcdir)/NEWS $(RD2REFENTRY_LIB) news.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/NEWS > news.xml

news.xml.ja: $(top_srcdir)/NEWS.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/NEWS.ja > news.xml.ja

readme.xml: $(top_srcdir)/README $(RD2REFENTRY_LIB) readme.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/README > readme.xml

readme.xml.ja: $(top_srcdir)/README.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/README.ja > readme.xml.ja

tutorial.xml: $(top_srcdir)/TUTORIAL $(RD2REFENTRY_LIB) tutorial.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/TUTORIAL > tutorial.xml

tutorial.xml.ja: $(top_srcdir)/TUTORIAL.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/TUTORIAL.ja > tutorial.xml.ja

features.xml: $(top_srcdir)/FEATURES $(RD2REFENTRY_LIB) features.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/FEATURES > features.xml

features.xml.ja: $(top_srcdir)/FEATURES.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/FEATURES.ja > features.xml.ja

users.xml: $(top_srcdir)/USERS $(RD2REFENTRY_LIB) users.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/USERS > users.xml

users.xml.ja: $(top_srcdir)/USERS.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/USERS.ja > users.xml.ja

cutter.xml: $(top_srcdir)/doc/cutter.rd $(RD2REFENTRY_LIB) cutter.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/cutter.rd > cutter.xml

cutter.xml.ja: $(top_srcdir)/doc/cutter.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/cutter.rd.ja > cutter.xml.ja

cut-diff.xml: $(top_srcdir)/doc/cut-diff.rd $(RD2REFENTRY_LIB) cut-diff.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/cut-diff.rd > cut-diff.xml

cut-diff.xml.ja: $(top_srcdir)/doc/cut-diff.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/cut-diff.rd.ja > cut-diff.xml.ja

install-to-debian.xml: $(top_srcdir)/doc/install-to-debian.rd $(RD2REFENTRY_LIB) install-to-debian.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-debian.rd > install-to-debian.xml

install-to-debian.xml.ja: $(top_srcdir)/doc/install-to-debian.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-debian.rd.ja > install-to-debian.xml.ja

install-to-ubuntu.xml: $(top_srcdir)/doc/install-to-ubuntu.rd $(RD2REFENTRY_LIB) install-to-ubuntu.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-ubuntu.rd > install-to-ubuntu.xml

install-to-ubuntu.xml.ja: $(top_srcdir)/doc/install-to-ubuntu.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-ubuntu.rd.ja > install-to-ubuntu.xml.ja

install-to-fedora.xml: $(top_srcdir)/doc/install-to-fedora.rd $(RD2REFENTRY_LIB) install-to-fedora.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-fedora.rd > install-to-fedora.xml

install-to-fedora.xml.ja: $(top_srcdir)/doc/install-to-fedora.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-fedora.rd.ja > install-to-fedora.xml.ja

install-to-centos.xml: $(top_srcdir)/doc/install-to-centos.rd $(RD2REFENTRY_LIB) install-to-centos.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-centos.rd > install-to-centos.xml

install-to-centos.xml.ja: $(top_srcdir)/doc/install-to-centos.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-centos.rd.ja > install-to-centos.xml.ja

install-to-freebsd.xml: $(top_srcdir)/doc/install-to-freebsd.rd $(RD2REFENTRY_LIB) install-to-freebsd.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-freebsd.rd > install-to-freebsd.xml

install-to-freebsd.xml.ja: $(top_srcdir)/doc/install-to-freebsd.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-freebsd.rd.ja > install-to-freebsd.xml.ja

install-to-solaris.xml: $(top_srcdir)/doc/install-to-solaris.rd $(RD2REFENTRY_LIB) install-to-solaris.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-solaris.rd > install-to-solaris.xml

install-to-solaris.xml.ja: $(top_srcdir)/doc/install-to-solaris.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-solaris.rd.ja > install-to-solaris.xml.ja

install-to-mac-os-x.xml: $(top_srcdir)/doc/install-to-mac-os-x.rd $(RD2REFENTRY_LIB) install-to-mac-os-x.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-mac-os-x.rd > install-to-mac-os-x.xml

install-to-mac-os-x.xml.ja: $(top_srcdir)/doc/install-to-mac-os-x.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-mac-os-x.rd.ja > install-to-mac-os-x.xml.ja

install-to-cygwin.xml: $(top_srcdir)/doc/install-to-cygwin.rd $(RD2REFENTRY_LIB) install-to-cygwin.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-cygwin.rd > install-to-cygwin.xml

install-to-cygwin.xml.ja: $(top_srcdir)/doc/install-to-cygwin.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-cygwin.rd.ja > install-to-cygwin.xml.ja

install-to-others.xml: $(top_srcdir)/doc/install-to-others.rd $(RD2REFENTRY_LIB) install-to-others.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-others.rd > install-to-others.xml

install-to-others.xml.ja: $(top_srcdir)/doc/install-to-others.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-others.rd.ja > install-to-others.xml.ja

travis-ci-integration.xml: $(top_srcdir)/doc/travis-ci-integration.rd $(RD2REFENTRY_LIB) travis-ci-integration.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/travis-ci-integration.rd > travis-ci-integration.xml

travis-ci-integration.xml.ja: $(top_srcdir)/doc/travis-ci-integration.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/travis-ci-integration.rd.ja > travis-ci-integration.xml.ja

release.xml: $(top_srcdir)/doc/release.rd.ja $(RD2REFENTRY_LIB) ja.po release.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/release.rd.ja > release.xml

release.xml.ja: $(top_srcdir)/doc/release.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/release.rd.ja > release.xml.ja

upload: docs
	rm -rf tmp/
	mkdir -p tmp/
	cp -rp html/ tmp/reference
	for catalog in $(CATALOGS); do					\
	  lang=`echo $$catalog | sed 's/.po$$//'`;			\
	  cp -rp $$lang/html/ tmp/reference/$$lang/;			\
	done
	chmod -R g+w tmp/reference
	ruby $(top_srcdir)/misc/prepare-gtk-doc-html-for-publishing.rb	\
	  --template-directory=$(srcdir) --title "$(PACKAGE_TITLE)"	\
	  tmp/reference/*.html
	ruby $(top_srcdir)/misc/prepare-gtk-doc-html-for-publishing.rb	\
	  --template-directory=$(srcdir) --title "$(PACKAGE_TITLE)"	\
	  --language=ja tmp/reference/ja/*.html
	rsync -avz --delete tmp/reference/ $(SF_HTDOCS)/reference/
