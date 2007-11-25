EXTRA_DIST = index.html

upload:
	scp $(srcdir)/index.html $(SF_HTDOCS)/
	ssh $(SF_HOST) chmod g+w $(SF_HTDOCS_PATH)/index.html
