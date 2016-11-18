cmake_minimum_required(VERSION 3.1.0)
project(tinyunit)

add_executable(tinyunit
  tinyunit.h
  tinyunit.c
  tinyunit_example.c
)

install(TARGETS tinyunit DESTINATION bin)
