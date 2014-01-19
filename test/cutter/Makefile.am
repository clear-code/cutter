AM_CPPFLAGS =					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/cutter			\
	-I$(top_srcdir)/gcutter

AM_CFLAGS =			\
	$(CUTTER_CFLAGS)	\
	$(GTK_CFLAGS)

AM_CXXFLAGS = $(AM_CFLAGS)

CLEANFILES = *.gcno *.gcda

check_LTLIBRARIES =			\
	test-cutter.la			\
	test-cut-assertions.la		\
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
	test-cut-readable-differ.la	\
	test-cut-unified-differ.la	\
	test-cut-main.la		\
	test-cut-fixture-data.la	\
	test-cut-test-data.la		\
	test-cut-test-iterator.la	\
	test-cut-sub-process.la		\
	test-cut-sub-process-group.la	\
	test-cut-file-stream-reader.la	\
	test-cut-path.la		\
	test-cut-test-utils.la		\
	test-cut-thread.la		\
	test-cut-logger.la		\
	test-cut-cpp-helpers.la

AM_LDFLAGS =			\
	-module			\
	-rpath `pwd`/.libs	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/cutter/libcutter.la		\
	$(top_builddir)/test/lib/libcuttest-utils.la	\
	$(GLIB_LIBS)					\
	$(GTK_LIBS)					\
	$(WINDOWS_LIBS)

EVENT_RECEIVER_LIBS = $(top_builddir)/test/lib/libcuttest-event-receiver.la

test_cutter_la_SOURCES			= test-cutter.c
test_cut_assertions_la_SOURCES		= test-cut-assertions.c
test_cut_test_la_SOURCES		= test-cut-test.c
test_cut_iterated_test_la_SOURCES	= test-cut-iterated-test.c
test_cut_test_result_la_SOURCES		= test-cut-test-result.c
test_cut_test_case_la_SOURCES		= test-cut-test-case.c
test_cut_test_suite_la_SOURCES		= test-cut-test-suite.c
test_cut_loader_la_SOURCES		= test-cut-loader.c
test_cut_loader_suite_la_SOURCES	= test-cut-loader-suite.c
test_cut_repository_la_SOURCES		= test-cut-repository.c
test_cut_run_context_la_SOURCES		= test-cut-run-context.c
test_cut_test_runner_la_SOURCES		= test-cut-test-runner.c
test_cut_report_xml_la_SOURCES		= test-cut-report-xml.c
test_cut_xml_stream_la_SOURCES		= test-cut-xml-stream.c
test_cut_module_la_SOURCES		= test-cut-module.c
test_cut_module_factory_la_SOURCES	= test-cut-module-factory.c
test_cut_test_attribute_la_SOURCES	= test-cut-test-attribute.c
test_cut_factory_builder_la_SOURCES	= test-cut-factory-builder.c
test_cut_contractor_la_SOURCES		= test-cut-contractor.c
test_cut_fork_la_SOURCES		= test-cut-fork.c
test_cut_process_la_SOURCES		= test-cut-process.c
test_cut_pipeline_la_SOURCES		= test-cut-pipeline.c
test_cut_stream_parser_la_SOURCES	= test-cut-stream-parser.c
test_cut_stream_parser_la_LIBADD	= $(EVENT_RECEIVER_LIBS)
test_cut_verbose_level_la_SOURCES	= test-cut-verbose-level.c
test_cut_test_context_la_SOURCES	= test-cut-test-context.c
test_cut_utils_la_SOURCES		= test-cut-utils.c
test_cut_sequence_matcher_la_SOURCES	= test-cut-sequence-matcher.c
test_cut_readable_differ_la_SOURCES	= test-cut-readable-differ.c
test_cut_unified_differ_la_SOURCES	= test-cut-unified-differ.c
test_cut_main_la_SOURCES		= test-cut-main.c
test_cut_fixture_data_la_SOURCES	= test-cut-fixture-data.c
test_cut_test_data_la_SOURCES		= test-cut-test-data.c
test_cut_test_iterator_la_SOURCES	= test-cut-test-iterator.c
test_cut_sub_process_la_SOURCES		= test-cut-sub-process.c
test_cut_sub_process_group_la_SOURCES	= test-cut-sub-process-group.c
test_cut_file_stream_reader_la_SOURCES	= test-cut-file-stream-reader.c
test_cut_path_la_SOURCES		= test-cut-path.c
test_cut_test_utils_la_SOURCES		= test-cut-test-utils.c
test_cut_thread_la_SOURCES		= test-cut-thread.c
test_cut_logger_la_SOURCES		= test-cut-logger.c
test_cut_cpp_helpers_la_SOURCES		= test-cut-cpp-helpers.cpp

echo-tests:
	@echo $(noinst_LTLIBRARIES)
