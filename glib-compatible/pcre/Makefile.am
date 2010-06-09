INCLUDES = \
	-DG_LOG_DOMAIN=\"GLib-GRegex\" \
	-DSUPPORT_UCP \
	-DSUPPORT_UTF8 \
	-DNEWLINE=-1 \
	-DMATCH_LIMIT=10000000 \
	-DMATCH_LIMIT_RECURSION=10000000 \
	-DMAX_NAME_SIZE=32 \
	-DMAX_NAME_COUNT=10000 \
	-DMAX_DUPLENGTH=30000 \
	-DLINK_SIZE=2 \
	-DPOSIX_MALLOC_THRESHOLD=10 \
	-DPCRE_STATIC \
	$(GLIB_CFLAGS)

if GLIB_2_12
noinst_LTLIBRARIES = libpcre.la
endif

libpcre_la_SOURCES = \
	pcre_compile.c \
	pcre_chartables.c \
	pcre_config.c \
	pcre_dfa_exec.c \
	pcre_exec.c \
	pcre_fullinfo.c \
	pcre_get.c \
	pcre_globals.c \
	pcre_info.c \
	pcre_maketables.c \
	pcre_newline.c \
	pcre_ord2utf8.c \
	pcre_refcount.c \
	pcre_study.c \
	pcre_tables.c \
	pcre_try_flipped.c \
	pcre_ucp_searchfuncs.c \
	pcre_valid_utf8.c \
	pcre_version.c \
	pcre_xclass.c \
	pcre.h \
	pcre_internal.h \
	ucp.h \
	ucpinternal.h

EXTRA_DIST =				\
		COPYING
