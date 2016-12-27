#define TINYUNIT_IMPLEMENTATION
#include "tinyunit.h"

TU_TEST(test_tu_length)
{
  int count = 0;
  count = TU_ARG_LENGTH(1);
  tu_check(count == 1);
  count = TU_ARG_LENGTH(1, 2);
  tu_check(count == 2);

  /* Note: don't support for TU_ARG_LENGTH with zero args */
  count = TU_ARG_LENGTH();
  tu_check(count == 1);
}

static int foo = 7;
static int bar = 4;
static double dbar = 0.1;

TU_TEST(test_check)
{
  tu_check(foo == 7);
}

TU_TEST(test_assert)
{
  tu_assert(foo == 7, "foo should be 7");
}

TU_TEST(test_assert_int_eq)
{
  tu_assert_int_eq(4, bar);
}

TU_TEST(test_assert_double_eq)
{
  tu_assert_double_eq(0.1, dbar);
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

TU_TEST(main_suite, test_check_with_suite)
{
  tu_check(foo_suite == 7);
}

TU_TEST_SUITE(main_suite, test_setup, test_teardown)
