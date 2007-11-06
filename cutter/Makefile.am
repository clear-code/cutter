INCLUDES = $(GLIB_CFLAGS)	\
	 -I$(top_builddir)	\
	 -I$(top_srcdir)

EXTRA_DIST = config.h.in

pkginclude_HEADERS = cut-assertion.h cutter.h

cutter_h_sources =				\
	cut-context.h				\
	cut-output.h

enum_source_prefix = cut-enum-types

enum_sources_h = \
	$(cutter_h_sources)

enum_sources =			\
	$(enum_source_prefix).c	\
	$(enum_source_prefix).h

MAINTAINERCLEANFILES =			\
	stamp-$(enum_source_prefix)-c	\
	stamp-$(enum_source_prefix)-h

BUILT_SOURCES = $(enum_sources)

bin_PROGRAMS = cutter

cutter_SOURCES =			\
		$(cutter_h_sources)	\
		cut-enum-types.c	\
		cut-assertion.h		\
		cut-test.h		\
		cut-test.c		\
		cut-context-private.h	\
		cut-context.c		\
		cut-test-container.h	\
		cut-test-container.c	\
		cut-test-case.h		\
		cut-test-case.c		\
		cut-test-suite.h	\
		cut-test-suite.c	\
		cut-loader.h		\
		cut-loader.c		\
		cut-repository.h	\
		cut-repository.c	\
		cut-output.c		\
		cut-main.c

cutter_LDADD =				\
	$(GLIB_LIBS)


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
