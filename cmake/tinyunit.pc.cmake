prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
includedir=${prefix}/include
libdir=${exec_prefix}/lib

Name: tinyunit
Description: This is tinyunit header library
Version: @CMAKE_PROJECT_VERSION@
Cflags: -I${includedir}/tinyunit
