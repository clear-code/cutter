
/* Generated data (by glib-mkenums) */

#include "cut-enum-types.h"
#include "cut-public.h"
#include "cut-private.h"
#include "cut-runner.h"
#include "cut-run-context.h"
#include "cut-test-runner.h"
#include "cut-listener.h"
#include "cut-listener-utils.h"
#include "cut-main.h"
#include "cut-test.h"
#include "cut-iterated-test.h"
#include "cut-test-container.h"
#include "cut-test-iterator.h"
#include "cut-test-case.h"
#include "cut-test-suite.h"
#include "cut-test-context.h"
#include "cut-test-result.h"
#include "cut-test-data.h"
#include "cut-ui.h"
#include "cut-contractor.h"
#include "cut-factory-builder.h"
#include "cut-report-factory-builder.h"
#include "cut-stream-factory-builder.h"
#include "cut-ui-factory-builder.h"
#include "cut-module-factory.h"
#include "cut-module-factory-utils.h"
#include "cut-report.h"
#include "cut-stream.h"
#include "cut-process.h"
#include "cut-pipeline.h"
#include "cut-stream-parser.h"
#include "cut-verbose-level.h"
#include "cut-sequence-matcher.h"
#include "cut-diff.h"
#include "cut-differ-readable.h"
#include "cut-differ-colorize.h"
#include "cut-console-colors.h"
#include "cut-analyzer.h"
#include "cut-stream-reader.h"
#include "cut-file-stream-reader.h"
#include "cut-sub-process.h"
#include "cut-sub-process-group.h"
#include "cut-backtrace-entry.h"

/* enumerations from "cut-public.h" */
GType
cut_test_result_status_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { CUT_TEST_RESULT_INVALID, "CUT_TEST_RESULT_INVALID", "invalid" },
      { CUT_TEST_RESULT_SUCCESS, "CUT_TEST_RESULT_SUCCESS", "success" },
      { CUT_TEST_RESULT_NOTIFICATION, "CUT_TEST_RESULT_NOTIFICATION", "notification" },
      { CUT_TEST_RESULT_OMISSION, "CUT_TEST_RESULT_OMISSION", "omission" },
      { CUT_TEST_RESULT_PENDING, "CUT_TEST_RESULT_PENDING", "pending" },
      { CUT_TEST_RESULT_FAILURE, "CUT_TEST_RESULT_FAILURE", "failure" },
      { CUT_TEST_RESULT_ERROR, "CUT_TEST_RESULT_ERROR", "error" },
      { CUT_TEST_RESULT_CRASH, "CUT_TEST_RESULT_CRASH", "crash" },
      { CUT_TEST_RESULT_LAST, "CUT_TEST_RESULT_LAST", "last" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("CutTestResultStatus", values);
  }
  return etype;
}
GType
cut_order_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { CUT_ORDER_NONE_SPECIFIED, "CUT_ORDER_NONE_SPECIFIED", "none-specified" },
      { CUT_ORDER_NAME_ASCENDING, "CUT_ORDER_NAME_ASCENDING", "name-ascending" },
      { CUT_ORDER_NAME_DESCENDING, "CUT_ORDER_NAME_DESCENDING", "name-descending" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("CutOrder", values);
  }
  return etype;
}
GType
cut_test_context_error_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { CUT_TEST_CONTEXT_ERROR_FATAL, "CUT_TEST_CONTEXT_ERROR_FATAL", "fatal" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("CutTestContextError", values);
  }
  return etype;
}
GType
cut_pipeline_error_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { CUT_PIPELINE_ERROR_PIPE, "CUT_PIPELINE_ERROR_PIPE", "pipe" },
      { CUT_PIPELINE_ERROR_COMMAND_LINE, "CUT_PIPELINE_ERROR_COMMAND_LINE", "command-line" },
      { CUT_PIPELINE_ERROR_SPAWN, "CUT_PIPELINE_ERROR_SPAWN", "spawn" },
      { CUT_PIPELINE_ERROR_CHILD_PID, "CUT_PIPELINE_ERROR_CHILD_PID", "child-pid" },
      { CUT_PIPELINE_ERROR_IO_ERROR, "CUT_PIPELINE_ERROR_IO_ERROR", "io-error" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("CutPipelineError", values);
  }
  return etype;
}
GType
cut_verbose_level_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { CUT_VERBOSE_LEVEL_INVALID, "CUT_VERBOSE_LEVEL_INVALID", "invalid" },
      { CUT_VERBOSE_LEVEL_SILENT, "CUT_VERBOSE_LEVEL_SILENT", "silent" },
      { CUT_VERBOSE_LEVEL_NORMAL, "CUT_VERBOSE_LEVEL_NORMAL", "normal" },
      { CUT_VERBOSE_LEVEL_VERBOSE, "CUT_VERBOSE_LEVEL_VERBOSE", "verbose" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("CutVerboseLevel", values);
  }
  return etype;
}
GType
cut_verbose_level_error_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { CUT_VERBOSE_LEVEL_ERROR_BAD_VALUE, "CUT_VERBOSE_LEVEL_ERROR_BAD_VALUE", "value" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("CutVerboseLevelError", values);
  }
  return etype;
}
GType
cut_sequence_match_operation_type_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { CUT_SEQUENCE_MATCH_OPERATION_EQUAL, "CUT_SEQUENCE_MATCH_OPERATION_EQUAL", "equal" },
      { CUT_SEQUENCE_MATCH_OPERATION_INSERT, "CUT_SEQUENCE_MATCH_OPERATION_INSERT", "insert" },
      { CUT_SEQUENCE_MATCH_OPERATION_DELETE, "CUT_SEQUENCE_MATCH_OPERATION_DELETE", "delete" },
      { CUT_SEQUENCE_MATCH_OPERATION_REPLACE, "CUT_SEQUENCE_MATCH_OPERATION_REPLACE", "replace" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("CutSequenceMatchOperationType", values);
  }
  return etype;
}
GType
cut_stream_reader_error_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { CUT_STREAM_READER_ERROR_READ, "CUT_STREAM_READER_ERROR_READ", "read" },
      { CUT_STREAM_READER_ERROR_PARSE, "CUT_STREAM_READER_ERROR_PARSE", "parse" },
      { CUT_STREAM_READER_ERROR_END_PARSE, "CUT_STREAM_READER_ERROR_END_PARSE", "end-parse" },
      { CUT_STREAM_READER_ERROR_IO_ERROR, "CUT_STREAM_READER_ERROR_IO_ERROR", "io-error" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("CutStreamReaderError", values);
  }
  return etype;
}
GType
cut_file_stream_reader_error_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { CUT_FILE_STREAM_READER_ERROR_FILE, "CUT_FILE_STREAM_READER_ERROR_FILE", "file" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("CutFileStreamReaderError", values);
  }
  return etype;
}

/* Generated data ends here */

