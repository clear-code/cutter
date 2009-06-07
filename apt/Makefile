SF_HTDOCS = ktou,cutter@web.sourceforge.net:/home/groups/c/cu/cutter/htdocs
DISTRIBUTIONS = debian ubuntu

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
