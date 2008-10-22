INCLUDES = 					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter

AM_CFLAGS = $(CUTTER_CFLAGS)

EXTRA_DIST = run-test.sh
CLEANFILES = *.gcno *.gcda

noinst_LTLIBRARIES = 			\
	test-gcut-assertions.la		\
	test-gcut-assertions-helper.la	\
	test-gcut-test-utils.la		\
	test-gcut-hash-table.la		\
	test-gcut-enum.la		\
	test-gcut-object.la		\
	test-gcut-list.la		\
	test-gcut-io-channel-string.la	\
	test-gcut-spawn.la		\
	test-gcut-io.la

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/test/lib/libcuttest-utils.la	\
	$(GLIB_LIBS)

test_gcut_assertions_la_SOURCES		= test-gcut-assertions.c
test_gcut_assertions_helper_la_SOURCES	= test-gcut-assertions-helper.c
test_gcut_test_utils_la_SOURCES		= test-gcut-test-utils.c
test_gcut_hash_table_la_SOURCES		= test-gcut-hash-table.c
test_gcut_enum_la_SOURCES		= test-gcut-enum.c
test_gcut_object_la_SOURCES		= test-gcut-object.c
test_gcut_list_la_SOURCES		= test-gcut-list.c
test_gcut_io_channel_string_la_SOURCES	= test-gcut-io-channel-string.c
test_gcut_spawn_la_SOURCES		= test-gcut-spawn.c
test_gcut_io_la_SOURCES			= test-gcut-io.c