ACLOCAL_AMFLAGS = -I m4macros $$ACLOCAL_OPTIONS

SUBDIRS = 			\
	m4macros		\
	gcutter			\
	cutter			\
	gdkcutter-pixbuf	\
	module			\
	test			\
	sample			\
	gst-plugins		\
	po			\
	misc			\
	license			\
	doc			\
	html

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = 	\
	cutter.pc	\
	gcutter.pc	\
	libcutter.pc

if HAVE_GDK_PIXBUF
pkgconfig_DATA += gdkcutter-pixbuf.pc
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

release: dist
	$(srcdir)/misc/release.rb				\
	  $(SF_USER) $(SF_PROJECT_NAME) $(PACKAGE_NAME)		\
	  $(VERSION) $(PACKAGE)-$(VERSION).tar.gz       	\
	  README NEWS

upload-doc:
	cd html && $(MAKE) $(AM_MAKEFLAGS) upload
	cd doc/reference && $(MAKE) $(AM_MAKEFLAGS) upload

update-po:
	cd $(top_srcdir) &&			\
	  (find cutter -name '*.c';		\
	   find gcutter -name '*.c';		\
	   find gdkcutter-pixbuf -name '*.c';	\
	   find module -name '*.c') |		\
	  grep -v cut-marshalers.c |		\
	  sort > po/POTFILES.in
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po
	cd doc/reference && $(MAKE) $(AM_MAKEFLAGS) update-po

tag:
	svn ls $(SF_REPOS)/tags/$(VERSION) > /dev/null 2>&1; 	\
	  if test "$$?" = "0"; then				\
	    echo "$(VERSION) is already tagged";		\
	    exit 1;						\
	  fi
	svn cp -m "released $(VERSION)!!!" $(SF_USER_OPTION) \
	  $(SF_REPOS)/trunk $(SF_REPOS)/tags/$(VERSION)
