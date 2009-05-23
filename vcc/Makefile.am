nobase_EXTRA_DIST =				\
	cutter.sln				\
	cutter.vcproj				\
	cutter-cui.vcproj			\
	include/config.h			\
	include/cutter/cut-enum-types.h		\
	include/cutter/cut-features.h		\
	include/cutter/cut-marshalers.h		\
	include/cutter/cut-version.h		\
	include/gcutter/gcut-marshalers.h	\
	source/cutter/cut-enum-types.c		\
	source/cutter/cut-marshalers.c		\
	source/gcutter/gcut-marshalers.c

sync:
	sed -i''								\
	  -e 's/VERSION ".+"/VERSION "$(VERSION)"/'				\
	  -e 's/PACKAGE_STRING ".+"/PACKAGE_STRING "$(PACKAGE_STRING)"/'	\
	  include/config.h
	cp $(top_builddir)/cutter/cut-version.h include/cutter/
	cp $(top_builddir)/cutter/cut-enum-types.h include/cutter/
	cp $(top_builddir)/cutter/cut-marshalers.h include/cutter/
	cp $(top_builddir)/gcutter/gcut-marshalers.h include/gcutter/
	cp $(top_builddir)/cutter/cut-enum-types.c source/cutter/
	cp $(top_builddir)/cutter/cut-marshalers.c source/cutter/
	cp $(top_builddir)/gcutter/gcut-marshalers.c source/gcutter/
