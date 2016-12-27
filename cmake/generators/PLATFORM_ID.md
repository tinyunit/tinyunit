$<PLATFORM_ID:platform_ids>
where platform_ids is a comma-separated(,) list. 1 if the CMake’s platform id matches any one of the entries in platform_ids, otherwise 0. See also the CMAKE_SYSTEM_NAME variable.

https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#id8
It's test the value of CMAKE_SYSTEM_NAME
CMAKE_SYSTEM_NAME:
  Linux
  Windows
  Darwin
  eCos
  WindowsCE
  VxWorks
