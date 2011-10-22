SERVER_PATH = $(SF_FRS)
DISTRIBUTIONS = debian ubuntu
CHROOT_BASE = /var/lib/chroot
ARCHITECTURES = i386 amd64
CODE_NAMES = squeeze wheezy unstable lucid natty oneiric

release: download build update sign upload

update:
	./update-repository.sh \
	  '$(SF_PROJECT_NAME)' '$(PACKAGE_TITLE)' '$(CODE_NAMES)'

sign:
	./sign-repository.sh '$(CODE_NAMES)'

upload:
	for distribution in $(DISTRIBUTIONS); do	\
	  (cd $${distribution};				\
	   rsync -avz --delete				\
	     .htaccess dists pool			\
	     $(SERVER_PATH)/$${distribution};		\
	  );						\
	done

download:
	for distribution in $(DISTRIBUTIONS); do		\
	  (mkdir -p $${distribution} &&				\
	   cd $${distribution} &&				\
	   rm -rf pool dists &&					\
	   rsync -avz $(SERVER_PATH)/$${distribution}/ .;	\
	  );							\
	done

build: source
	./build-in-chroot.sh			\
	  $(PACKAGE) $(VERSION) $(CHROOT_BASE)	\
	  '$(ARCHITECTURES)' '$(CODE_NAMES)'

source: ../$(PACKAGE)-$(VERSION).tar.gz

../$(PACKAGE)-$(VERSION).tar.gz: $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz
	ln -s $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz ../

$(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz:
	cd $(abs_top_builddir) && make dist
