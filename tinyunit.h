/*
* Copyright (c) 2016 Yonggang Luo, luoyonggang@gmail.com
* Copyright (c) 2012 David Siñuela Pastor, siu.4coders@gmail.com
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef __TINYUNIT_H__
#define __TINYUNIT_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
  #pragma warning( disable : 4996)

  #if defined(_MSC_VER) && _MSC_VER < 1900
  #define snprintf _snprintf
  #define __func__ __FUNCTION__
  #endif

  //define something for Windows (32-bit and 64-bit, this part is common)
  #ifdef _WIN64
    //define something for Windows (64-bit only)
  #endif
#elif __APPLE__
  #include "TargetConditionals.h"
  #if TARGET_IPHONE_SIMULATOR
    #define tu_printf(...) fprintf(stderr, __VA_ARGS__)
  #elif TARGET_OS_IPHONE
    #define tu_printf(...) fprintf(stderr, __VA_ARGS__)
    // iOS device
  #elif TARGET_OS_MAC
      // Other kinds of Mac OS
  #else
  # error "Unknown Apple platform"
  #endif
#elif __linux__
  // linux
#elif __unix__ // all unices not caught above
  // Unix
#elif defined(_POSIX_VERSION)
  // POSIX
#else
# error "Unknown compiler"
#endif

#ifndef tu_printf
#define tu_printf(...) fprintf(stderr, __VA_ARGS__)
#endif

#define TU_EXPAND(x) x

#define TU_ARG_LENGTH_(_63, _62, _61, _60, _59, _58, _57, _56, _55, _54, _53,          \
            _52, _51, _50, _49, _48, _47, _46, _45, _44, _43, _42, _41, _40, _39, _38, \
            _37, _36, _35, _34, _33, _32, _31, _30, _29, _28, _27, _26, _25, _24, _23, \
            _22, _21, _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, _8,   \
            _7, _6, _5, _4, _3, _2, _1, count, ...)     count

#define TU_ARG_LENGTH(...)                                                              \
    TU_EXPAND(TU_ARG_LENGTH_(__VA_ARGS__,                                               \
            63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, \
            44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, \
            25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, \
            5, 4, 3, 2, 1, 0))


#ifdef __GNUC__
#define GLOBAL_STAIC_CONSTRUCT(construct) \
static void construct ## _constructor(void) __attribute__((constructor)); \
static void construct ## _constructor(void) { construct(); } \


#define GLOBAL_STAIC_DESTRUCT(destruct) \
static void destruct ## _destructor(void) __attribute__((destructor)); \
static void destruct ## _destructor(void) { destruct(); }

#elif defined (_MSC_VER)

/* Only Vs 2008 and Upper support for __pragma keyword */
/* https://msdn.microsoft.com/en-us/library/d9x1s805(v=vs.90).aspx */
#if _MSC_VER>=1500

#ifdef _M_X64
#define INCLUDE_SYM(s) comment(linker, "/include:" #s)
#else
#define INCLUDE_SYM(s) comment(linker, "/include:_" #s)
#endif

#define MSVC_DECL_SECTION_FUNCTION_POINTER(sec_name, func_name) \
__pragma(data_seg(push)) \
__pragma(section(sec_name, read)) \
__declspec(allocate(sec_name)) \
int (__cdecl *func_name ## _section)(void) = func_name; \
__pragma(data_seg(pop)) \
__pragma(INCLUDE_SYM(func_name ## _section))


#define GLOBAL_STAIC_CONSTRUCT(construct) \
static int __cdecl construct ## _constructor(void) { construct(); return 0; } \
MSVC_DECL_SECTION_FUNCTION_POINTER(".CRT$XCU", construct ## _constructor)

#define GLOBAL_STAIC_DESTRUCT(destruct) \
static int __cdecl destruct ## _destructor(void) { destruct(); return 0; } \
MSVC_DECL_SECTION_FUNCTION_POINTER(".CRT$XTU", destruct ## _destructor)

#else
#message "Visual Studio 2005 and lower unsupported"
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

/*  Maximum length of last message */
#define TU_MESSAGE_LEN 1024
/*  Accuracy with which floats are compared */
#define TU_EPSILON 1E-12

typedef void(*test_function_t)();

/*  Test setup and teardown function pointers for test suite*/
typedef struct test_suite_info_t {
  const char* suite_name;
  test_function_t setup;
  test_function_t teardown;
  int test_count;
} test_suite_info_t;

typedef struct test_function_info_t {
  const char* suite_name;
  const char* test_name;
  test_suite_info_t* suite;
  test_function_t test_ptr;
  double timer_real;
  double timer_cpu;
  int assert_count;
} test_function_info_t;

/*  Misc. counters */
extern int tinyunit_assert;
extern int tinyunit_status;

extern char* tu_get_last_message();

extern void tu_add_test(const char* suite_name, const char* test_name, test_function_t test_ptr);
extern void tu_add_suite(const char* suite_name, test_function_t setup, test_function_t teardown);

#define tu_last_message tu_get_last_message()

/*  TinyUnit test without suite */
#define _TU_TEST_1(method_name) \
static void method_name(); \
static void method_name ## _add_test() { \
  tu_add_test(NULL, "" #method_name, method_name); \
} \
GLOBAL_STAIC_CONSTRUCT(method_name ## _add_test); \
static void method_name()

/*  TinyUnit test with suite */
#define _TU_TEST_2(suite_name, method_name) \
static void method_name(); \
static void method_name ## _add_test() { \
  tu_add_test("" #suite_name, "" #method_name, method_name); \
} \
GLOBAL_STAIC_CONSTRUCT(method_name ## _add_test) \
static void method_name()

#define TU_TEST_APPLY__(n, ...) TU_EXPAND(_TU_TEST_ ## n(__VA_ARGS__))

#define TU_TEST_APPLY_(n, ...) TU_EXPAND(TU_TEST_APPLY__(n, __VA_ARGS__))

#define TU_TEST(...) TU_EXPAND(TU_TEST_APPLY_(TU_ARG_LENGTH(__VA_ARGS__), __VA_ARGS__))

#define TU_TEST_SUITE(suite_name, setup_fun, teardown_fun) \
static void suite_name ## _add_suite() { \
  tu_add_suite("" #suite_name, setup_fun, teardown_fun); \
} \
GLOBAL_STAIC_CONSTRUCT(suite_name ## _add_suite)

#define TU__SAFE_BLOCK(block) do {\
  block\
} while(0)

/*  Assertions */
#define tu_check(test) TU__SAFE_BLOCK(\
  tinyunit_assert++;\
  if (!(test)) {\
    snprintf(tu_last_message, TU_MESSAGE_LEN, "failed at %s:%s:%d:\n  %s", __func__, __FILE__, __LINE__, #test);\
    tinyunit_status = 1;\
    return;\
  } else {\
    tu_printf(".");\
  }\
)

#define tu_fail(message) TU__SAFE_BLOCK(\
  tinyunit_assert++;\
  snprintf(tu_last_message, TU_MESSAGE_LEN, "failed at %s:%s:%d:\n  %s", __func__, __FILE__, __LINE__, message);\
  tinyunit_status = 1;\
  return;\
)

#define tu_assert(test, message) TU__SAFE_BLOCK(\
  tinyunit_assert++;\
  if (!(test)) {\
    snprintf(tu_last_message, TU_MESSAGE_LEN, "failed at %s:%s:%d:\n  %s", __func__, __FILE__, __LINE__, message);\
    tinyunit_status = 1;\
    return;\
  } else {\
    tu_printf(".");\
  }\
)

#define tu_assert_int_eq(expected, result) TU__SAFE_BLOCK(\
  int tinyunit_tmp_e;\
  int tinyunit_tmp_r;\
  tinyunit_assert++;\
  tinyunit_tmp_e = (expected);\
  tinyunit_tmp_r = (result);\
  if (tinyunit_tmp_e != tinyunit_tmp_r) {\
    snprintf(tu_last_message, TU_MESSAGE_LEN, "failed at %s:%s:%d:\n  %d expected but was %d", __func__, __FILE__, __LINE__, tinyunit_tmp_e, tinyunit_tmp_r);\
    tinyunit_status = 1;\
    return;\
  } else {\
    tu_printf(".");\
  }\
)

#define tu_assert_double_eq(expected, result) TU__SAFE_BLOCK(\
  double tinyunit_tmp_e;\
  double tinyunit_tmp_r;\
  tinyunit_assert++;\
  tinyunit_tmp_e = (expected);\
  tinyunit_tmp_r = (result);\
  if (fabs(tinyunit_tmp_e-tinyunit_tmp_r) > TU_EPSILON) {\
    int tinyunit_significant_figures = 1 - (int)log10(TU_EPSILON);\
    snprintf(tu_last_message, TU_MESSAGE_LEN, "failed at %s:%s:%d:\n  %.*g expected but was %.*g", __func__, __FILE__, __LINE__, tinyunit_significant_figures, tinyunit_tmp_e, tinyunit_significant_figures, tinyunit_tmp_r);\
    tinyunit_status = 1;\
    return;\
  } else {\
    tu_printf(".");\
  }\
)

#ifdef __cplusplus
}
#endif

#endif /* __TINYUNIT_H__ */
