EXTRA_DIST =					\
	$(web_contents)				\
	$(svg_images)

web_contents =					\
	index.html				\
	index.html.ja				\
	cutter.css				\
	$(images)

svg_images =					\
	logo.svg				\
	bar.svg					\
	blog.svg				\
	download.svg				\
	heading-mark.svg			\
	install.svg				\
	readme.svg				\
	tutorial.svg

images =					\
	famfamfam-logo.png			\
	tango-logo.png				\
	jp.png					\
	us.png					\
	reference.png				\
	$(generated_images)

generated_images =				\
	logo.png				\
	bar.png					\
	blog.png				\
	download.png				\
	heading-mark.png			\
	install.png				\
	readme.png				\
	tutorial.png

MAINTAINER_CLEANFILES = $(generated_images)

upload: $(web_contents)
	(cd $(srcdir) && rsync -avz $(web_contents) $(SF_HTDOCS)/)

SUFFIXES = .svg .png

.svg.png:
	inkscape --export-png $@ $<
