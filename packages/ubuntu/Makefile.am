UBUNTU_CODE_NAMES = xenial,bionic,focal
UBUNTU_VERSIONS = 16.04,18.04,20.04
PPA = cutter-ppa
PPA_INCOMING = ~cutter-testing-framework/ppa/ubuntu/
SOURCE = $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz

all:

ensure-launchpad-configuration:
	@if test -z "$(LAUNCHPAD_UPLOADER_PGP_KEY)"; then		\
	  echo "--with-launchpad-uploader-pgp-key configure option must be specified.";	\
	  false;							\
	fi

upload: source ensure-launchpad-configuration
	$(GROONGA_SOURCE_PATH)/packages/ubuntu/upload.rb \
	  --ppa '$(PPA)' \
	  --ppa-incoming '$(PPA_INCOMING)' \
	  --package 'cutter' \
	  --version '$(VERSION)' \
	  --source-archive '$(SOURCE)' \
	  --ubuntu-code-names '$(UBUNTU_CODE_NAMES)' \
	  --ubuntu-versions '$(UBUNTU_VERSIONS)' \
	  --debian-directory '$(srcdir)/../debian/' \
	  --pgp-sign-key '$(LAUNCHPAD_UPLOADER_PGP_KEY)'

source: $(SOURCE)

