cmake_minimum_required(VERSION 3.1.0)
project(minunit)

add_executable(minunit
  minunit.c
)

install(TARGETS minunit DESTINATION bin)
