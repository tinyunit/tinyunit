cmake_minimum_required(VERSION 3.1.0)
project(tinyunit)

add_library(
  tinyunit
  tinyunit.h
  tinyunit.c
)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/tinyunit_add_test.cmake")

include(CTest)

set(tinyunit_example_retval 7)
tinyunit_add_test(
  tinyunit_example
  tinyunit_example.c
)

set(tinyunit_readme_sample_retval 1)
tinyunit_add_test(
  tinyunit_readme_sample
  tinyunit_readme_sample.c
)

install(TARGETS tinyunit DESTINATION bin)
