cmake_minimum_required(VERSION 3.1.0)
project(minunit)

add_executable(minunit
  minunit.h
  minunit.c
  minunit_example.c
)

install(TARGETS minunit DESTINATION bin)
