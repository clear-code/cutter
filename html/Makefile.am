EXTRA_DIST =					\
	$(web_contents)				\
	$(svg_images)

web_contents =					\
	.htaccess				\
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
	tutorial.svg				\
	favicon.svg

images =					\
	github-logo.png				\
	famfamfam-logo.png			\
	tango-logo.png				\
	jp.png					\
	us.png					\
	reference.png				\
	notify-on-gnome.png			\
	notify-on-macos.png			\
	$(generated_images)

generated_images =				\
	logo.png				\
	bar.png					\
	blog.png				\
	download.png				\
	heading-mark.png			\
	mini-heading-mark.png			\
	install.png				\
	mini-install.png			\
	readme.png				\
	tutorial.png				\
	favicon.png				\
	favicon.ico

MAINTAINER_CLEANFILES = $(generated_images)

upload: $(web_contents)
	(cd $(srcdir) && rsync -avz $(web_contents) $(OSDN_HTDOCS)/)

SUFFIXES = .svg .png

.svg.png:
	inkscape --export-png $@ $<

mini-install.png: install.png
	convert $(srcdir)/install.png -resize 12x12 mini-install.png

mini-heading-mark.png: heading-mark.png
	convert $(srcdir)/heading-mark.png -resize 19x19 mini-heading-mark.png
