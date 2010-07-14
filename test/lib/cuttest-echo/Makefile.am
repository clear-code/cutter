INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

AM_CFLAGS = $(CUTTER_CFLAGS)

noinst_PROGRAMS = cuttest-echo

cuttest_echo_LDADD = $(GLIB_LIBS)
cuttest_echo_SOURCES = cuttest-echo.c
