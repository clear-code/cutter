EXTRA_DIST = index.html index.html.ja

upload:
	scp $(srcdir)/index.html $(srcdir)/index.html.ja $(SF_HTDOCS)/
	echo chmod 664 index.html | sftp $(SF_HTDOCS)
	echo chmod 664 index.html.ja | sftp $(SF_HTDOCS)
