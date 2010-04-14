VERSION = 1.1.3
SERVER_PATH = ktou,cutter@web.sourceforge.net:/home/groups/c/cu/cutter/htdocs
DISTRIBUTIONS = debian ubuntu
CHROOT_BASE = /var/lib/chroot
ARCHITECTURES = i386 amd64
CODES = lenny unstable hardy karmic

all: remove-existing-packages build update sign upload

remove-existing-packages:
	for distribution in $(DISTRIBUTIONS); do	\
	  find $${distribution}/pool -not		\
	    -path '*/.svn/*' \-type f -delete;		\
	done

update:
	./update-repository.sh '$(DISTRIBUTIONS)' '$(ARCHITECTURES)'

sign:
	./sign-repository.sh '$(DISTRIBUTIONS)'

upload:
	for distribution in $(DISTRIBUTIONS); do		\
	  (cd $${distribution};					\
	   rsync -avz --exclude .svn --delete			\
	     dists pool $(SERVER_PATH)/$${distribution};	\
	  );							\
	done

download:
	for distribution in $(DISTRIBUTIONS); do			\
	  (cd $${distribution};						\
	   rsync -avz $(SERVER_PATH)/$${distribution}/pool/ pool;	\
	  );								\
	done

build:
	./build-in-chroot.sh \
	  $(VERSION) $(CHROOT_BASE) \
	  '$(ARCHITECTURES)' '$(CODES)'
