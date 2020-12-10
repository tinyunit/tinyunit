## TinyUnit

TinyUnit is a minimal unit testing framework for C/C++ self-contained in a
single header file and a single source file. Bigger than minunit, but in other 
aspect, less boilerplate code when running tests.

It provides a way to define tests, test suites and a few handy assertion
types. It reports the summary of the number of tests run, number of assertions
and time elapsed.

Note that this project is based on:
http://www.jera.com/techinfo/jtns/jtn002.html and https://github.com/siu/minunit

## How to use it

This is a minimal test suite written with tinyunit `tests\tinyunit_readme_sample.c`, the contents are:
```
#define TINYUNIT_IMPLEMENTATION
#include "tinyunit.h"

TU_TEST(test_check_fail) {
  tu_check(5 == 7);
}
```
Compiling and running it with:
```
gcc tests\tinyunit_readme_sample.c -I. -o tinyunit_readme_sample
tinyunit_readme_sample
```

Which will produce the following output:
```
F(global_suite:test_check_fail)
failed at test_check_fail:tests\tinyunit_readme_sample.c:5:
  5 == 7


1 tests, 1 assertions, 1 failures

Finished in 0.01092470 seconds (real) 0.01562500 seconds (proc)
```

Check out `tests\tinyunit_example.c` to see a complete example.
Compiling and running it with:
```
gcc tests\tinyunit_example.c -I. -o tinyunit_example
tinyunit_example
```
Which will produce the following output:
```
........

6 tests, 8 assertions, 0 failures

Finished in 0.01255390 seconds (real) 0.00000000 seconds (proc)
```

Don't forget to add `-lrt` for the timer and `-lm` for linking the function fabs
used in tu_assert_double_eq when under linux like system.

## Setup and teardown functions

One can define setup and teardown functions and
configure the test suite to run them by using the macro
`TU_TEST_SUITE(suite_name, setup_function, teardown_fucntion)`
And for tests belong the suite, using `TU_TEST(suite_name, test_name)`
to creating a binding between the test and suite.

## Using CMake building system

### Windows
```
mkdir build & cd build
cmake -DBUILD_TESTING:BOOL=TRUE -G "Visual Studio 10 2010" ..
cmake --build . --config RelWithDebInfo
ctest -VV -C RelWithDebInfo
```

### Linux
```
mkdir build
cd build
cmake -DBUILD_TESTING:BOOL=TRUE ..
cmake --build .
ctest -VV
```

### OSX for iOS:
```
export XCODE_XCCONFIG_FILE=${tinyunit}/cmake/polly/scripts/NoCodeSign.xcconfig
cmake -DBUILD_TESTING:BOOL=TRUE -G Xcode -DCMAKE_TOOLCHAIN_FILE=../cmake/polly/ios-nocodesign-10-1.cmake ..
cmake --build . --config RelWithDebInfo
ctest -VV -C RelWithDebInfo
open /Applications/Xcode.app/Contents/Developer/Applications/Simulator.app


```

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
