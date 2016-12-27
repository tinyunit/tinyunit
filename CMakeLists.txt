cmake_minimum_required(VERSION 3.17)

project(tinyunit VERSION 0.1.0.0 LANGUAGES C CXX)

add_library(tinyunit INTERFACE)
set(TINYUNIT_PUBLIC_HEADERS
  tinyunit.h
)
set_target_properties(tinyunit PROPERTIES PUBLIC_HEADER "${TINYUNIT_PUBLIC_HEADERS}")
install(TARGETS tinyunit PUBLIC_HEADER DESTINATION include/${CMAKE_PROJECT_NAME})

include(cmake/CpkHelpers.cmake)
cpk_install_pkg_config()

if(BUILD_TESTING)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
