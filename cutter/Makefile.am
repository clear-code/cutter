SUBDIRS = module

INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS = 						\
	-DLOCALEDIR=\""$(localedir)"\"			\
	$(CUTTER_CFLAGS)				\
	$(COVERAGE_CFLAGS)

EXTRA_DIST = config.h.in cut-marshalers.list

lib_LTLIBRARIES = libcutter.la

cutter_public_headers = 			\
		cut-assertions.h		\
		cut-assertions-helper.h		\
		cut-experimental.h		\
		cut-hidden-definitions.h	\
		cut-public.h

gcutter_public_headers =			\
		cut-gassertions.h		\
		cut-gassertions-helper.h	\
		cut-gpublic.h			\
		cut-value-equal.h		\
		cut-list.h

libcutter_public_headers = 			\
		cut-private.h			\
		cut-run-context.h		\
		cut-runner.h			\
		cut-listener.h			\
		cut-main.h			\
		cut-test.h			\
		cut-test-case.h			\
		cut-test-container.h		\
		cut-test-context.h		\
		cut-test-suite.h		\
		cut-test-result.h		\
		cut-ui.h			\
		cut-contractor.h		\
		cut-factory-builder.h		\
		cut-report-factory-builder.h	\
		cut-streamer-factory-builder.h	\
		cut-ui-factory-builder.h	\
		cut-module-factory.h		\
		cut-module-factory-utils.h	\
		cut-report.h			\
		cut-streamer.h			\
		cut-process.h			\
		cut-pipeline.h			\
		cut-xml-parser.h		\
		cut-verbose-level.h		\
		cut-sequence-matcher.h		\
		cut-diff.h

noinst_headers = 			\
		gbsearcharray.h		\
		cut-loader.h		\
		cut-module.h		\
		cut-module-impl.h	\
		cut-repository.h	\
		cut-utils.h

cutterincludedir=$(includedir)/cutter
cutterinclude_HEADERS = 		\
	cutter.h			\
	gcutter.h

cuttersubincludedir=$(includedir)/cutter/cutter
cuttersubinclude_HEADERS = 		\
	$(cutter_public_headers)	\
	$(gcutter_public_headers)	\
	$(libcutter_public_headers)

enum_source_prefix = cut-enum-types

enum_sources_h =			\
	cut-public.h			\
	$(libcutter_public_headers)

enum_sources =			\
	$(enum_source_prefix).c	\
	$(enum_source_prefix).h

marshal_sources =	\
	cut-marshalers.c cut-marshalers.h

MAINTAINERCLEANFILES =			\
	stamp-$(enum_source_prefix)-c	\
	stamp-$(enum_source_prefix)-h   \
	$(marshal_sources)

BUILT_SOURCES = $(enum_sources) $(marshal_sources)

bin_PROGRAMS = cutter

cutter_SOURCES = main.c

libcutter_la_SOURCES =				\
		cut-enum-types.h		\
		cut-enum-types.c		\
		$(marshal_sources)		\
		$(cutter_public_headers)	\
		$(gcutter_public_headers)	\
		$(libcutter_public_headers)	\
		$(noinst_headers)		\
		cut-run-context.c		\
		cut-runner.c			\
		cut-listener.c			\
		cut-main.c			\
		cut-test.c			\
		cut-test-context.c		\
		cut-test-container.c		\
		cut-test-case.c			\
		cut-test-result.c		\
		cut-test-suite.c		\
		cut-verbose-level.c		\
		cut-loader.c			\
		cut-repository.c		\
		cut-module.c			\
		cut-module-impl.h		\
		cut-ui.c			\
		cut-contractor.c		\
		cut-factory-builder.c		\
		cut-report-factory-builder.c	\
		cut-streamer-factory-builder.c	\
		cut-ui-factory-builder.c	\
		cut-module-factory.c		\
		cut-module-factory-utils.c	\
		cut-report.c			\
		cut-streamer.c			\
		cut-process.c			\
		cut-pipeline.c			\
		cut-utils.c			\
		cut-xml-parser.c		\
		cut-value-equal.c		\
		cut-list.c			\
		cut-sequence-matcher.c		\
		cut-diff.c

libcutter_la_LIBADD =			\
	$(GLIB_LIBS)

cutter_LDADD = libcutter.la

$(enum_source_prefix).c: stamp-$(enum_source_prefix)-c $(enum_source_prefix).h
	@true

stamp-$(enum_source_prefix)-c: $(enum_sources_h) Makefile
	(cd $(srcdir) && \
	  include_headers="" && \
	  for h in $(enum_sources_h); do \
	    include_headers="$${include_headers}#include \"$${h}\"\n"; \
	  done && \
	  $(GLIB_MKENUMS) \
	    --fhead "#include \"$(enum_source_prefix).h\"\n$${include_headers}" \
	    --fprod "\n/* enumerations from \"@filename@\" */" \
	    --vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" 	\
	    --vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
	    --vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
	    $(enum_sources_h)) > tmp-$(enum_source_prefix).c && \
	(cmp -s tmp-$(enum_source_prefix).c $(enum_source_prefix).c || \
	  cp tmp-$(enum_source_prefix).c $(enum_source_prefix).c ) && \
	rm -f tmp-$(enum_source_prefix).c && \
	echo timestamp > $(@F)

$(enum_source_prefix).h: stamp-$(enum_source_prefix)-h
	@true

stamp-$(enum_source_prefix)-h: $(enum_sources_h) Makefile
	(cd $(srcdir) && \
	  mark="__`echo $(enum_source_prefix) | sed -e 's/-/_/g' | tr a-z A-Z`_H__" && \
	  $(GLIB_MKENUMS) \
	    --fhead "#ifndef $${mark}\n#define $${mark}\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
	    --fprod "/* enumerations from \"@filename@\" */\n" \
	    --vhead "GType @enum_name@_get_type (void);\n#define CUT_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" 	\
	    --ftail "G_END_DECLS\n\n#endif /* $${mark} */" \
	    $(enum_sources_h)) > tmp-$(enum_source_prefix).h && \
	(cmp -s tmp-$(enum_source_prefix).h $(enum_source_prefix).h || \
	  cp tmp-$(enum_source_prefix).h $(enum_source_prefix).h) && \
	rm -f tmp-$(enum_source_prefix).h && \
	echo timestamp > $(@F)

cut-marshalers.h : cut-marshalers.list
	$(GLIB_GENMARSHAL) $(top_srcdir)/cutter/cut-marshalers.list --header --prefix=_cut_marshal > $@

cut-marshalers.c : cut-marshalers.list
	$(GLIB_GENMARSHAL) $(top_srcdir)/cutter/cut-marshalers.list --header --body --prefix=_cut_marshal > $@
