#define TINYUNIT_IMPLEMENTATION
#include "tinyunit.h"

TU_TEST(test_check_fail) {
  tu_check(5 == 7);
}
