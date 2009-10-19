VERSION = 1.0.9
SF_HTDOCS = ktou,cutter@web.sourceforge.net:/home/groups/c/cu/cutter/htdocs
DISTRIBUTIONS = debian ubuntu
CHROOT_BASE = /var/lib/chroot
ARCHITECTURES = i386 amd64
CODES = lenny unstable hardy jaunty

update:
	for distribution in $(DISTRIBUTIONS); do			\
	  (cd $${distribution};						\
	   for target in `cat TARGETS`; do				\
	     apt-ftparchive generate generate-$${target}.conf;		\
	     rm -f dists/$${target}/Release*;				\
	     apt-ftparchive -c release-$${target}.conf			\
	       release dists/$${target} > /tmp/Release;			\
	     mv /tmp/Release dists/$${target};				\
	     gpg --sign -ba -o dists/$${target}/Release{.gpg,};		\
	    done;							\
          );								\
	done

upload: update
	for distribution in $(DISTRIBUTIONS); do	\
	  (cd $${distribution};				\
	   rsync -avz --exclude .svn --delete		\
	     dists pool $(SF_HTDOCS)/$${distribution};	\
	  );						\
	done

download:
	for distribution in $(DISTRIBUTIONS); do		\
	  (cd $${distribution};					\
	   rsync -avz $(SF_HTDOCS)/$${distribution}/pool/ pool;	\
	  );							\
	done

build:
	for architecture in $(ARCHITECTURES); do				\
	  for code_name in $(CODES); do						\
	    target=$${code_name}-$${architecture};				\
	    case $${code_name} in						\
	    lenny|unstable)							\
	      distribution=debian;						\
	      ;;								\
	    *)									\
	      distribution=ubuntu;						\
	      ;;								\
	    esac;								\
	    build_dir=$(CHROOT_BASE)/$$target/home/$$USER/work/c;		\
	    pool_dir=$$distribution/pool/$$code_name/main/c/cutter;		\
	    mkdir -p $$build_dir;						\
	    cp ../cutter-$(VERSION).tar.gz					\
	      $$build_dir/cutter_$(VERSION).orig.tar.gz &&			\
	    sudo su -c								\
	      "chroot $(CHROOT_BASE)/$$target su - $$USER" < build-deb.sh;	\
	    cp -p $$build_dir/*cutter*_$(VERSION)* $$pool_dir;			\
	  done;									\
	done
