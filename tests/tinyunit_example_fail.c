#define TINYUNIT_IMPLEMENTATION
#include "tinyunit.h"

static int foo = 7;
static int bar = 4;
static double dbar = 0.1;

TU_TEST(test_check_fail)
{
  tu_check(foo != 7);
}

TU_TEST(test_assert_fail)
{
  tu_assert(foo != 7, "foo should be <> 7");
}

TU_TEST(test_assert_int_eq_fail)
{
  tu_assert_int_eq(5, bar);
}

TU_TEST(test_assert_double_eq_fail)
{
  tu_assert_double_eq(0.2, dbar);
}

TU_TEST(test_fail)
{
  tu_fail("Fail now!");
}

static int foo_suite = 0;

static void test_setup(void)
{
  foo_suite = 7;
}

static void test_teardown(void)
{
  /* Nothing */
}

TU_TEST(main_suite, test_check_fail_with_suite)
{
  tu_check(foo_suite != 7);
}

TU_TEST_SUITE(main_suite, test_setup, test_teardown)
