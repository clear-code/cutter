SERVER_PATH = $(SF_USER),cutter@$(SF_HTDOCS)
ARCHITECTURES = i386 x86_64
CHROOT_BASE = /var/lib/chroot

release: remove-existing-packages build sign update upload

remove-existing-packages:
	find fedora -not -path '*/.svn/*' -name "*.rpm" -delete

sign:
	./sign-rpm.sh

update:
	./update-repository.sh

upload:
	rsync -avz --delete --exclude .gitignore --exclude .svn \
		fedora/ $(SERVER_PATH)/fedora

download:
	rsync -avz $(SERVER_PATH)/fedora/ fedora

build: build-in-chroot build-repository-rpm

build-in-chroot:
	./build-in-chroot.sh $(VERSION) $(CHROOT_BASE) '$(ARCHITECTURES)'

build-repository-rpm: RPM-GPG-KEY-cutter
	./build-repository-rpm.sh

RPM-GPG-KEY-cutter:
	./gpg-public-key.sh > $@
