SUBDIRS = cutter test sample po misc doc html

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = 	\
	cutter.pc	\
	gcutter.pc	\
	libcutter.pc

EXTRA_DIST =		\
	autogen.sh	\
	TUTORIAL	\
	TUTORIAL.ja	\
	README.ja	\
	NEWS		\
	NEWS.ja

upload: upload-doc

release: $(PACKAGE)-$(VERSION).tar.gz
	$(srcdir)/misc/release.rb				\
	  $(SF_USER) $(SF_PROJECT_NAME) $(PACKAGE_TITLE)	\
	  $(VERSION) $(PACKAGE)-$(VERSION).tar.gz       	\
	  README NEWS

upload-doc:
	cd html && $(MAKE) $(AM_MAKEFLAGS) upload
	cd doc/reference && $(MAKE) $(AM_MAKEFLAGS) upload

update-po:
	cd $(top_srcdir) && find cutter -name '*.c' | sort > po/POTFILES.in
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

if ENABLE_COVERAGE
coverage: check
	lcov -d cutter/.libs -b cutter -c -o coverage.info && \
	  genhtml -o coverage coverage.info
endif
