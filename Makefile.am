SUBDIRS = cutter test sample po misc doc html

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = 	\
	cutter.pc	\
	libcutter.pc

EXTRA_DIST =		\
	autogen.sh	\
	TUTORIAL	\
	TUTORIAL.ja	\
	README.ja	\
	NEWS		\
	NEWS.ja

upload: upload-doc

upload-doc:
	cd html && $(MAKE) upload
	cd doc/reference && $(MAKE) $(AM_MAKEFLAGS) upload
