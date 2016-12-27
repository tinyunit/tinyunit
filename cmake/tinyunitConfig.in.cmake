get_filename_component(THIS_PACKAGE_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)
# Version info variable
set(${THIS_PACKAGE_NAME}_VERSION "@CMAKE_PROJECT_VERSION@")

set(_IMPORT_CHECK_TARGETS)
set(_IMPORT_CHECK_REQUIRED_VARS)

include("${CMAKE_CURRENT_LIST_DIR}/ConfigHelpers.cmake")
# set_and_check(${THIS_PACKAGE_NAME}_SYSCONFIG_DIR "${PACKAGE_PREFIX_DIR}/etc/${THIS_PACKAGE_NAME}/")
add_library(${THIS_PACKAGE_NAME}::tinyunit INTERFACE IMPORTED)
cpk_setup_target(tinyunit
  MAIN
  TARGET_NAME tinyunit
  TARGET_TYPE interface
)

cpk_config_finalize()
# Dump all found variables
cpk_dump_vars()
