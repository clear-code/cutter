ACLOCAL_AMFLAGS = -I acmacros -I m4macros ${ACLOCAL_OPTIONS}

SUBDIRS =			\
	m4macros		\
	glib-compatible		\
	gcutter			\
	cutter			\
	cppcutter		\
	gdkcutter-pixbuf	\
	soupcutter		\
	module			\
	data			\
	test			\
	sample			\
	gst-plugins		\
	po			\
	misc			\
	license			\
	doc			\
	html			\
	vcc			\
	packages

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =	\
	cutter.pc	\
	gcutter.pc	\
	cppcutter.pc	\
	libcutter.pc

if HAVE_GDK_PIXBUF
pkgconfig_DATA += gdkcutter-pixbuf.pc
endif

if HAVE_LIBSOUP
pkgconfig_DATA += soupcutter.pc
endif

EXTRA_DIST =		\
	autogen.sh	\
	config.h.in	\
	TUTORIAL	\
	TUTORIAL.ja	\
	README.ja	\
	NEWS		\
	NEWS.ja		\
	FEATURES	\
	FEATURES.ja	\
	USERS		\
	USERS.ja

upload: upload-doc

release: dist release-official release-debian

release-official:
	$(srcdir)/misc/release.rb				\
	  $(SF_USER) $(SF_PROJECT_ID) $(SF_PROJECT_NAME)	\
	  $(PACKAGE_NAME) $(VERSION) README NEWS		\
	  $(PACKAGE)-$(VERSION).tar.gz				\
	  $(PACKAGE)-$(VERSION).zip

release-debian:
	rm -rf tmp
	mkdir -p tmp
	tar xfz $(PACKAGE)-$(VERSION).tar.gz -C tmp
	mv tmp/$(PACKAGE)-$(VERSION) tmp/$(PACKAGE)-testing-framework-$(VERSION)
	tar cfz tmp/$(PACKAGE)-testing-framework-$(VERSION).tar.gz \
	  -C tmp \
	  $(PACKAGE)-testing-framework-$(VERSION)
	mkdir -p tmp/archives/debian/
	echo "Options +Indexes" > tmp/archives/.htaccess
	mv tmp/$(PACKAGE)-testing-framework-$(VERSION).tar.gz \
	  tmp/archives/debian/
	rsync -avz tmp/archives $(SF_HTDOCS)/

upload-doc:
	cd html && $(MAKE) upload
	cd doc/reference && $(MAKE) upload

update-po:
	cd $(top_srcdir) &&			\
	  (find cutter -name '*.c';		\
	   find gcutter -name '*.c';		\
	   find cppcutter -name '*.c';		\
	   find gdkcutter-pixbuf -name '*.c';	\
	   find soupcutter -name '*.c';		\
	   find module -name '*.c';		\
	   find glib-compatible -name '*.c') |	\
	  grep -v cut-marshalers.c |		\
	  sort > po/POTFILES.in
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po
	cd doc/reference && $(MAKE) $(AM_MAKEFLAGS) update-po

update-latest-release:
	@if test -z "$(OLD_RELEASE)"; then	\
	  echo "\$$(OLD_RELEASE) is missing";	\
	  exit 1;				\
	fi
	@if test -z "$(OLD_RELEASE_DATE)"; then		\
	  echo "\$$(OLD_RELEASE_DATE) is missing";	\
	  exit 1;					\
	fi
	@if test -z "$(NEW_RELEASE_DATE)"; then		\
	  echo "\$$(NEW_RELEASE_DATE) is missing";	\
	  exit 1;					\
	fi
	cd $(top_srcdir) &&					\
	  misc/update-latest-release.rb				\
	  $(PACKAGE)						\
	  $(OLD_RELEASE) $(OLD_RELEASE_DATE)			\
	  $(VERSION) $(NEW_RELEASE_DATE)			\
	  packages/rpm/fedora/cutter.spec.in			\
	  packages/rpm/centos/cutter.spec.in			\
	  packages/debian/changelog				\
	  html/index.html					\
	  html/index.html.ja					\
	  doc/reference/header.html				\
	  doc/reference/header-ja.html				\
	  doc/install-to-cygwin.rd				\
	  doc/install-to-cygwin.rd.ja				\
	  doc/install-to-freebsd.rd				\
	  doc/install-to-freebsd.rd.ja				\
	  doc/install-to-others.rd				\
	  doc/install-to-others.rd.ja				\
	  doc/install-to-solaris.rd				\
	  doc/install-to-solaris.rd.ja

tag:
	cd $(top_srcdir) &&					\
	  git tag $(VERSION) -a -m 'released $(VERSION)!!!' &&	\
	  git push --tags
