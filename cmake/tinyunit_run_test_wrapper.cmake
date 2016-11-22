cmake_minimum_required (VERSION 3.1)

if ( NOT ("$ENV{EFFECTIVE_PLATFORM_NAME}" EQUAL "") )
  set(EFFECTIVE_PLATFORM_NAME $ENV{EFFECTIVE_PLATFORM_NAME})
endif ()

set(TINYUNIT_TEST_FINAL_CMD ${TINYUNIT_TEST_CMD})
if ( DEFINED EFFECTIVE_PLATFORM_NAME )
  string(REPLACE "\$\{EFFECTIVE_PLATFORM_NAME\}" "${EFFECTIVE_PLATFORM_NAME}"
    TINYUNIT_TEST_CMD
    ${TINYUNIT_TEST_CMD}
  )
  set(IOS_DECICE_ID $ENV{IOS_DECICE_ID})
  get_filename_component(TINYUNIT_TEST_IOS_INSTALL_BUNDLE ${TINYUNIT_TEST_CMD} DIRECTORY)
  message("TINYUNIT_TEST_IOS_INSTALL_BUNDLE ${TINYUNIT_TEST_IOS_INSTALL_BUNDLE}")
  set(TINYUNIT_TEST_IOS_INSTALL_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/scripts/install-ios.sh")
  set(TINYUNIT_TEST_IOS_INSTALL_COMMAND bash -l -c "sh ${TINYUNIT_TEST_IOS_INSTALL_SCRIPT} ${IOS_DECICE_ID} ${TINYUNIT_TEST_IOS_INSTALL_BUNDLE}" )

  execute_process (COMMAND ${TINYUNIT_TEST_IOS_INSTALL_COMMAND}
    TIMEOUT 30000
    RESULT_VARIABLE IOS_INSTALL_RETURN_VAL
  )
  if (NOT(IOS_INSTALL_RETURN_VAL EQUAL 0))
    message (FATAL_ERROR "Installing ${TINYUNIT_TEST_IOS_INSTALL_COMMAND} failed with:\n${IOS_INSTALL_RETURN_VAL}")
  endif ()

  set(TINYUNIT_TEST_FINAL_CMD xcrun simctl launch --console ${IOS_DECICE_ID} com.example)
endif ()

message ("Running test ${TINYUNIT_TEST_FINAL_CMD}")
execute_process (COMMAND ${TINYUNIT_TEST_FINAL_CMD}
  RESULT_VARIABLE TEST_RETURN_VAL
  OUTPUT_VARIABLE TEST_OUTPUT_TEXT
  ERROR_VARIABLE TEST_ERROR_TEXT
)

set (TEST_RESULT_STRING "retval:${TEST_RETURN_VAL}\nstdout:${TEST_OUTPUT_TEXT}\nstderr:${TEST_ERROR_TEXT}\n")

if (TEST_RETURN_VAL EQUAL ${TINYUNIT_TEST_RETVAL})
  message ("Tests passed with:\n${TEST_RESULT_STRING}")
else ()
  message (FATAL_ERROR "Tests failed with:\n${TEST_RESULT_STRING}")
endif()
