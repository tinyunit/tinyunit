
set(tinyunit_tests_include_directories
  ${CMAKE_CURRENT_LIST_DIR}/..
)

cpk_add_test(
  NAME tinyunit_example
  SOURCES tinyunit_example.c
  INCLUDE_DIRECTORIES ${tinyunit_tests_include_directories}
)

cpk_add_test(
  NAME tinyunit_example_fail
  SOURCES tinyunit_example_fail.c
  RESULT_VARIABLE 6
  INCLUDE_DIRECTORIES ${tinyunit_tests_include_directories}
)
