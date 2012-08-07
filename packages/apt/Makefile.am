SERVER_PATH = $(SF_FRS)
DISTRIBUTIONS = debian ubuntu
CHROOT_BASE = /var/lib/chroot
ARCHITECTURES = i386 amd64
CODE_NAMES = squeeze wheezy unstable lucid natty oneiric precise
REPOSITORIES_PATH = repositories
KEYRING_PACKAGE = cutter-keyring
KEYRING_VERSION = 2012.08.29
KEYRING_BASE_NAME = $(KEYRING_PACKAGE)-$(KEYRING_VERSION)

release: download build update sign upload

update:
	./update-repository.sh \
	  '$(SF_PROJECT_NAME)' '$(PACKAGE_TITLE)' '$(CODE_NAMES)'

sign:
	./sign-repository.sh '$(CODE_NAMES)'

upload:
	for distribution in $(DISTRIBUTIONS); do	\
	  (cd $(REPOSITORIES_PATH)/$${distribution};				\
	   rsync -avz --delete				\
	     .htaccess dists pool			\
	     $(SERVER_PATH)/$${distribution};		\
	  );						\
	done

download:
	for distribution in $(DISTRIBUTIONS); do		\
	  rsync -avz --progress --delete                        \
            $(SERVER_PATH)/$${distribution}                     \
	    $(REPOSITORIES_PATH)/;                              \
	done

build: build-package-deb build-keyring-deb

build-package-deb: source
	./build-in-chroot.sh			\
	  $(PACKAGE) $(VERSION) $(CHROOT_BASE)	\
          $(srcdir)/.. $(REPOSITORIES_PATH)/    \
	  '$(ARCHITECTURES)' '$(CODE_NAMES)'

source: ../$(PACKAGE)-$(VERSION).tar.gz

../$(PACKAGE)-$(VERSION).tar.gz: $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz
	ln -s $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz ../

$(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz:
	cd $(abs_top_builddir) && make dist

build-keyring-deb: keyring-source
	./build-in-chroot.sh                                    \
	  $(KEYRING_PACKAGE) $(KEYRING_VERSION)                 \
          $(srcdir) $(REPOSITORIES_PATH)/                       \
          $(CHROOT_BASE) '$(ARCHITECTURES)' '$(CODE_NAMES)'

keyring-source: $(KEYRING_BASE_NAME).tar.gz

ensure-public-key:
	gpg --list-keys '$(GPG_UID)' > /dev/null ||                     \
	  gpg --keyserver keyserver.ubuntu.com --recv-key '$(GPG_UID)'

$(KEYRING_BASE_NAME).tar.gz:
	rm -rf $(KEYRING_BASE_NAME)
	mkdir -p $(KEYRING_BASE_NAME)
	gpg --armor --export '$(GPG_UID)' > \
	  $(KEYRING_BASE_NAME)/cutter-keyring.gpg
	tar cvzf $(KEYRING_BASE_NAME).tar.gz $(KEYRING_BASE_NAME)


