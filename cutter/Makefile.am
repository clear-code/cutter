INCLUDES = 			\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

AM_CFLAGS = 					\
	-DLOCALEDIR=\""$(localedir)"\"		\
	$(CUTTER_CFLAGS)			\
	$(COVERAGE_CFLAGS)

CLEANFILES = *.gcno *.gcda

lib_LTLIBRARIES = libcutter.la

cutter_public_headers = 		\
	cut-version.h			\
	cut-features.h			\
	cut-assertions.h		\
	cut-assertions-helper.h		\
	cut-test-utils.h		\
	cut-experimental.h		\
	cut-public.h			\
	cut-types.h			\
	cut-macros.h			\
	cut-multi-process.h		\
	cut-helper.h			\
	cut-test-utils-helper.h

libcutter_public_headers = 		\
	cut-private.h			\
	cut-runner.h			\
	cut-run-context.h		\
	cut-test-runner.h		\
	cut-listener.h			\
	cut-listener-utils.h		\
	cut-main.h			\
	cut-test.h			\
	cut-iterated-test.h		\
	cut-test-container.h		\
	cut-test-iterator.h		\
	cut-test-case.h			\
	cut-test-suite.h		\
	cut-test-context.h		\
	cut-test-result.h		\
	cut-test-data.h			\
	cut-ui.h			\
	cut-contractor.h		\
	cut-factory-builder.h		\
	cut-report-factory-builder.h	\
	cut-stream-factory-builder.h	\
	cut-ui-factory-builder.h	\
	cut-module-factory.h		\
	cut-module-factory-utils.h	\
	cut-report.h			\
	cut-stream.h			\
	cut-process.h			\
	cut-pipeline.h			\
	cut-stream-parser.h		\
	cut-verbose-level.h		\
	cut-sequence-matcher.h		\
	cut-differ.h			\
	cut-diff-writer.h		\
	cut-string-diff-writer.h	\
	cut-readable-differ.h		\
	cut-colorize-differ.h		\
	cut-console-colors.h		\
	cut-analyzer.h			\
	cut-stream-reader.h		\
	cut-file-stream-reader.h	\
	cut-sub-process.h		\
	cut-sub-process-group.h		\
	cut-backtrace-entry.h

noinst_headers = 		\
	cut-loader.h		\
	cut-elf-loader.h	\
	cut-mach-o-loader.h	\
	cut-pe-loader.h		\
	cut-module.h		\
	cut-module-impl.h	\
	cut-repository.h	\
	cut-utils.h		\
	cut-crash-backtrace.h

pkginclude_HEADERS = 		\
	cutter.h

cutter_includedir=$(pkgincludedir)/cutter
cutter_include_HEADERS = 		\
	$(cutter_public_headers)	\
	$(libcutter_public_headers)	\
	$(enum_source_prefix).h

enum_source_prefix = cut-enum-types

enum_sources_h =			\
	cut-public.h			\
	$(libcutter_public_headers)

enum_sources =			\
	$(enum_source_prefix).c	\
	$(enum_source_prefix).h

MAINTAINERCLEANFILES =			\
	stamp-$(enum_source_prefix)-c	\
	stamp-$(enum_source_prefix)-h

BUILT_SOURCES = $(enum_sources)

bin_PROGRAMS = cutter

cutter_SOURCES = main.c

libcutter_la_SOURCES =			\
	cut-enum-types.h		\
	cut-enum-types.c		\
	$(cutter_public_headers)	\
	$(libcutter_public_headers)	\
	$(noinst_headers)		\
	cut-run-context.c		\
	cut-runner.c			\
	cut-test-runner.c		\
	cut-listener.c			\
	cut-main.c			\
	cut-test.c			\
	cut-iterated-test.c		\
	cut-test-container.c		\
	cut-test-iterator.c		\
	cut-test-case.c			\
	cut-test-suite.c		\
	cut-test-context.c		\
	cut-test-result.c		\
	cut-test-data.c			\
	cut-verbose-level.c		\
	cut-loader.c			\
	cut-elf-loader.c		\
	cut-mach-o-loader.c		\
	cut-pe-loader.c			\
	cut-repository.c		\
	cut-module.c			\
	cut-module-impl.h		\
	cut-ui.c			\
	cut-contractor.c		\
	cut-factory-builder.c		\
	cut-report-factory-builder.c	\
	cut-stream-factory-builder.c	\
	cut-ui-factory-builder.c	\
	cut-module-factory.c		\
	cut-module-factory-utils.c	\
	cut-report.c			\
	cut-stream.c			\
	cut-process.c			\
	cut-pipeline.c			\
	cut-utils.c			\
	cut-stream-parser.c		\
	cut-sequence-matcher.c		\
	cut-differ.c			\
	cut-diff-writer.c		\
	cut-string-diff-writer.c	\
	cut-readable-differ.c		\
	cut-colorize-differ.c		\
	cut-analyzer.c			\
	cut-stream-reader.c		\
	cut-file-stream-reader.c	\
	cut-sub-process.c		\
	cut-sub-process-group.c		\
	cut-backtrace-entry.c		\
	cut-assertions-helper.c		\
	cut-test-utils-helper.c		\
	cut-helper.c			\
	cut-crash-backtrace.c

libcutter_la_LDFLAGS =				\
	-version-info $(LT_VERSION_INFO)	\
	-no-undefined

libcutter_la_LIBADD =				\
	$(top_builddir)/gcutter/libgcutter.la	\
	$(GLIB_LIBS)

if HAVE_LIBBFD
libcutter_la_LIBADD += $(BFD_LIBS)
endif

if OS_WIN32
libcutter_la_LDFLAGS += -mwindows
endif

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

echo-public-headers:
	@for header in $(cutter_include_HEADERS); do			\
	  if test "$${header}" = "$(enum_source_prefix).h"; then	\
	    echo $(abs_builddir)/$(enum_source_prefix).h;		\
	  else								\
	    echo $(abs_srcdir)/$${header};				\
	  fi;								\
	done
