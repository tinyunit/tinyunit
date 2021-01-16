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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#define TU_EXTERN_C extern "C"
#else
#define TU_EXTERN_C
#endif

#if defined(_WIN32)
#pragma warning(disable : 4996)

#if defined(_MSC_VER) && _MSC_VER < 1900
#define tu_snprintf _snprintf
#define __func__ __FUNCTION__
#endif

/* define something for Windows (32-bit and 64-bit, this part is common) */
#ifdef _WIN64
/* define something for Windows (64-bit only) */
#endif
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
#define tu_printf(...) fprintf(stderr, __VA_ARGS__)
#elif TARGET_OS_IPHONE
#define tu_printf(...) fprintf(stderr, __VA_ARGS__)
/* iOS device */
#elif TARGET_OS_MAC
/* Other kinds of Mac OS */
#else
#error "Unknown Apple platform"
#endif
#elif defined(__vxworks)
int vxworks_snprintf(char *s, size_t n, const char *format, /*args*/...);
#define tu_snprintf vxworks_snprintf
#elif __linux__
/* Linux */
#elif __unix__
/* all unices not caught above */
/* Unix */
#elif defined(_POSIX_VERSION)
/* POSIX */
#else
#error "Unknown compiler"
#endif

#ifndef tu_printf
#define tu_printf(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef tu_snprintf
#define tu_snprintf snprintf
#endif

#define TU_EXPAND(x) x

#define TU_ARG_LENGTH_(_63, _62, _61, _60, _59, _58, _57, _56, _55, _54, _53, \
                       _52, _51, _50, _49, _48, _47, _46, _45, _44, _43, _42, \
                       _41, _40, _39, _38, _37, _36, _35, _34, _33, _32, _31, \
                       _30, _29, _28, _27, _26, _25, _24, _23, _22, _21, _20, \
                       _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9,  \
                       _8, _7, _6, _5, _4, _3, _2, _1, count, ...) count

#define TU_ARG_LENGTH(...)                                                 \
  TU_EXPAND(TU_ARG_LENGTH_(__VA_ARGS__,                                    \
                           63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, \
                           51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
                           39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, \
                           27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, \
                           15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, \
                           1, 0))

#ifdef _MSC_VER

typedef int(__cdecl *tu_msvc_section_function_t)(void);

/* Only Visual Studio 2008 and Upper support for __pragma keyword */
/* https://msdn.microsoft.com/en-us/library/d9x1s805(v=vs.90).aspx */
#if _MSC_VER >= 1500
#ifdef _M_X64
#define TU_MSVC_INCLUDE_SYM(s) comment(linker, "/include:" #s)
#else
#define TU_MSVC_INCLUDE_SYM(s) comment(linker, "/include:_" #s)
#endif /* _M_X64 */

#define TU_MSVC_DECL_SECTION_FUNCTION_POINTER(sec_name, func_name)     \
  __pragma(data_seg(push))                                             \
      __pragma(section(sec_name, read)) __declspec(allocate(sec_name)) \
          TU_EXTERN_C tu_msvc_section_function_t                       \
              func_name##_section = func_name;                         \
  __pragma(data_seg(pop))                                              \
      __pragma(TU_MSVC_INCLUDE_SYM(func_name##_section))

#define TU_MODULE_CONSTRUCTOR(construct)           \
  static int __cdecl construct##_constructor(void) \
  {                                                \
    construct();                                   \
    return 0;                                      \
  }                                                \
  TU_MSVC_DECL_SECTION_FUNCTION_POINTER(".CRT$XCU", construct##_constructor)

#define TU_MODULE_DESTRUCTOR(destruct)           \
  static int __cdecl destruct##_destructor(void) \
  {                                              \
    destruct();                                  \
    return 0;                                    \
  }                                              \
  TU_MSVC_DECL_SECTION_FUNCTION_POINTER(".CRT$XTU", destruct##_destructor)

#else /* _MSC_VER >= 1500 */
#error "Visual Studio 2005 and lower unsupported"
#endif /* _MSC_VER >= 1500 */
#elif defined(__GNUC__)
#define TU_MODULE_CONSTRUCTOR(construct)                                  \
  static void construct##_constructor(void) __attribute__((constructor)); \
  static void construct##_constructor(void) { construct(); }

#define TU_MODULE_DESTRUCTOR(destruct)                                 \
  static void destruct##_destructor(void) __attribute__((destructor)); \
  static void destruct##_destructor(void) { destruct(); }
#else /* __GNUC__ */
#error "`tinyunit` isn't supported on this compiler"
#endif /* _MSC_VER */

/*  Maximum length of last message */
#define TU_MESSAGE_LEN 1024
/*  Accuracy with which floats are compared */
#define TU_EPSILON 1E-7

typedef void (*tu_test_function_t)(void);

/*  Test setup and teardown function pointers for test suite*/
typedef struct test_suite_info_t {
  const char *suite_name;
  tu_test_function_t setup;
  tu_test_function_t teardown;
  int test_count;
} test_suite_info_t;

typedef struct test_function_info_t {
  const char *suite_name;
  const char *test_name;
  test_suite_info_t *suite;
  tu_test_function_t test_ptr;
  double timer_real;
  double timer_cpu;
  int assert_count;
} test_function_info_t;

/*  Misc. counters */
extern int tinyunit_assert;
extern int tinyunit_status;

extern char *tu_get_last_message(void);

extern double tu_timer_real(void);
extern double tu_timer_cpu(void);
extern void tu_add_test(const char *suite_name, const char *test_name, tu_test_function_t test_ptr);
extern void tu_add_suite(const char *suite_name, tu_test_function_t setup, tu_test_function_t teardown);

#define tu_last_message tu_get_last_message()

/*  TinyUnit test without suite */
#define _TU_TEST_1(method_name)                      \
  static void method_name(void);                     \
  static void method_name##_add_test(void)           \
  {                                                  \
    tu_add_test(NULL, "" #method_name, method_name); \
  }                                                  \
  TU_MODULE_CONSTRUCTOR(method_name##_add_test);    \
  static void method_name(void)

/*  TinyUnit test with suite */
#define _TU_TEST_2(suite_name, method_name)                    \
  static void method_name(void);                               \
  static void method_name##_add_test(void)                     \
  {                                                            \
    tu_add_test("" #suite_name, "" #method_name, method_name); \
  }                                                            \
  TU_MODULE_CONSTRUCTOR(method_name##_add_test)               \
  static void method_name(void)

#define TU_TEST_APPLY__(n, ...) TU_EXPAND(_TU_TEST_##n(__VA_ARGS__))

#define TU_TEST_APPLY_(n, ...) TU_EXPAND(TU_TEST_APPLY__(n, __VA_ARGS__))

#define TU_TEST(...) TU_EXPAND(TU_TEST_APPLY_(TU_ARG_LENGTH(__VA_ARGS__), __VA_ARGS__))

#define TU_TEST_SUITE(suite_name, setup_fun, teardown_fun) \
  static void suite_name##_add_suite(void)                 \
  {                                                        \
    tu_add_suite("" #suite_name, setup_fun, teardown_fun); \
  }                                                        \
  TU_MODULE_CONSTRUCTOR(suite_name##_add_suite)

#define TU__SAFE_BLOCK(block) \
  do {                        \
    block                     \
  } while (0)

/*  Assertions */
#define tu_check(test) TU__SAFE_BLOCK(                  \
    tinyunit_assert++;                                  \
    if (!(test)) {                                      \
      tu_snprintf(tu_last_message, TU_MESSAGE_LEN,      \
                  "failed at %s:%s:%d:\n  %s",          \
                  __func__, __FILE__, __LINE__, #test); \
      tinyunit_status = 1;                              \
      return;                                           \
    } else {                                            \
      tu_printf(".");                                   \
    })

#define tu_fail(message) TU__SAFE_BLOCK(                \
    tinyunit_assert++;                                  \
    tu_snprintf(tu_last_message, TU_MESSAGE_LEN,        \
                "failed at %s:%s:%d:\n  %s",            \
                __func__, __FILE__, __LINE__, message); \
    tinyunit_status = 1;                                \
    return;)

#define tu_assert(test, message) TU__SAFE_BLOCK(          \
    tinyunit_assert++;                                    \
    if (!(test)) {                                        \
      tu_snprintf(tu_last_message, TU_MESSAGE_LEN,        \
                  "failed at %s:%s:%d:\n  %s",            \
                  __func__, __FILE__, __LINE__, message); \
      tinyunit_status = 1;                                \
      return;                                             \
    } else {                                              \
      tu_printf(".");                                     \
    })

#define tu_assert_int_eq(expected, result) TU__SAFE_BLOCK(                       \
    int tinyunit_tmp_e;                                                          \
    int tinyunit_tmp_r;                                                          \
    tinyunit_assert++;                                                           \
    tinyunit_tmp_e = (expected);                                                 \
    tinyunit_tmp_r = (result);                                                   \
    if (tinyunit_tmp_e != tinyunit_tmp_r) {                                      \
      tu_snprintf(tu_last_message, TU_MESSAGE_LEN,                               \
                  "failed at %s:%s:%d:\n  %d expected but was %d",               \
                  __func__, __FILE__, __LINE__, tinyunit_tmp_e, tinyunit_tmp_r); \
      tinyunit_status = 1;                                                       \
      return;                                                                    \
    } else {                                                                     \
      tu_printf(".");                                                            \
    })

#define tu_assert_longlong_eq(expected, result) TU__SAFE_BLOCK(                  \
    long long tinyunit_tmp_e;                                                    \
    long long tinyunit_tmp_r;                                                    \
    tinyunit_assert++;                                                           \
    tinyunit_tmp_e = (expected);                                                 \
    tinyunit_tmp_r = (result);                                                   \
    if (tinyunit_tmp_e != tinyunit_tmp_r) {                                      \
      tu_snprintf(tu_last_message, TU_MESSAGE_LEN,                               \
                  "failed at %s:%s:%d:\n  %lld expected but was %lld",           \
                  __func__, __FILE__, __LINE__, tinyunit_tmp_e, tinyunit_tmp_r); \
      tinyunit_status = 1;                                                       \
      return;                                                                    \
    } else {                                                                     \
      tu_printf(".");                                                            \
    })

#define tu_assert_double_eq(expected, result) TU__SAFE_BLOCK(            \
    double tinyunit_tmp_e;                                               \
    double tinyunit_tmp_r;                                               \
    tinyunit_assert++;                                                   \
    tinyunit_tmp_e = (expected);                                         \
    tinyunit_tmp_r = (result);                                           \
    if (fabs(tinyunit_tmp_e - tinyunit_tmp_r) > TU_EPSILON) {            \
      int tinyunit_significant_figures = 1 - (int)log10(TU_EPSILON);     \
      tu_snprintf(tu_last_message, TU_MESSAGE_LEN,                       \
                  "failed at %s:%s:%d:\n  %.*lf expected but was %.*lf", \
                  __func__, __FILE__, __LINE__,                          \
                  tinyunit_significant_figures, tinyunit_tmp_e,          \
                  tinyunit_significant_figures, tinyunit_tmp_r);         \
      tinyunit_status = 1;                                               \
      return;                                                            \
    } else {                                                             \
      tu_printf(".");                                                    \
    })

/* The tinyunit implementation */
#if defined(TINYUNIT_IMPLEMENTATION)

#define TU_MAX_TEST_COUNT 65536
#define TU_MAX_SUITE_COUNT 65536

#if defined(_WIN32)

typedef unsigned __int64 TU_FILETIME;

#if 0
#pragma warning(disable : 4668)
#pragma warning(disable : 4255)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_IX86)
#define _X86_
#if !defined(_CHPE_X86_ARM64_) && defined(_M_HYBRID)
#define _CHPE_X86_ARM64_
#endif
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_AMD64)
#define _AMD64_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_ARM)
#define _ARM_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_ARM64)
#define _ARM64_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_M68K)
#define _68K_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_MPPC)
#define _MPPC_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_M_IX86) && !defined(_AMD64_) && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_IA64)
#if !defined(_IA64_)
#define _IA64_
#endif /* !_IA64_ */
#endif

#ifndef _MAC
#if defined(_68K_) || defined(_MPPC_)
#define _MAC
#endif
#endif

/* windef.h also present in Visual Studio C++ 6.0 */
#include <windef.h>

__declspec(dllimport) HMODULE __stdcall LoadLibraryW(
    const wchar_t *lpLibFileName);
__declspec(dllimport) FARPROC __stdcall GetProcAddress(
    HMODULE hModule,
    const char *lpProcName);

typedef int(__stdcall *TU_QueryPerformanceFrequencyFunction)(__int64 *lpFrequency);
static TU_QueryPerformanceFrequencyFunction TU_QueryPerformanceFrequency;

typedef int(__stdcall *TU_QueryPerformanceCounterFunction)(__int64 *lpPerformanceCount);
static TU_QueryPerformanceCounterFunction TU_QueryPerformanceCounter;

typedef void *(__stdcall *TU_GetCurrentProcessFunction)(void);
static TU_GetCurrentProcessFunction TU_GetCurrentProcess;

typedef int(__stdcall *TU_GetProcessTimesFunction)(
    void *hProcess,
    TU_FILETIME *lpCreationTime,
    TU_FILETIME *lpExitTime,
    TU_FILETIME *lpKernelTime,
    TU_FILETIME *lpUserTime);
static TU_GetProcessTimesFunction TU_GetProcessTimes;

#elif defined(__vxworks) || defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))

/* Change POSIX C SOURCE version for pure c99 compilers */
#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200112L
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <sys/times.h>
#include <time.h>   /* clock_gettime(), time() */
#include <unistd.h> /* POSIX flags */
#if defined(__vxworks)
#include <tickLib.h>
#include <timers.h>

static int gettimeofday(struct timeval *tv, struct timezone *tz);
#else
#include <sys/resource.h>
#include <sys/time.h> /* gethrtime(), gettimeofday() */
#endif

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#else
#error "Unable to define timers for an unknown OS."
#endif

/*  Misc. counters */
int tinyunit_run = 0;
int tinyunit_assert = 0;
int tinyunit_fail = 0;
int tinyunit_status = 0;

/*  Last message */
static char tu_last_message_static[TU_MESSAGE_LEN];

void tu_init()
{
#if defined(_WIN32)
  HMODULE kernel32 = LoadLibraryW(L"kernel32");

  TU_QueryPerformanceFrequency = (TU_QueryPerformanceFrequencyFunction)GetProcAddress(kernel32, "QueryPerformanceFrequency");
  TU_QueryPerformanceCounter = (TU_QueryPerformanceCounterFunction)GetProcAddress(kernel32, "QueryPerformanceCounter");
  TU_GetCurrentProcess = (TU_GetCurrentProcessFunction)GetProcAddress(kernel32, "GetCurrentProcess");
  TU_GetProcessTimes = (TU_GetProcessTimesFunction)GetProcAddress(kernel32, "GetProcessTimes");
#endif
}

/* TODO: Use TLS, Thread Local Storage */
char *tu_get_last_message(void)
{
  return tu_last_message_static;
}

/*
* The following two functions were written by David Robert Nadeau
* from http://NadeauSoftware.com/ and distributed under the
* Creative Commons Attribution 3.0 Unported License
*/

/**
* Returns the real time, in seconds, or -1.0 if an error occurred.
*
* Time is measured since an arbitrary and OS-dependent start time.
* The returned real time is only useful for computing an elapsed time
* between two calls to this function.
*/
double tu_timer_real(void)
{
#if defined(_WIN32)
  /* Windows 2000 and later. ---------------------------------- */
  __int64 Time;
  __int64 Frequency;

  TU_QueryPerformanceFrequency(&Frequency);
  TU_QueryPerformanceCounter(&Time);

  return (double)Time / (Frequency * 1.0);

#elif (defined(__hpux) || defined(hpux)) || ((defined(__sun__) || defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__)))
  /* HP-UX, Solaris. ------------------------------------------ */
  return (double)gethrtime() / 1000000000.0;

#elif defined(__MACH__) && defined(__APPLE__)
  /* OSX. ----------------------------------------------------- */
  static double timeConvert = 0.0;
  if (timeConvert == 0.0) {
    mach_timebase_info_data_t timeBase;
    (void)mach_timebase_info(&timeBase);
    timeConvert = (double)timeBase.numer /
                  (double)timeBase.denom /
                  1000000000.0;
  }
  return (double)mach_absolute_time() * timeConvert;

#elif defined(_POSIX_VERSION)
  /* POSIX. --------------------------------------------------- */
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
  {
    struct timespec ts;
#if defined(CLOCK_MONOTONIC_PRECISE)
    /* BSD. --------------------------------------------- */
    const clockid_t id = CLOCK_MONOTONIC_PRECISE;
#elif defined(CLOCK_MONOTONIC_RAW)
    /* Linux. ------------------------------------------- */
    const clockid_t id = CLOCK_MONOTONIC_RAW;
#elif defined(CLOCK_HIGHRES)
    /* Solaris. ----------------------------------------- */
    const clockid_t id = CLOCK_HIGHRES;
#elif defined(CLOCK_MONOTONIC)
    /* AIX, BSD, Linux, POSIX, Solaris. ----------------- */
    const clockid_t id = CLOCK_MONOTONIC;
#elif defined(CLOCK_REALTIME)
    /* AIX, BSD, HP-UX, Linux, POSIX. ------------------- */
    const clockid_t id = CLOCK_REALTIME;
#else
    const clockid_t id = (clockid_t)-1; /* Unknown. */
#endif /* CLOCK_* */
    if (id != (clockid_t)-1 && clock_gettime(id, &ts) != -1)
      return (double)ts.tv_sec +
             (double)ts.tv_nsec / 1000000000.0;
    /* Fall thru. */
  }
#endif /* _POSIX_TIMERS */

  /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, POSIX, Solaris. ----- */
  struct timeval tm;
  gettimeofday(&tm, NULL);
  return (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
#else
  return -1.0; /* Failed. */
#endif
}

/**
* Returns the amount of CPU time used by the current process,
* in seconds, or -1.0 if an error occurred.
*/
double tu_timer_cpu(void)
{
#if defined(_WIN32)
  /* Windows -------------------------------------------------- */
  TU_FILETIME createTime;
  TU_FILETIME exitTime;
  TU_FILETIME kernelTime;
  TU_FILETIME userTime;

  /* This approach has a resolution of 1/64 second. Unfortunately, Windows' API does not offer better */
  if (TU_GetProcessTimes(TU_GetCurrentProcess(),
                         &createTime, &exitTime, &kernelTime, &userTime) != 0) {
    return (double)userTime / 10000000.0;
  }
#elif defined(__vxworks)
  return tickGet() * 1.0 / sysClkRateGet();
#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
  /* VxWorks, AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
  /* Prefer high-res POSIX timers, when available. */
  {
    clockid_t id;
    struct timespec ts;
#if _POSIX_CPUTIME > 0
    /* Clock ids vary by OS.  Query the id, if possible. */
    if (clock_getcpuclockid(0, &id) == -1)
#endif
    {
#if defined(CLOCK_PROCESS_CPUTIME_ID)
      /* Use known clock id for AIX, Linux, or Solaris. */
      id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
      /* Use known clock id for BSD or HP-UX. */
      id = CLOCK_VIRTUAL;
#else
      id = (clockid_t)-1;
#endif
    }
    if (id != (clockid_t)-1 && clock_gettime(id, &ts) != -1) {
      return (double)ts.tv_sec +
             (double)ts.tv_nsec / 1000000000.0;
    }
  }
#endif

#if defined(RUSAGE_SELF)
  {
    struct rusage rusage;
    if (getrusage(RUSAGE_SELF, &rusage) != -1)
      return (double)rusage.ru_utime.tv_sec +
             (double)rusage.ru_utime.tv_usec / 1000000.0;
  }
#endif

#if defined(_SC_CLK_TCK)
  {
    const double ticks = (double)sysconf(_SC_CLK_TCK);
    struct tms tms;
    if (times(&tms) != (clock_t)-1)
      return (double)tms.tms_utime / ticks;
  }
#endif

#if defined(CLOCKS_PER_SEC)
  {
    clock_t cl = clock();
    if (cl != (clock_t)-1)
      return (double)cl / (double)CLOCKS_PER_SEC;
  }
#endif

#endif

  return -1; /* Failed. */
}

static int tests_count = 0;
static test_function_info_t tests[TU_MAX_TEST_COUNT];

static const char *get_valid_suite_name(const char *suite_name)
{
  return suite_name ? suite_name : "global_suite";
}

void tu_add_test(const char *suite_name, const char *test_name, tu_test_function_t test_ptr)
{
  if (tests_count == TU_MAX_TEST_COUNT) {
    tu_printf("Please increase TU_MAX_TEST_COUNT:%d, the test:%s are not added\n", TU_MAX_TEST_COUNT, test_name);
    return;
  }
  /* tu_printf("add test:%s\n", test_name); */
  tests[tests_count].suite_name = get_valid_suite_name(suite_name);
  tests[tests_count].test_name = test_name;
  tests[tests_count].test_ptr = test_ptr;
  ++tests_count;
}

static volatile int suites_count = 0;
static test_suite_info_t suites[TU_MAX_SUITE_COUNT];
void tu_add_suite(const char *suite_name, tu_test_function_t setup, tu_test_function_t teardown)
{
  const char *suite_name_valid = get_valid_suite_name(suite_name);
  int count = suites_count;
  if (count == TU_MAX_SUITE_COUNT) {
    tu_printf("Please increase TU_MAX_SUITE_COUNT:%d, the test:%s are not added\n", TU_MAX_SUITE_COUNT, suite_name_valid);
  }
  suites_count += 1;

  /* tu_printf("add suite:%s\n", suite_name_valid); */
  suites[count].suite_name = suite_name_valid;
  suites[count].setup = setup;
  suites[count].teardown = teardown;
  suites[count].test_count = 0;
}

static void tu_run_test(test_function_info_t *test_info)
{
  test_suite_info_t *suite = test_info->suite;
  const char *suite_name = test_info->suite_name;
  const char *test_name = test_info->test_name;
  tu_test_function_t test_ptr = test_info->test_ptr;
  /* Timers */
  double timer_real = tu_timer_real();
  double timer_cpu = tu_timer_cpu();

  if (suite && suite->setup)
    suite->setup();
  tinyunit_assert = 0;
  tinyunit_status = 0;
  test_ptr();
  test_info->assert_count = tinyunit_assert;
  tinyunit_run++;
  if (tinyunit_status) {
    tinyunit_fail++;
    tu_printf("\nF(%s:%s)\n", get_valid_suite_name(suite_name), test_name);
    tu_printf("%s\n", tu_last_message);
  }
  fflush(stdout);
  if (suite && suite->teardown)
    suite->teardown();
  test_info->timer_real = tu_timer_real() - timer_real;
  test_info->timer_cpu = tu_timer_cpu() - timer_cpu;
}

typedef int (*tu_qsort_compare_t)(const void *, const void *);

static int compare_suite_info(const test_suite_info_t *a, const test_suite_info_t *b)
{
  return strcmp(a->suite_name, b->suite_name);
}

static int compare_test_info(const test_function_info_t *a, const test_function_info_t *b)
{
  int x = strcmp(a->suite_name, b->suite_name);
  if (x == 0) {
    x = strcmp(a->test_name, b->test_name);
  }
  return x;
}

typedef struct tu_results {
  int total_asserts;
  double timer_real;
  double timer_cpu;
} tu_results;

static void tu_run_suites(tu_results *results)
{
  int test_pos = 0;
  int suite_pos = 0;
  tu_add_suite(NULL, NULL, NULL);
  qsort(suites, suites_count, sizeof(suites[0]), (tu_qsort_compare_t)compare_suite_info);
  qsort(tests, tests_count, sizeof(tests[0]), (tu_qsort_compare_t)compare_test_info);
  results->total_asserts = 0;
  results->timer_real = 0;
  results->timer_cpu = 0;
  for (; test_pos < tests_count; ++test_pos) {
    test_function_info_t *test = tests + test_pos;
    while (suite_pos < suites_count) {
      if (strcmp(suites[suite_pos].suite_name, test->suite_name) == 0) {
        break;
      }
      if (suites[suite_pos].test_count == 0) {
        tu_printf("There is no tests for suite:%s\n", get_valid_suite_name(suites[suite_pos].suite_name));
      }
      ++suite_pos;
    }
    if (strcmp(suites[suite_pos].suite_name, test->suite_name) != 0) {
      tu_printf("Can not found suite for test:%s with suite_name:%s\n",
                test->test_name, get_valid_suite_name(test->suite_name));
      continue;
    }
    test->suite = suites + suite_pos;
    test->suite->test_count++;
    tu_run_test(test);
    results->timer_real += test->timer_real;
    results->timer_cpu += test->timer_cpu;
    results->total_asserts += test->assert_count;
  }
}

/*  Report */
static void tu_report(tu_results *results)
{
  tu_printf("\n\n%d tests, %d assertions, %d failures\n",
            tinyunit_run, results->total_asserts, tinyunit_fail);
  tu_printf("\nFinished in %.8f seconds (real) %.8f seconds (proc)\n\n",
            results->timer_real, results->timer_cpu);
}

#if defined(__vxworks)

static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  int ret;
  struct timespec tp;

  if ((ret = clock_gettime(CLOCK_REALTIME, &tp)) == 0) {
    tv->tv_sec = tp.tv_sec;
    tv->tv_usec = (tp.tv_nsec + 500) / 1000; /* convert nanoseconds to microseconds */
  }
  return ret;
}

static int vxworks_vsnprintf(char *s, size_t n, const char *format, va_list ap)
{
  /* do not check size of buffer */
  return vsprintf(s, format, ap);
}

int vxworks_snprintf(char *s, size_t n, const char *format, /*args*/...)
{
  va_list ap;
  int ret;

  va_start(ap, format);

  ret = vxworks_vsnprintf(s, n, format, ap);

  va_end(ap);

  return ret;
}

#endif /* __vxworks */

int main(int argc, char *argv[])
{
  tu_results results;
  tinyunit_run = 0;
  tinyunit_assert = 0;
  tinyunit_fail = 0;
  tinyunit_status = 0;
  tu_init();
  tu_run_suites(&results);
  tu_report(&results);
  return tinyunit_fail;
}

#endif /* TINYUNIT_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* __TINYUNIT_H__ */
