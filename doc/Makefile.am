SUBDIRS = reference

EXTRA_DIST =					\
	cutter.rd				\
	cutter.rd.ja				\
	install-to-debian.rd			\
	install-to-debian.rd.ja			\
	install-to-cygwin.rd			\
	install-to-cygwin.rd.ja			\
	install-to-others.rd			\
	install-to-others.rd.ja			\
	$(svg_images)

svg_images =					\
	cutter-accelerate-development-cycle.svg	\
	cutter-two-important-features.svg	\
	cutter-easy-to-debug.svg		\
	cutter-easy-to-write.svg

dist_man1_MANS =	\
	cutter.man

ja_man1dir = $(mandir)/ja/man1

dist_ja_man1_mans =	\
	cutter.jman

EXTRA_DIST += $(dist_ja_man1_mans)

jman: $(dist_ja_man1_mans)

install-data-local:
	test -z "$(DESTDIR)$(ja_man1dir)" || $(MKDIR_P) "$(DESTDIR)$(ja_man1dir)"
	@for jman in $(dist_ja_man1_mans); do					\
	  jman_file=`echo $${jman} | sed -e s/jman$$/1/`;			\
	  echo $(INSTALL_DATA) $${jman} "$(DESTDIR)$(ja_man1dir)"/$${jman_file};\
	  $(INSTALL_DATA) $${jman} "$(DESTDIR)$(ja_man1dir)"/$${jman_file};	\
	done

uninstall-local:
	@for jman in $(dist_ja_man1_mans); do			\
	  jman_file=`echo $${jman} | sed -e s/jman$$/1/`;	\
	  rm -f "$(DESTDIR)$(ja_man1dir)"/$${jman_file};	\
	done

# dirty...
cutter.jman: cutter.rd.ja

MAINTAINERCLEANFILES = $(dist_man1_MANS) $(dist_ja_man1_mans)

SUFFIXES: .rd .man .jman .svg .png

.rd.man:
	rd2 -r$(top_srcdir)/misc/rd2man-lib $< > $@

.rd.jman: # dirty...
	rd2 -r$(top_srcdir)/misc/rd2man-lib $<.ja > $@

.svg.png:
	inkscape --export-png $@ $<
