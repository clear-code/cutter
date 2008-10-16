SUBDIRS = 			\
	lib			\
	fixtures		\
	gdkcutter-pixbuf

INCLUDES = 					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter

AM_CFLAGS = $(CUTTER_CFLAGS)
if HAVE_GTK
AM_CFLAGS += $(GTK_CFLAGS)
endif

EXTRA_DIST = run-test.sh
CLEANFILES = *.gcno *.gcda

TESTS = run-test.sh
TESTS_ENVIRONMENT = 		\
	NO_MAKE=yes 		\
	CUTTER="$(CUTTER)"

noinst_LTLIBRARIES = 				\
		test-cutter.la			\
		test-cut-assertions.la		\
		test-gcut-assertions.la		\
		test-cut-test-attribute.la	\
		test-cut-test.la		\
		test-cut-iterated-test.la	\
		test-cut-test-result.la		\
		test-cut-test-case.la		\
		test-cut-test-suite.la		\
		test-cut-test-context.la	\
		test-cut-loader.la		\
		test-cut-loader-suite.la	\
		test-cut-repository.la		\
		test-cut-run-context.la		\
		test-cut-test-runner.la		\
		test-cut-value-equal.la		\
		test-cut-module.la		\
		test-cut-module-factory.la	\
		test-cut-factory-builder.la	\
		test-cut-contractor.la		\
		test-cut-fork.la		\
		test-cut-process.la		\
		test-cut-pipeline.la		\
		test-cut-stream-parser.la	\
		test-cut-report-xml.la		\
		test-cut-xml-stream.la		\
		test-cut-verbose-level.la	\
		test-cut-utils.la		\
		test-cut-sequence-matcher.la	\
		test-cut-diff.la		\
		test-gcut-assertions-helper.la	\
		test-gcut-test-utils.la		\
		test-gcut-hash-table.la		\
		test-gcut-enum.la		\
		test-gcut-object.la		\
		test-gcut-list.la		\
		test-gcut-io-channel-string.la	\
		test-cut-main.la		\
		test-cut-fixture-data.la	\
		test-cut-test-data.la		\
		test-cut-test-iterator.la	\
		test-cut-sub-process.la		\
		test-cut-sub-process-group.la	\
		test-cut-file-stream-reader.la	\
		test-cut-path.la

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/test/lib/libcuttest-utils.la	\
	$(GLIB_LIBS)

EVENT_RECEIVER_LIBS = $(top_builddir)/test/lib/libcuttest-event-receiver.la

test_cutter_la_SOURCES		= test-cutter.c
test_cutter_la_LIBADD		= $(GTK_LIBS)
test_cut_assertions_la_SOURCES  = test-cut-assertions.c
test_gcut_assertions_la_SOURCES = test-gcut-assertions.c
test_cut_test_la_SOURCES        = test-cut-test.c
test_cut_iterated_test_la_SOURCES = test-cut-iterated-test.c
test_cut_test_result_la_SOURCES = test-cut-test-result.c
test_cut_test_case_la_SOURCES   = test-cut-test-case.c
test_cut_test_suite_la_SOURCES  = test-cut-test-suite.c
test_cut_loader_la_SOURCES      = test-cut-loader.c
test_cut_loader_suite_la_SOURCES = test-cut-loader-suite.c
test_cut_repository_la_SOURCES  = test-cut-repository.c
test_cut_run_context_la_SOURCES = test-cut-run-context.c
test_cut_test_runner_la_SOURCES = test-cut-test-runner.c
test_cut_value_equal_la_SOURCES = test-cut-value-equal.c
test_cut_report_xml_la_SOURCES  = test-cut-report-xml.c
test_cut_xml_stream_la_SOURCES  = test-cut-xml-stream.c
test_cut_module_la_SOURCES      = test-cut-module.c
test_cut_module_factory_la_SOURCES = test-cut-module-factory.c
test_cut_test_attribute_la_SOURCES = test-cut-test-attribute.c
test_cut_factory_builder_la_SOURCES = test-cut-factory-builder.c
test_cut_contractor_la_SOURCES  = test-cut-contractor.c
test_cut_fork_la_SOURCES  	= test-cut-fork.c
test_cut_process_la_SOURCES  	= test-cut-process.c
test_cut_pipeline_la_SOURCES  	= test-cut-pipeline.c
test_cut_stream_parser_la_SOURCES = test-cut-stream-parser.c
test_cut_stream_parser_la_LIBADD = $(EVENT_RECEIVER_LIBS)
test_cut_verbose_level_la_SOURCES = test-cut-verbose-level.c
test_cut_test_context_la_SOURCES = test-cut-test-context.c
test_cut_utils_la_SOURCES       = test-cut-utils.c
test_cut_sequence_matcher_la_SOURCES = test-cut-sequence-matcher.c
test_cut_diff_la_SOURCES        = test-cut-diff.c
test_gcut_assertions_helper_la_SOURCES = test-gcut-assertions-helper.c
test_gcut_test_utils_la_SOURCES = test-gcut-test-utils.c
test_gcut_hash_table_la_SOURCES = test-gcut-hash-table.c
test_gcut_enum_la_SOURCES       = test-gcut-enum.c
test_gcut_object_la_SOURCES     = test-gcut-object.c
test_gcut_list_la_SOURCES       = test-gcut-list.c
test_gcut_io_channel_string_la_SOURCES = test-gcut-io-channel-string.c
test_cut_main_la_SOURCES        = test-cut-main.c
test_cut_fixture_data_la_SOURCES = test-cut-fixture-data.c
test_cut_test_data_la_SOURCES = test-cut-test-data.c
test_cut_test_iterator_la_SOURCES = test-cut-test-iterator.c
test_cut_sub_process_la_SOURCES = test-cut-sub-process.c
test_cut_sub_process_group_la_SOURCES = test-cut-sub-process-group.c
test_cut_file_stream_reader_la_SOURCES = test-cut-file-stream-reader.c
test_cut_path_la_SOURCES = test-cut-path.c

echo-cutter:
	@echo $(CUTTER)
