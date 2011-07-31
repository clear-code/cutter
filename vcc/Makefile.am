nobase_EXTRA_DIST =				\
	download-vendor-libraries.rb		\
	cutter.sln				\
	cutter.vcproj				\
	cutter-cui.vcproj			\
	include/config.h			\
	include/cutter/cut-enum-types.h		\
	include/cutter/cut-features.h		\
	include/cutter/cut-version.h		\
	include/gcutter/gcut-marshalers.h	\
	source/cutter/cut-enum-types.c		\
	source/gcutter/gcut-marshalers.c

sync_actions =					\
	sync-config				\
	sync-generated-files			\
	generate-def				\
	generate-makefile			\
	generate-test-def			\
	generate-test-makefile			\
	generate-test-fixture-def		\
	generate-test-fixture-makefile

sync: $(sync_actions)

sync-config:
	sed -i''								\
	  -e 's/VERSION ".*"/VERSION "$(VERSION)"/'				\
	  -e 's/PACKAGE_STRING ".*"/PACKAGE_STRING "$(PACKAGE_STRING)"/'	\
	  include/config.h

sync-generated-files:
	cp $(top_builddir)/cutter/cut-version.h include/cutter/
	cp $(top_builddir)/cutter/cut-enum-types.h include/cutter/
	cp $(top_builddir)/gcutter/gcut-marshalers.h include/gcutter/
	cp $(top_builddir)/cutter/cut-enum-types.c source/cutter/
	cp $(top_builddir)/gcutter/gcut-marshalers.c source/gcutter/

generate-def:
	($(MAKE) -s -C $(top_builddir)/cutter echo-public-headers;	\
	 $(MAKE) -s -C $(top_builddir)/cutter echo-private-headers;	\
	 $(MAKE) -s -C $(top_builddir)/gcutter echo-public-headers) |	\
	  xargs $(top_srcdir)/misc/generate-def.rb >			\
	    resource/cutter.def
	$(MAKE) -s -C $(top_builddir)/gdkcutter-pixbuf echo-public-headers |	\
	  xargs $(top_srcdir)/misc/generate-def.rb >				\
	    resource/gdkcutter-pixbuf.def

generate-makefile:
	$(MAKE) -s -C $(top_builddir)/cutter echo-libcutter-sources |	\
	  xargs $(top_srcdir)/misc/generate-makefile-objects.rb		\
	    LIBCUTTER_OBJECTS '$$(top_builddir)/cutter/' > 		\
	      Makefile.objects.cutter
	$(MAKE) -s -C $(top_builddir)/gcutter echo-libgcutter-sources |	\
	  xargs $(top_srcdir)/misc/generate-makefile-objects.rb		\
	    LIBGCUTTER_OBJECTS '$$(top_builddir)/gcutter/' >		\
	      Makefile.objects.gcutter
	$(MAKE) -s -C $(top_builddir)/cppcutter echo-libcppcutter-sources |	\
	  xargs $(top_srcdir)/misc/generate-makefile-objects.rb			\
	    LIBCPPCUTTER_OBJECTS '$$(top_builddir)/cppcutter/' >		\
	      Makefile.objects.cppcutter

generate-test-def:
	($(MAKE) -s -C $(top_builddir)/test/cutter echo-tests) |	\
	  xargs $(top_srcdir)/misc/generate-test-defs.rb		\
	    $(top_srcdir)/test/cutter resource/test/cutter

generate-test-makefile:
	$(MAKE) -s -C $(top_builddir)/test/lib		\
	  echo-cuttest-utils-public-headers |		\
	    xargs $(top_srcdir)/misc/generate-def.rb >	\
	      resource/cuttest-utils.def
	$(MAKE) -s -C $(top_builddir)/test/lib		\
	  echo-cuttest-event-receiver-public-headers |	\
	    xargs $(top_srcdir)/misc/generate-def.rb >	\
	      resource/cuttest-event-receiver.def
	($(MAKE) -s -C $(top_builddir)/test/cutter echo-tests) |	\
	  xargs $(top_srcdir)/misc/generate-test-makefile.rb		\
	    '$$(top_srcdir)' test/cutter

binary_fixture_paths =						\
	test/fixtures/factory/type1				\
	test/fixtures/factory/type2				\
	test/fixtures/loader/attribute				\
	test/fixtures/loader/cpp-fixture			\
	test/fixtures/loader/cpp				\
	test/fixtures/loader/fixture				\
	test/fixtures/loader/iterator				\
	test/fixtures/loader/module				\
	test/fixtures/loader/suite				\
	test/fixtures/loader/test				\
	test/fixtures/path/parent/sub				\
	test/fixtures/pipeline/error				\
	test/fixtures/pipeline/failure				\
	test/fixtures/pipeline/omission				\
	test/fixtures/pipeline/pending				\
	test/fixtures/pipeline/setup-error			\
	test/fixtures/pipeline/success				\
	test/fixtures/repository				\
	test/fixtures/sub-process-group				\
	test/fixtures/sub-process-group/fixtures/normal		\
	test/fixtures/sub-process				\
	test/fixtures/sub-process/fixtures/normal		\
	test/fixtures/test-context

generate-test-fixture-def:
	for path in $(binary_fixture_paths); do			\
	  ($(MAKE) -s -C $(top_builddir)/$${path} echo-tests) |	\
	    xargs $(top_srcdir)/misc/generate-test-defs.rb	\
	      $(top_srcdir)/$${path} resource/$${path};		\
	done

generate-test-fixture-makefile:
	for path in $(binary_fixture_paths); do			\
	  ($(MAKE) -s -C $(top_builddir)/$${path} echo-tests) |	\
	    xargs $(top_srcdir)/misc/generate-test-makefile.rb	\
	      '$$(top_srcdir)' $${path};			\
	done
