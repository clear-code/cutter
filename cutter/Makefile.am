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
	cut-assertions-helper.h		\
	cut-assertions.h		\
	cut-experimental.h		\
	cut-features.h			\
	cut-helper.h			\
	cut-macros.h			\
	cut-multi-process.h		\
	cut-public.h			\
	cut-test-utils-helper.h		\
	cut-test-utils.h		\
	cut-types.h			\
	cut-version.h			\
	cut-config.h

libcutter_public_headers = 		\
	cut-analyzer.h			\
	cut-backtrace-entry.h		\
	cut-colorize-differ.h		\
	cut-console-diff-writer.h	\
	cut-console.h			\
	cut-contractor.h		\
	cut-diff-writer.h		\
	cut-differ.h			\
	cut-factory-builder.h		\
	cut-file-stream-reader.h	\
	cut-iterated-test.h		\
	cut-listener-utils.h		\
	cut-listener.h			\
	cut-main.h			\
	cut-module-factory-utils.h	\
	cut-module-factory.h		\
	cut-pipeline.h			\
	cut-private.h			\
	cut-process.h			\
	cut-readable-differ.h		\
	cut-report-factory-builder.h	\
	cut-report.h			\
	cut-run-context.h		\
	cut-runner.h			\
	cut-sequence-matcher.h		\
	cut-stream-factory-builder.h	\
	cut-stream-parser.h		\
	cut-stream-reader.h		\
	cut-stream.h			\
	cut-string-diff-writer.h	\
	cut-sub-process-group.h		\
	cut-sub-process.h		\
	cut-test-case.h			\
	cut-test-container.h		\
	cut-test-context.h		\
	cut-test-data.h			\
	cut-test-iterator.h		\
	cut-test-result.h		\
	cut-test-runner.h		\
	cut-test-suite.h		\
	cut-test.h			\
	cut-ui-factory-builder.h	\
	cut-ui.h			\
	cut-unified-differ.h		\
	cut-verbose-level.h

noinst_headers = 		\
	cut-crash-backtrace.h	\
	cut-elf-loader.h	\
	cut-loader.h		\
	cut-mach-o-loader.h	\
	cut-module-impl.h	\
	cut-module.h		\
	cut-pe-loader.h		\
	cut-repository.h	\
	cut-utils.h

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

# We can't use $(enum_source_prefix) here. :<
enum_sources =			\
	cut-enum-types.c	\
	cut-enum-types.h

MAINTAINERCLEANFILES =			\
	stamp-$(enum_source_prefix)-c	\
	stamp-$(enum_source_prefix)-h

BUILT_SOURCES =	$(enum_sources)

bin_PROGRAMS = cutter cut-diff

cutter_SOURCES = main.c
cut_diff_SOURCES = cut-diff.c

libcutter_sources = 			\
	cut-analyzer.c			\
	cut-assertions-helper.c		\
	cut-backtrace-entry.c		\
	cut-colorize-differ.c		\
	cut-console-diff-writer.c	\
	cut-console.c			\
	cut-contractor.c		\
	cut-crash-backtrace.c		\
	cut-diff-writer.c		\
	cut-differ.c			\
	cut-elf-loader.c		\
	cut-factory-builder.c		\
	cut-file-stream-reader.c	\
	cut-helper.c			\
	cut-iterated-test.c		\
	cut-listener.c			\
	cut-loader.c			\
	cut-mach-o-loader.c		\
	cut-main.c			\
	cut-module-factory-utils.c	\
	cut-module-factory.c		\
	cut-module.c			\
	cut-pe-loader.c			\
	cut-pipeline.c			\
	cut-process.c			\
	cut-readable-differ.c		\
	cut-report-factory-builder.c	\
	cut-report.c			\
	cut-repository.c		\
	cut-run-context.c		\
	cut-runner.c			\
	cut-sequence-matcher.c		\
	cut-stream-factory-builder.c	\
	cut-stream-parser.c		\
	cut-stream-reader.c		\
	cut-stream.c			\
	cut-string-diff-writer.c	\
	cut-sub-process-group.c		\
	cut-sub-process.c		\
	cut-test-case.c			\
	cut-test-container.c		\
	cut-test-context.c		\
	cut-test-data.c			\
	cut-test-iterator.c		\
	cut-test-result.c		\
	cut-test-runner.c		\
	cut-test-suite.c		\
	cut-test-utils-helper.c		\
	cut-test.c			\
	cut-ui-factory-builder.c	\
	cut-ui.c			\
	cut-unified-differ.c		\
	cut-utils.c			\
	cut-verbose-level.c

libcutter_la_SOURCES =			\
	$(cutter_public_headers)	\
	$(libcutter_public_headers)	\
	$(noinst_headers)		\
	$(enum_sources)			\
	$(libcutter_sources)

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
cut_diff_LDADD = libcutter.la

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

echo-private-headers:
	@for header in $(noinst_headers); do		\
	  echo $(abs_srcdir)/$${header};		\
	done

echo-public-headers:
	@for header in $(cutter_include_HEADERS); do			\
	  if test "$${header}" = "$(enum_source_prefix).h"; then	\
	    echo $(abs_builddir)/$(enum_source_prefix).h;		\
	  else								\
	    echo $(abs_srcdir)/$${header};				\
	  fi;								\
	done

echo-libcutter-sources:
	@echo $(libcutter_sources) $(enum_source_prefix).c
