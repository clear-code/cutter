SUBDIRS = 			\
	lib			\
	fixtures

INCLUDES = 			\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter	\
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
		test_cutter.la			\
		test_cut_assertions.la		\
		test_gcut_assertions.la		\
		test_cut_test_attribute.la	\
		test_cut_test.la		\
		test_cut_iterated_test.la	\
		test_cut_test_result.la		\
		test_cut_test_case.la		\
		test_cut_test_suite.la		\
		test_cut_test_context.la	\
		test_cut_loader.la		\
		test_cut_loader_suite.la	\
		test_cut_repository.la		\
		test-cut-run-context.la		\
		test_cut_test_runner.la		\
		test_cut_value_equal.la		\
		test_cut_module.la		\
		test_cut_module_factory.la	\
		test_cut_factory_builder.la	\
		test_cut_contractor.la		\
		test_cut_fork.la		\
		test_cut_process.la		\
		test_cut_pipeline.la		\
		test_cut_stream_parser.la	\
		test_cut_report_xml.la		\
		test_cut_xml_stream.la		\
		test_cut_verbose_level.la	\
		test_cut_utils.la		\
		test_cut_sequence_matcher.la	\
		test_cut_diff.la		\
		test_gcut_assertions_helper.la	\
		test_gcut_test_utils.la		\
		test_cut_main.la		\
		test_cut_fixture_data.la	\
		test_cut_test_data.la		\
		test_cut_test_iterator.la	\
		test_cut_sub_process.la		\
		test_cut_sub_process_group.la	\
		test_cut_file_stream_reader.la

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/test/lib/libcuttest_utils.la	\
	$(GLIB_LIBS)

EVENT_RECEIVER_LIBS = $(top_builddir)/test/lib/libcuttest_event_receiver.la


test_cutter_la_SOURCES		= test-cutter.c
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
test_cut_main_la_SOURCES        = test-cut-main.c
test_cut_fixture_data_la_SOURCES = test-cut-fixture-data.c
test_cut_test_data_la_SOURCES = test-cut-test-data.c
test_cut_test_iterator_la_SOURCES = test-cut-test-iterator.c
test_cut_sub_process_la_SOURCES = test-cut-sub-process.c
test_cut_sub_process_group_la_SOURCES = test-cut-sub-process-group.c
test_cut_file_stream_reader_la_SOURCES = test-cut-file-stream-reader.c

echo-cutter:
	@echo $(CUTTER)
