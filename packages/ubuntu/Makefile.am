CODE_NAMES = precise,trusty,wily,xenial
SOURCE = $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz

all:

ensure-launchpad-configuration:
	@if test -z "$(LAUNCHPAD_UPLOADER_PGP_KEY)"; then		\
	  echo "--with-launchpad-uploader-pgp-key configure option must be specified.";	\
	  false;							\
	fi

upload: source ensure-launchpad-configuration
	./upload.rb \
	  --package 'cutter' \
	  --version '$(VERSION)' \
	  --source-archive '$(SOURCE)' \
	  --code-names '$(CODE_NAMES)' \
	  --debian-directory '$(srcdir)/../debian/' \
	  --pgp-sign-key '$(LAUNCHPAD_UPLOADER_PGP_KEY)'

source: $(SOURCE)

