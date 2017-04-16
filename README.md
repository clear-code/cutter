# -*- rd -*-

= README --- An introduction of Cutter, a Unit Testing Framework for C and C++

== Name

Cutter

== Author

=== Program

  * Kouhei Sutou <kou@clear-code.com>
  * Hiroyuki Ikezoe <ikezoe@clear-code.com>
  * Yuto Hayamizu <y.hayamizu@gmail.com>

=== Kinotan Icons

  * Mayu & Co.

== License

  * Source: LGPLv3 or later. (detail:
    ((<"license/lgpl-3.txt"|URL:http://www.gnu.org/licenses/lgpl.html>)))
  * Document and kinotan icons: Triple license: LGPL, GFDL and/or CC.
    * LGPL: v3 or later. (detail:
      ((<"license/lgpl-3.txt"|URL:http://www.gnu.org/licenses/lgpl.html>)))
    * GFDL: v1.3 or later. (detail:
      ((<"license/gfdl-1.3.txt"|URL:http://www.gnu.org/licenses/fdl.html>)))
    * CC: ((<BY-SA|URL:http://creativecommons.org/licenses/by-sa/3.0/>))
  * Exceptions:
    * glib-compatible/glibintl.h, glib-compatible/gregex.*,
      glib-compatible/gscripttable.h, glib-compatible/gsequence.*,
      glib-compatible/gstring.*, glib-compatible/gunicode.h,
      glib-compatible/guniprop.c: LGPL v2.0 or later.
      (detail:
      ((<"glib-compatible/COPYING"|URL:http://www.gnu.org/licenses/old-licenses/lgpl-2.0.html>)))
    * glib-compatible/pcre/: PCRE LICENSE.
      (detail:
      ((<"glib-compatible/pcre/COPYING"|URL:http://www.pcre.org/licence.txt>)))
    * html/blog.*, html/download.*, html/heading-mark.*
      html/install.*, html/readme.*, html/reference.*,
      html/tango-logo.png, html/tutorial.*, html/mini-*.svg:
      Public domain. They are deliverables by ((<Tango Desktop
      Project|URL:http://tango.freedesktop.org/>)). (Some of
      them are modified.)
    * html/ja.png, html/us.png, html/famfamfam-logo.png:
      Public domain. They are distributed by
      ((<famfamfam.com|URL:http://famfamfam.com/>)).

== What's this?

Cutter is a xUnit family Unit Testing Framework for C and
C++.

This is a list of features of Cutter:
  * easy to write tests.
  * outputs result with useful format for debugging.
  * tests are built as shared libraries.

See ((<FEATURES>)) for more details.

== Dependency libraries

  * GLib >= 2.16

== Install

See ((<Install>)).

== Repository

There is the repository at ((<"clear-code/cutter on GitHub"|URL:https://github.com/clear-code/cutter/>)).

  % git clone https://github.com/clear-code/cutter.git

== Usage

  % cutter [OPTION ...] TEST_DIRECTORY

TEST_DIRECTORY should have test_*.so. test_*.so are searched
recursively.

See ((<cutter|"doc/cutter.rd">)) for more details.

== How to test

Executing flow of test is the following.

  (1) Write a test.
  (2) Compile it and build test_*.so.
  (3) Execute cutter. It loads test_*.so and runs them.

See ((<a tutorial|TUTORIAL>)) and sample/stack/.

== Test result

Here is an example test result:

  ..........F.................................................

  1) Failure: test_test_case_count
  <1 == cut_test_case_get_n_tests(test_object, NULL)>
  expected: <1>
   but was: <0>
  test/test-cut-test-case.c:143: test_test_case_count()

  Finished in 0.020857 seconds

  60 test(s), 253 assertion(s), 1 failure(s), 0 error(s), 0 pending(s), 0 notification(s)

=== Progress

A part that contains "." and "F" of the test result shows
test progress:

  ..........F.................................................

Each "." and "F" shows a test case (test function). "."
shows a test case that is succeeded and "F" shows a test
case that is failed. There are "E", "P" and "N". They shows
error, pending and notification respectively. Here is a
summary of test case marks:

: .
   A succeeded test

: F
   A failed test

: E
   A test that had an error

: P
   A test that is marked as pending

: N
   A test that had an notification

The above marks are showed after each test is finished. We
can confirm the test progress from the output in testing.

=== Summary of test result

Cutter outputs a summary of test result after all tests are
finished. The first of a summary is a list of a detail of
test result of non-succeeded test. In the example, cutter
outputs a detail of test result because there is a failure.

  1) Failure: test_test_case_count
  <1 == cut_test_case_get_n_tests(test_object, NULL)>
  expected: <1>
   but was: <0>
  test/test-cut-test-case.c:143: test_test_case_count()

In the example, test_test_case_count test case is failed. We
expected that cut_test_case_get_n_tests(test_object, NULL)
is 1 but was 0. The failed assertion is in
test_test_case_count() function in test/test-cut-test-case.c
at 143th line.

Elapsed time for testing is showed after a list of a detail
of test result:

  Finished in 0.020857 seconds

The last line is an summary of test result:

  60 test(s), 253 assertion(s), 1 failure(s), 0 error(s), 0 pending(s), 0 notification(s)

Here are the means of each output:

: n test(s)

   n test case(s) (test function(s)) are run.

: n assertion(s)

   n assertion(s) are passed.

: n failure(s)

   n assertion(s) are failed.

: n error(s)

   n error(s) are occurred (cut_error() is used n times)

: n pending(s)

   n test case(s) are pending (cut_pending() is used n times)

: n notification(s)

   n notification(s) are occurred (cut_notification() is used n times)

In the example, 60 test cases are run, 253 assertions are
passed and an assertion is failed. There are no error,
pending, notification.

=== XML report

Cutter reports test result as XML format if --xml-report
option is specified. A reported XML has the following
structure:

  <report>
    <!-- "result" tag is generated for a test result.
          Normally, a "result" tag is generated for a test.
          If you use cut_message(), you will get two or more "result" tags
          for a test. -->
    <result>
      <test-case>
        <name>TEST CASE NAME</name>
        <description>DESCRIPTION OF TEST CASE (if exists)</description>
        <start-time>START TIME OF TEST CASE (ISO 8601 format) [e.g.: 2013-11-12T03:32:56.691676Z]</start-time>
        <elapsed>ELAPSED TIME OF TEST CASE (in seconds) [e.g.: 0.030883]</elapsed>
      </test-case>
      <test>
        <name>TEST NAME</name>
        <description>DESCRIPTION OF TEST CASE (if exists)</description>
        <start-time>START TIME OF TEST (ISO 8601 format) [e.g.: 2013-11-12T03:32:56.691823Z]</start-time>
        <elapsed>ELAPSED TIME OF TEST (in seconds) [e.g.: 0.030883]</elapsed>
        <option><!-- ATTRIBUTE INFORMATION (if exists) -->
          <name>ATTRIBUTE NAME [e.g.: bug]</name>
          <value>ATTRIBUTE VALUE [e.g.: 1234]</value>
        </option>
        <option>
          ...
        </option>
        ...
      </test>
      <status>TEST RESULT (one of them: success, notification, omission, pending, failure, error, crash)</status>
      <detail>DETAIL OF TEST RESULT (if exists)</detail>
      <backtrace><!-- BACKTRACE (if exists) -->
        <entry>
          <file>FILE NAME</file>
          <line>LINE</line>
          <info>ADDITIONAL INFORMATION</info>
        </entry>
        <entry>
          ...
        </entry>
      </backtrace>
      <start-time>START TIME OF TEST (ISO 8601 format) [e.g.: 2013-11-12T03:32:56.691823Z]</start-time>
      <elapsed>ELAPSED TIME OF TEST (in seconds) [e.g.: 0.030883]</elapsed>
    </result>
    <result>
      ...
    </result>
    ...
  </report>

=== Test coverage

You can see the code coverage with Cutter if your system
have ((<LTP tools|URL:http://ltp.sourceforge.net/>)).  To
see the coverage, add the followling line in your
configure.ac and type "make coverage".

  AC_CHECK_COVERAGE

== References

=== Assertions

See ((<"cutter/cut-assertions.h"|cutter-cut-assertions.html>)).

=== Attributes

You can add attributes to your test to get more useful
information on failure. For example, you can add Bug ID like
the following

  void attributes_invalid_input(void);
  void test_invalid_input(void);

  void
  attributes_invalid_input (void)
  {
      cut_set_attributes("bug", "123");
  }

  void
  test_invalid_input (void)
  {
      cut_assert_equal("OK", get_input());
  }

In the above example, test_invalid_input test has an
attribute that the test is for Bug #123.

You need to define a function whose name is
"attributes_#{TEST_NAME - 'test_' PREFIX}" to add
attributes to a test. In the above example, attributes set
function, "attributes_invalid_input", is defined to set
"bug" attribute to "test_invalid_input" test.

=== Template

The following is a template of test.

  #include <cutter.h>
  
  #include "HEADER_FILE_OF_YOUR_PROGRAM"
  
  void test_condition(void);
  void test_strstr(void);

  static int condition = 0;
  
  void
  cut_setup (void)
  {
      condition = 1;
  }
  
  void
  cut_teardown (void)
  {
      condition = 0;
  }

  void
  test_condition(void)
  {
      cut_set_message("The condition value should be set to 1 in cut_setup()");
      cut_assert_equal_int(1, condition);
    ...
  }
  
  void
  test_strstr(void)
  {
      cut_assert_equal_string("sub-string",
                              strstr("string sub-string", "sub"));
      ...
  }
  
== Thanks

  * Kazumasa Matsunaga: reported a build bug.
  * Daijiro MORI: reported bugs.
  * UNNO Hideyuki:
    * reported a document bug.
    * assisted Solaris install document.
  * gunyara-kun: suggested API design.
  * Yamakawa Hiroshi: reported works on Cygwin.
  * Yoshinori K. Okuji:
    * reported locale related bugs.
    * suggested a new feature.
  * Zed Shaw: reported bugs.
  * Romuald Conty: reported a document bug.
  * Romain Tartière:
    * reported bugs.
    * suggested improvements.
  * Ilya Barygin:
    * reported bugs in test.
  * Hiroaki Nakamura:
    * reported a bug on CentOS.
  * Tobias Gruetzmacher:
    * fixed a GTK+ test runner bug.
