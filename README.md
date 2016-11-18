## TinyUnit

TinyUnit is a minimal unit testing framework for C/C++ self-contained in a
single header file and a single source file. Bigger than tinyunit, but at some 
aspect, more usefull.

It provides a way to define and configure test suites and a few handy assertion
types.  It reports the summary of the number of tests run, number of assertions
and time elapsed.

Note that this project is based on:
http://www.jera.com/techinfo/jtns/jtn002.html and https://github.com/siu/minunit

## How to use it

This is a minimal test suite written with tinyunit (named with tinyunit_readme_sample.c):

  #include "tinyunit.h"

  TU_TEST(test_check) {
    tu_check(5 == 7);
  }

Which will produce the following output:

  F
  test_check failed:
    readme_sample.c:4: 5 == 7


  1 tests, 1 assertions, 1 failures

  Finished in 0.00032524 seconds (real) 0.00017998 seconds (proc)

Check out tinyunit_example.c to see a complete example. Compile with something
like:

  gcc tinyunit.c tinyunit_example.c -lrt -lm -o tinyunit_example

Don't forget to add -lrt for the timer and -lm for linking the function fabs
used in tu_assert_double_eq.

## Setup and teardown functions

One can define setup and teardown functions and configure the test suite to run
them by using the macro `TU_TEST_SUITE(suite_name, setup_function, teardown_fucntion)`
And for tests belong the suite, using `TU_TEST(suite_name, test_name)` to creating 
a binding between the test and suite.

## Assertion types

tu_check(condition): will pass if the condition is evaluated to true, otherwise
it will show the condition as the error message

tu_fail(message): will fail and show the message

tu_assert(condition, message): will pass if the condition is true, otherwise it
will show the failed condition and the message

tu_assert_int_eq(expected, result): it will pass if the two numbers are
equal or show their values as the error message

tu_assert_double_eq(expected, result): it will pass if the two values
are almost equal or show their values as the error message. The value of
TU_EPSILON sets the threshold to determine if the values are close enough.

## Authors

Yonggang Luo <luoyonggang@gmail.com>
