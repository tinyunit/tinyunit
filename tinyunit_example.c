#include "tinyunit.h"

static int foo = 0;
static int bar = 0;
static double dbar = 0.1;

void test_setup() {
  foo = 7;
  bar = 4;
}

void test_teardown() {
  /* Nothing */
}

TU_TEST(test_check) {
  tu_check(foo == 7);
}

TU_TEST(test_check_fail) {
  tu_check(foo != 7);
}

TU_TEST(test_assert) {
  tu_assert(foo == 7, "foo should be 7");
}

TU_TEST(test_assert_fail) {
  tu_assert(foo != 7, "foo should be <> 7");
}

TU_TEST(test_assert_int_eq) {
  tu_assert_int_eq(4, bar);
}

TU_TEST(test_assert_int_eq_fail) {
  tu_assert_int_eq(5, bar);
}

TU_TEST(test_assert_double_eq) {
  tu_assert_double_eq(0.1, dbar);
}

TU_TEST(test_assert_double_eq_fail) {
  tu_assert_double_eq(0.2, dbar);
}

TU_TEST(test_fail) {
  tu_fail("Fail now!");
}

TU_TEST(main_suite, test_check_with_suite) {
  tu_check(foo == 7);
}

TU_TEST(main_suite, test_check_fail_with_suite) {
  tu_check(foo != 7);
}

TU_TEST_SUITE(main_suite, test_setup, test_teardown)
