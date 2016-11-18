cmake_minimum_required(VERSION 3.1.0)
option (TINYUNIT_BUILD_TESTS "Build TinyUnit tests" OFF)

project(tinyunit)

add_library(
  tinyunit
  tinyunit.h
  tinyunit.c
)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/tinyunit_add_test.cmake")

install(TARGETS tinyunit DESTINATION bin)

if (NOT TINYUNIT_BUILD_TESTS)
  return()
endif ()

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

