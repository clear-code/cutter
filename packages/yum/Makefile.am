SERVER_PATH = $(SF_FRS)
DISTRIBUTIONS = fedora centos
ARCHITECTURES = i386 x86_64
CHROOT_BASE = /var/lib/chroot
BASE_URL_PREFIX = http://downloads.sourceforge.net/project/$(PACKAGE)
HAVE_DEVELOPMENT_BRANCH = no
USE_RPMFORGE = no
USE_ATRPMS = no

release: download build sign update upload

sign:
	./sign-rpm.sh '$(DISTRIBUTIONS)'

update:
	./update-repository.sh $(PACKAGE) '$(DISTRIBUTIONS)'

upload:
	for distribution in $(DISTRIBUTIONS); do		\
	  rsync -avz --delete					\
	    --exclude .gitignore --exclude .svn			\
	    $${distribution}/ $(SERVER_PATH)/$${distribution};	\
	done

download:
	for distribution in $(DISTRIBUTIONS); do		\
	  (rm -rf $${distribution} &&				\
	   mkdir -p $${distribution} &&				\
	   rsync -avz						\
	     $(SERVER_PATH)/$${distribution}/ $${distribution};	\
	  );							\
	done

build: build-in-chroot build-release-rpm

build-in-chroot: source
	./build-in-chroot.sh			\
	  $(PACKAGE)				\
	  $(VERSION)				\
	  $(top_builddir)/$(PACKAGE)		\
	  ../rpm				\
	  $(CHROOT_BASE)			\
	  '$(ARCHITECTURES)'			\
	  '$(DISTRIBUTIONS)'			\
	  '$(HAVE_DEVELOPMENT_BRANCH)'		\
	  '$(USE_RPMFORGE)'			\
	  '$(USE_ATRPMS)'

build-release-rpm: RPM-GPG-KEY-$(PACKAGE)
	./build-release-rpm.sh		\
	  $(PACKAGE)				\
	  '$(PACKAGE_TITLE)'			\
	  $(BASE_URL_PREFIX)			\
	  '$(DISTRIBUTIONS)'			\
	  $(HAVE_DEVELOPMENT_BRANCH)

RPM-GPG-KEY-$(PACKAGE):
	./gpg-public-key.sh > $@

source: ../$(PACKAGE)-$(VERSION).tar.gz

../$(PACKAGE)-$(VERSION).tar.gz: $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz
	ln -s $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz ../

$(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz:
	cd $(abs_top_builddir) && make dist
