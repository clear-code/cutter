VERSION = 1.0.8
SF_HTDOCS = ktou,cutter@web.sourceforge.net:/home/groups/c/cu/cutter/htdocs
DISTRIBUTIONS = debian ubuntu
CHROOT_BASE = /var/lib/chroot

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
	for target in {lenny,sid,hardy,jaunty}-{i386,amd64}; do		\
	  if echo $$target | grep -e 'lenny\|sid' > /dev/null; then	\
	    distribution=debian;					\
	  else								\
	    distribution=ubuntu;					\
	  fi								\
	  build_dir=$(CHROOT_BASE)/$$target/home/$$USER/work/c;		\
	  cp ../cutter-$(VERSION).tar.gz				\
	    $$build_dir/cutter_$(VERSION).orig.tar.gz &&		\
	  sudo su -c							\
	    "chroot $(CHROOT_BASE)/$$target su - $$USER" < build-deb.sh;\
	  cp -p $$build_dir/*cutter*_$(VERSION)*			\
	    $$distribution/pool/main/c/cutter;				\
	done
