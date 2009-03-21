SUBDIRS = reference

EXTRA_DIST =

dist_man1_MANS =	\
	cutter.man

ja_man1dir = $(mandir)/ja/man1

#dist_ja_man1_DATA = 	\
# man in Japanese is not installed. Should they be installed? But how?
dist_ja_man1_mans =	\
	cutter.jman

EXTRA_DIST += $(dist_ja_man1_mans)

# durty...
cutter.jman: cutter.rd.ja

MAINTAINERCLEANFILES = $(dist_man1_MANS) $(dist_ja_man1_mans)

SUFFIXES: .rd .man .jman

.rd.man:
	rd2 -r$(top_srcdir)/misc/rd2man-lib $< > $@

.rd.jman: # durty...
	rd2 -r$(top_srcdir)/misc/rd2man-lib $<.ja > $@
