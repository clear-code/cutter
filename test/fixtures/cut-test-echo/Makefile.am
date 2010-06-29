INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

AM_CFLAGS = $(CUTTER_CFLAGS)

noinst_PROGRAMS = cut-test-echo

cut_test_echo_LDADD = $(GLIB_LIBS)
cut_test_echo_SOURCES = cut-test-echo.c
