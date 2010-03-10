VERSION = 1.1.1
SF_HTDOCS = ktou,cutter@web.sourceforge.net:/home/groups/c/cu/cutter/htdocs
ARCHITECTURES = i386 x86_64
CHROOT_BASE = /var/lib/chroot

all: build sign update upload

sign:
	./sign-rpm.sh

update:
	./update-repository.sh

upload:
	rsync -avz --delete --exclude .gitignore fedora/ $(SF_HTDOCS)/fedora

download:
	rsync -avz $(SF_HTDOCS)/fedora/ fedora

build: build-in-chroot build-repository-rpm

build-in-chroot:
	./build-in-chroot.sh $(VERSION) $(CHROOT_BASE) '$(ARCHITECTURES)'

build-repository-rpm: RPM-GPG-KEY-cutter
	./build-repository-rpm.sh

RPM-GPG-KEY-cutter:
	./gpg-public-key.sh > $@
