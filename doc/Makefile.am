SUBDIRS = reference

EXTRA_DIST =					\
	cutter.rd				\
	cutter.rd.ja				\
	cut-diff.rd				\
	cut-diff.rd.ja				\
	install-to-debian.rd			\
	install-to-debian.rd.ja			\
	install-to-cygwin.rd			\
	install-to-cygwin.rd.ja			\
	install-to-fedora.rd			\
	install-to-fedora.rd.ja			\
	install-to-centos.rd			\
	install-to-centos.rd.ja			\
	install-to-freebsd.rd			\
	install-to-freebsd.rd.ja		\
	install-to-mac-os-x.rd			\
	install-to-mac-os-x.rd.ja		\
	install-to-solaris.rd			\
	install-to-solaris.rd.ja		\
	install-to-ubuntu.rd			\
	install-to-ubuntu.rd.ja			\
	install-to-others.rd			\
	install-to-others.rd.ja			\
	travis-ci-integration.rd		\
	travis-ci-integration.rd.ja		\
	release.rd.ja				\
	$(svg_images)

svg_images =					\
	cutter-accelerate-development-cycle.svg	\
	cutter-two-important-features.svg	\
	cutter-easy-to-debug.svg		\
	cutter-easy-to-write.svg

if MAN_AVAILABLE
dist_man1_MANS =	\
	cutter.man	\
	cut-diff.man

ja_man1dir = $(mandir)/ja/man1

dist_ja_man1_mans =	\
	cutter.jman	\
	cut-diff.jman

man_build_stamp = man-build.stamp

EXTRA_DIST +=					\
	$(dist_ja_man1_mans)			\
	$(man_build_stamp)

man-build.stamp: $(dist_ja_man1_mans)
	@touch $@
endif

.PHONY: jman

jman: $(man_build_stamp)

install-data-local: jman
	test -z "$(DESTDIR)$(ja_man1dir)" || $(mkdir_p) "$(DESTDIR)$(ja_man1dir)"
	@for jman in $(dist_ja_man1_mans); do			\
	  test -f "$${jman}" || break;				\
	  jman_path=$(top_builddir)/doc/$${jman};		\
	  jman_file=`echo $${jman} | sed -e s/jman$$/1/`;	\
	  echo $(INSTALL_DATA) $${jman_path}			\
	    "$(DESTDIR)$(ja_man1dir)"/$${jman_file};		\
	  $(INSTALL_DATA) $${jman_path}				\
	    "$(DESTDIR)$(ja_man1dir)"/$${jman_file};		\
	done

uninstall-local:
	@for jman in $(dist_ja_man1_mans); do			\
	  jman_file=`echo $${jman} | sed -e s/jman$$/1/`;	\
	  rm -f "$(DESTDIR)$(ja_man1dir)"/$${jman_file};	\
	done

# dirty...
cutter.jman: cutter.rd.ja
cut-diff.jman: cut-diff.rd.ja

MAINTAINERCLEANFILES = $(dist_man1_MANS) $(dist_ja_man1_mans)

SUFFIXES = .rd .man .jman .svg .png

RD2LIB_DIR = $(top_srcdir)/misc
RD2MAN_LIB_FILE = rd2man-lib.rb
RD2MAN_LIB = $(RD2LIB_DIR)/$(RD2MAN_LIB_FILE)
RD2MAN = $(RUBY) -I $(RD2LIB_DIR) $(RD2) -r $(RD2MAN_LIB_FILE)

.rd.man:
	$(RD2MAN) $< > $@

.rd.jman: # dirty...
	LC_ALL=en_US.UTF-8 $(RD2MAN) $<.ja > $@

.svg.png:
	inkscape --export-png $@ $<
