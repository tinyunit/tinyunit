cmake_minimum_required (VERSION 3.1)

message("Running test ${TINYUNIT_TEST_CMD}")
execute_process (COMMAND "${TINYUNIT_TEST_CMD}"
  RESULT_VARIABLE TEST_RETURN_VAL
  OUTPUT_VARIABLE TEST_OUTPUT_TEXT
  ERROR_VARIABLE TEST_ERROR_TEXT
)

if (NOT TEST_RETURN_VAL EQUAL ${TINYUNIT_TEST_RETVAL} AND NOT TEST_RETURN_VAL EQUAL 77)
  message (FATAL_ERROR "Test has failed with: stdout:${TEST_OUTPUT_TEXT} stderr:${TEST_ERROR_TEXT} retval:${TEST_RETURN_VAL}")
else ()
  message ("Test result stdout:${TEST_OUTPUT_TEXT} stderr:${TEST_ERROR_TEXT} retval:${TEST_RETURN_VAL}")
endif()
