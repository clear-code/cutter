VERSION = 1.1.0
SERVER_PATH = ktou,cutter@web.sourceforge.net:/home/groups/c/cu/cutter/htdocs
DISTRIBUTIONS = debian ubuntu
CHROOT_BASE = /var/lib/chroot
ARCHITECTURES = i386 amd64
CODES = lenny unstable hardy karmic

update:
	for code_name in $(CODES); do						\
	  target=$${code_name}-$${architecture};				\
	  case $${code_name} in							\
	  lenny|unstable)							\
	    distribution=debian;						\
	    section=main;							\
	    ;;									\
	  *)									\
	    distribution=ubuntu;						\
	    section=main;							\
	    ;;									\
	  esac;									\
	  (cd $${distribution};							\
	   mkdir -p dists/$${code_name}/$${section}/source;			\
	   for architecture in $(ARCHITECTURES); do				\
	     mkdir -p dists/$${code_name}/$${section}/binary-$${architecture};	\
	   done;								\
	   apt-ftparchive generate generate-$${code_name}.conf;			\
	   rm -f dists/$${code_name}/Release*;					\
	   apt-ftparchive -c release-$${code_name}.conf				\
	     release dists/$${code_name} > /tmp/Release;			\
	   mv /tmp/Release dists/$${code_name};					\
	   gpg --sign -ba -o dists/$${code_name}/Release{.gpg,};		\
	  );									\
	done

upload: update
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
	for architecture in $(ARCHITECTURES); do				\
	  for code_name in $(CODES); do						\
	    target=$${code_name}-$${architecture};				\
	    case $${code_name} in						\
	    lenny|unstable)							\
	      distribution=debian;						\
	      section=main;							\
	      ;;								\
	    *)									\
	      distribution=ubuntu;						\
	      section=main;							\
	      ;;								\
	    esac;								\
	    build_dir=$(CHROOT_BASE)/$$target/home/$$USER/work/c;		\
	    pool_dir=$$distribution/pool/$$code_name/$$section/c/cutter;	\
	    mkdir -p $$build_dir;						\
	    cp ../cutter-$(VERSION).tar.gz					\
	      $$build_dir/cutter_$(VERSION).orig.tar.gz &&			\
	    sudo su -c								\
	      "chroot $(CHROOT_BASE)/$$target su - $$USER" < build-deb.sh;	\
	    mkdir -p $$pool_dir;						\
	    cp -p $$build_dir/*cutter*_$(VERSION)* $$pool_dir;			\
	  done;									\
	done
