cmake_minimum_required (VERSION 3.1)

macro(temp_name fname)
  if(${ARGC} GREATER 1) # Have to escape ARGC to correctly compare
    set(_base ${ARGV1})
  else(${ARGC} GREATER 1)
    set(_base ".cmake-tmp")
  endif(${ARGC} GREATER 1)
  set(_counter 0)
  while(EXISTS "${_base}${_counter}")
    math(EXPR _counter "${_counter} + 1")
  endwhile(EXISTS "${_base}${_counter}")
  set(${fname} "${_base}${_counter}")
endmacro(temp_name)


if ( NOT ("$ENV{EFFECTIVE_PLATFORM_NAME}" EQUAL "") )
  set(EFFECTIVE_PLATFORM_NAME $ENV{EFFECTIVE_PLATFORM_NAME})
endif ()

if ( NOT ("$ENV{WIND_BASE}" EQUAL "") )
  set(WIND_BASE $ENV{WIND_BASE})
endif ()

set(TINYUNIT_TEST_FINAL_CMD ${TINYUNIT_TEST_CMD})
if (DEFINED EFFECTIVE_PLATFORM_NAME)
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
elseif (DEFINED WIND_BASE)
  set(TINYUNIT_TEST_VXWROKS_INSTALL_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/scripts/install-vxworks.bat")
  temp_name(TINYUNIT_TEST_VXWROKS_GDB_INSTALL_SCRIPT ".cmake.vxworks/gdb")
  file(WRITE ${TINYUNIT_TEST_VXWROKS_GDB_INSTALL_SCRIPT} "target wtx target\nload ${TINYUNIT_TEST_CMD}\nq\n")

  set(TINYUNIT_TEST_VXWORKS_INSTALL_COMMAND
    gdbi86 --command "${TINYUNIT_TEST_VXWROKS_GDB_INSTALL_SCRIPT}"
  )

  execute_process (COMMAND ${TINYUNIT_TEST_VXWORKS_INSTALL_COMMAND}
    TIMEOUT 10000
    RESULT_VARIABLE VXWORKS_INSTALL_RETURN_VAL
  )
  if (NOT(VXWORKS_INSTALL_RETURN_VAL EQUAL 0))
    message (FATAL_ERROR "Installing ${TINYUNIT_TEST_VXWORKS_INSTALL_COMMAND} failed with:\n${VXWORKS_INSTALL_RETURN_VAL}")
  endif ()

  temp_name(TINYUNIT_TEST_VXWROKS_WINDSH_RUN_SCRIPT ".cmake.vxworks/windsh")
  file(WRITE ${TINYUNIT_TEST_VXWROKS_WINDSH_RUN_SCRIPT} "main()\nexit\n")

  set(TINYUNIT_TEST_FINAL_CMD windsh target -poll 1 -q -s "${TINYUNIT_TEST_VXWROKS_WINDSH_RUN_SCRIPT}")
endif ()

message ("Running test ${TINYUNIT_TEST_FINAL_CMD}")
execute_process (COMMAND ${TINYUNIT_TEST_FINAL_CMD}
  OUTPUT_VARIABLE TEST_OUTPUT_TEXT
  ERROR_VARIABLE TEST_ERROR_TEXT
)

set (TEST_RESULT_STRING "stdout:${TEST_OUTPUT_TEXT}\nstderr:${TEST_ERROR_TEXT}\n")

string(REGEX REPLACE "^.*TU_TEST_RETVAL_BEGIN:([0-9]+):TU_TEST_RETVAL_END.*$"
      "\\1" TEST_RETURN_VAL ${TEST_RESULT_STRING})

if (TEST_RETURN_VAL STREQUAL ${TINYUNIT_TEST_RETVAL})
  message ("Tests passed with:\n${TEST_RESULT_STRING}")
else ()
  message (FATAL_ERROR "Tests failed with:\n${TEST_RESULT_STRING}")
endif()
