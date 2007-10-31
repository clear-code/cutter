# -*- rd -*-

= README.en

$Id: README.en 14 2004-09-14 05:23:13Z kou $

== Author

Kouhei Sutou <kou@cozmixng.org>

== License

LGPL

== Mailing list

Send mail that To field is cozdev@cozmixng.quickml.com , Cc
field is kou@cozmixng.org and Subject and Body contain
somthig to
((<"cozdev@cozmixng.quickml.com"|URL:mailto:cozdev@cozmixng.quickml.com?cc=kou@cozmixng.org&subject=Join!>)).

Note that this mailing list's main language is Japanese. But
English mail is welcome too.

== What's this?

Cutter is Unit Testing Framework for C. Cutter is based on
((<CUnit|URL:http://www.gethos.net/opensource/cunit>)).
There are some documents of CUnit under the CUnit/ directory.

The feature of Cutter is to making test as shared library.

== Dependency libraries

None.

=== Suggested programs

  * run-test.el which is attached to
    ((<GaUnit|URL:http://www.cozmixng.org/~rwiki/?cmd=view;name=GaUnit>)).
    (If you want to run test with ease on the Emacs.)

== Get

((<URL:http://www.cozmixng.org/~kou/download/cutter.tar.gz>))

  % svn co http://www.cozmixng.org/repos/c/cutter/trunk cutter

== Install

  % ./configure
  % make
  # make install

== Usage

  % cutter [Options] [Directory which has libtest_*.so]

=== Options

: -vLEVEL, --verbose=LEVEL

   It specifies verbose level. LEVEL is silent (s), normal (n),
   progress (p) or verbose (v). Output is more verbosely in
   the back.
   
: -bBASE, --base=BASE
   
   Cutter adds BASE before file name when test fails. This
   is for tolls (like Emacs) which have function jumping to
   error line.

== How to make test

Executing flow of test is the following.

  (1) Make test.
  (2) Compile it and make libtest_*.so.
  (3) Execute cutter. It loads libtest_*.so and tests.

See sample/calc/.

== References

=== Assertions

This is the list of UT_ASSERT*. (({message})) in the
arguments is displayed when assertion fails.

--- UT_PASS
    
    It always passes.

--- UT_FAIL(message)
    
    It always fails.

--- UT_ASSERT(expect, message)
    
    It passes when (({expect})) is not 0.

--- UT_ASSERT_EQUAL_INT(expect, actual, message)
    
    It passes when (({expect})) equals (({actual})). They
    must be int type or variant of int (like long).

--- UT_ASSERT_EQUAL_FLOAT(expect, actual, message)
    
    It passes when (({expect})) equals (({actual})). They
    must be float type or variant of float (like double).

--- UT_ASSERT_EQUAL_STRING(expect, actual, message)
    
    It passes when (({expect})) and (({actual})) are same
    content string.

=== Template

The following is template of test.

  #include <cutter/cutter.h>
  
  #include "HEADER_FILE_OF_YOUR_PROGRAM"
  
  UT_DEF(TEST_NAME_1)
  {
    UT_ASSERT_SOMETHING;
    ...
    UT_PASS; /* test is passed when reach it!! */
  }
  
  UT_DEF(TEST_NAME_2)
  {
    ...
  }
  
  ...
  
  /* register tests */
  UT_REGISTER_BEGIN("TEST_NAME") /* don't add `;' */
  UT_REGISTER(TEST_NAME_1, "description of test1") /* don't add `;' */
  UT_REGISTER(TEST_NAME_2, "description of test2") /* don't add `;' */
  ...
  UT_REGISTER_END

