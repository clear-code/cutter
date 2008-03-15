SUBDIRS = 			\
	loader_test_dir		\
	attribute_test_dir	\
	repository_test_dir	\
	module_test_dir		\
	factory_test_dir

INCLUDES = $(CUTTER_CFLAGS)	\
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/cutter

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = \
	NO_MAKE=yes \
	CUTTER="$(CUTTER)"

noinst_LTLIBRARIES = 				\
		cuttest_utils.la		\
		test_cut_assertions.la		\
		test_cut_gassertions.la		\
		test_cut_test_attribute.la	\
		test_cut_test.la		\
		test_cut_test_result.la		\
		test_cut_test_case.la		\
		test_cut_test_suite.la		\
		test_cut_loader.la		\
		test_cut_repository.la		\
		test_cut_value_equal.la		\
		test_cut_module.la		\
		test_cut_module_factory.la	\
		test_cut_factory_builder.la	\
		test_cut_report_xml.la

LDFLAGS =			\
	-module			\
	-rpath $(libdir)	\
	-avoid-version

cuttest_utils_la_SOURCES = 		\
			cuttest-utils.c	\
			cuttest-utils.h

test_cut_assertions_la_SOURCES  = test-cut-assertions.c cuttest-assertions.h
test_cut_gassertions_la_SOURCES = test-cut-gassertions.c cuttest-assertions.h
test_cut_test_la_SOURCES        = test-cut-test.c
test_cut_test_result_la_SOURCES = test-cut-test-result.c
test_cut_test_case_la_SOURCES   = test-cut-test-case.c
test_cut_test_suite_la_SOURCES  = test-cut-test-suite.c
test_cut_test_suite_la_LIBADD   = cuttest_utils.la
test_cut_loader_la_SOURCES      = test-cut-loader.c
test_cut_loader_la_LIBADD       = cuttest_utils.la
test_cut_repository_la_SOURCES  = test-cut-repository.c
test_cut_repository_la_LIBADD   = cuttest_utils.la
test_cut_value_equal_la_SOURCES = test-cut-value-equal.c
test_cut_report_xml_la_SOURCES  = test-cut-report-xml.c
test_cut_module_la_SOURCES      = test-cut-module.c
test_cut_module_la_LIBADD       = cuttest_utils.la
test_cut_module_factory_la_SOURCES = test-cut-module-factory.c
test_cut_module_factory_la_LIBADD  = cuttest_utils.la
test_cut_test_attribute_la_SOURCES = test-cut-test-attribute.c
test_cut_test_attribute_la_LIBADD  = cuttest_utils.la
test_cut_factory_builder_la_SOURCES = test-cut-factory-builder.c

echo-cutter:
	@echo $(CUTTER)
