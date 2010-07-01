INCLUDES = 					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter			\
	-I$(top_srcdir)/test/lib

AM_CFLAGS = $(CUTTER_CFLAGS)

CLEANFILES = *.gcno *.gcda

check_LTLIBRARIES = 			\
	test-gcut-assertions.la		\
	test-gcut-assertions-helper.la	\
	test-gcut-test-utils.la		\
	test-gcut-hash-table.la		\
	test-gcut-enum.la		\
	test-gcut-object.la		\
	test-gcut-list.la		\
	test-gcut-string.la		\
	test-gcut-string-io-channel.la	\
	test-gcut-egg.la		\
	test-gcut-process.la		\
	test-gcut-io.la			\
	test-gcut-key-file.la		\
	test-gcut-inspect.la		\
	test-gcut-dynamic-data.la	\
	test-gcut-value-equal.la	\
	test-gcut-types.la

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
test_gcut_string_SOURCES		= test-gcut-string.c
test_gcut_string_io_channel_la_SOURCES	= test-gcut-string-io-channel.c
test_gcut_egg_la_SOURCES		= test-gcut-egg.c
test_gcut_process_la_SOURCES		= test-gcut-process.c
test_gcut_io_la_SOURCES			= test-gcut-io.c
test_gcut_key_file_la_SOURCES		= test-gcut-key-file.c
test_gcut_inspect_la_SOURCES		= test-gcut-inspect.c
test_gcut_dynamic_data_la_SOURCES	= test-gcut-dynamic-data.c
test_gcut_value_equal_la_SOURCES	= test-gcut-value-equal.c
test_gcut_types_la_SOURCES		= test-gcut-types.c
