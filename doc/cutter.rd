= cutter / Cutter / Cutter's manual

== NAME

cutter - xUnit family unit testing framework for C

== SYNOPSIS

(({cutter})) [((*option ...*))] ((*test-directory*))

(({cutter})) --mode=analyze [((*option ...*))] ((*log-directory*))

== DESCRIPTION

Cutter is a xUnit family unit testing framework for
C. Cutter provides programmers two important interfaces:

  (1) easy to write API
  (2) easy to debug UI

Cutter helps programmers to write their new tests, run their
existing tests, get feedbacks from ran their
tests.

((*test-directory*)) is a directory which has test_*.so.
test_*.so are searched recursively.

((*log-directory*)) is a directory which has Cutter log
files. Cutter logs test results when --stream-directory
option is specified.

== Options

: --version

   Cutter shows its own version and exits.

: --mode=[test|analyze]

   It specifies run mode. Cutter runs tests when run mode is
   test. Cutter analyzes test results when run mode is
   analyze.

   The default is test.

: -s DIRECTORY, --source-directory=DIRECTORY

   Cutter prepends DIRECTORY to file name when test fails. This
   is for tolls (like Emacs) which have function jumping to
   error line.

: -t TEST_CASE_NAME, --test-case=TEST_CASE_NAME

   Cutter runs test cases that are matched with
   TEST_CASE_NAME. If TEST_CASE_NAME is surrounded by "/"
   (e.g. /test_/), TEST_CASE_NAME is handled as regular
   expression.

   This option can be specified n times. In the case, Cutter
   runs test cases that are matched with any
   TEST_CASE_NAME. (OR)

: -n TEST_NAME, --name=TEST_NAME

   Cutter runs tests that are matched with TEST_NAME. If
   TEST_NAME is surrounded by "/" (e.g. /test_/), TEST_NAME
   is handled as regular expression.

   This option can be specified n times. In the case, Cutter
   runs test that are matched with any TEST_NAME. (OR)

: -m, --multi-thread

   Cutter runs a test case in a new thread.

   The default is off.

: --max-threads=MAX_THREADS

   Run test cases and iterated tests with MAX_THREADS
   threads concurrently at a maximum. -1 means no limit.

   The default is 10.

: --test-case-order=[none|name|name-desc]

   It specifies test case order.

   If 'none' is specified, Cutter doesn't sort. If 'name' is
   specified, Cutter sorts test cases by name in
   ascending order. If 'name-desc' is specified, Cutter
   sorts test cases by name in descending order.

   The default is none.

: --exclude-file=FILE

   Cutter doesn't read FILE on test collecting.

: --exclude-directory=DIRECTORY

   Cutter doesn't search tests under DIRECTORY.

: --fatal-failures

   Cutter treats failures as fatal problem. It means that
   Cutter stops test run.

   The default is off.

: --keep-opening-modules

   Cutter keeps opening loaded modules to resolve symbols
   for debugging.

   The default is off.

: --enable-convenience-attribute-definition

   It enables convenience but danger
   "#{ATTRIBUTE_NAME}_#{TEST_NAME - 'test_' PREFIX}"
   attribute set function.

   The default is off.

: -u[console|gtk], --ui=[console|gtk]

   It specifies UI.

   The default is console UI.

: -v[s|silent|n|normal|v|verbose], --verbose=[s|silent|n|normal|v|verbose]

   It specifies verbose level.

   This option is only for console UI.

: -c[yes|true|no|false|auto], --color=[yes|true|no|false|auto]

   If 'yes' or 'true' is specified, Cutter uses colorized
   output by escape sequence. If 'no' or 'false' is
   specified, Cutter never use colorized output. If 'auto'
   or the option is omitted, Cutter uses colorized output if
   available.

   This option is only for console UI.

: --pdf-report=FILE

   Cutter outputs a test report to FILE as PDF format.

: --xml-report=FILE

   Cutter outputs a test report to FILE as XML format.

: --stream=[xml]

   It specifies stream backend. Stream backend streams
   test results.

   There is only XML stream backend for now.

: --stream-fd=FILE_DESCRIPTOR

   It specifies output file descriptor. The XML stream
   backend streams to FILE_DESCRIPTOR.

   This option is only for XML stream backend.

: --stream-directory=DIRECTORY

   It specifies output directory. The XML stream backend
   streams to a file under DIRECTORY.

   This option is only for XML stream backend.

: -?, --help

   Cutter shows common options.

: --help-stream

   Cutter shows stream related options.

: --help-report

   Cutter shows report related options.

: --help-ui

   Cutter shows UI related options.

: --help-all

   Cutter shows all options.

== EXIT STATUS

The exit status is 0 if all tests are passed and non-0
otherwise.  Omission test result and notification test
result are not treated as failure.

== FILES

: /usr/local/share/doc/cutter/

   The directory has Cutter documents. e.g. reference
   manual, tutorial and so on.

== EXAMPLE

In the following example, cutter runs tests under tests/
directory and shows test progress verbosely.

  % cutter -v v test/

In the following example, test results are saved under logs/
directory:

  % cutter --stream=xml --stream-directory=logs/ test/

== SEE ALSO

GLib's reference manual.
