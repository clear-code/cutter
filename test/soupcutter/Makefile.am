INCLUDES = 					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter			\
	-I$(top_srcdir)/soupcutter

AM_CFLAGS = $(CUTTER_CFLAGS)
AM_CFLAGS += $(LIBSOUP_CFLAGS)

CLEANFILES = *.gcno *.gcda

if HAVE_LIBSOUP
check_LTLIBRARIES =				\
	suite-soupcut-test.la			\
	test-soupcut-assertions.la		\
	test-soupcut-client.la			\
	test-soupcut-server.la
endif

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version		\
	-no-undefined

LIBS =								\
	$(top_builddir)/cutter/libcutter.la			\
	$(top_builddir)/soupcutter/libsoupcutter.la		\
	$(top_builddir)/test/lib/libcuttest-utils.la		\
	$(GLIB_LIBS)						\
	$(LIBSOUP_LIBS)

suite_soupcut_test_la_SOURCES		= suite-soupcut-test.c
test_soupcut_assertions_la_SOURCES	= test-soupcut-assertions.c
test_soupcut_client_la_SOURCES		= test-soupcut-client.c
test_soupcut_server_la_SOURCES		= test-soupcut-server.c
