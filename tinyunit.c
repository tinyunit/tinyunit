#include "tinyunit.h"

#define TU_MAX_TEST_COUNT 65536
#define TU_MAX_SUITE_COUNT 65536

#if defined(_WIN32)

#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))

/* Change POSIX C SOURCE version for pure c99 compilers */
#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200112L
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <unistd.h>  /* POSIX flags */
#include <time.h>  /* clock_gettime(), time() */
#include <sys/time.h>  /* gethrtime(), gettimeofday() */
#include <sys/resource.h>
#include <sys/times.h>

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

/* TODO: Use TLS, Thread Local Storage */
char* tu_get_last_message()
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
static double tu_timer_real()
{
#if defined(_WIN32)
  /* Windows 2000 and later. ---------------------------------- */
  LARGE_INTEGER Time;
  LARGE_INTEGER Frequency;

  QueryPerformanceFrequency(&Frequency);
  QueryPerformanceCounter(&Time);

  Time.QuadPart *= 1000000;
  Time.QuadPart /= Frequency.QuadPart;

  return (double)Time.QuadPart / 1000000.0;

#elif (defined(__hpux) || defined(hpux)) || ((defined(__sun__) || defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__)))
  /* HP-UX, Solaris. ------------------------------------------ */
  return (double)gethrtime() / 1000000000.0;

#elif defined(__MACH__) && defined(__APPLE__)
  /* OSX. ----------------------------------------------------- */
  static double timeConvert = 0.0;
  if (timeConvert == 0.0)
  {
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
    const clockid_t id = (clockid_t)-1;  /* Unknown. */
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
  return -1.0;    /* Failed. */
#endif
}

/**
* Returns the amount of CPU time used by the current process,
* in seconds, or -1.0 if an error occurred.
*/
static double tu_timer_cpu()
{
#if defined(_WIN32)
  /* Windows -------------------------------------------------- */
  FILETIME createTime;
  FILETIME exitTime;
  FILETIME kernelTime;
  FILETIME userTime;

  /* This approach has a resolution of 1/64 second. Unfortunately, Windows' API does not offer better */
  if (GetProcessTimes(GetCurrentProcess(),
    &createTime, &exitTime, &kernelTime, &userTime) != 0)
  {
    ULARGE_INTEGER userSystemTime;
    memcpy(&userSystemTime, &userTime, sizeof(ULARGE_INTEGER));
    return (double)userSystemTime.QuadPart / 10000000.0;
  }

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
  /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
  /* Prefer high-res POSIX timers, when available. */
  {
    clockid_t id;
    struct timespec ts;
#if _POSIX_CPUTIME > 0
    /* Clock ids vary by OS.  Query the id, if possible. */
    if (clock_getcpuclockid(0, &id) == -1)
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
      /* Use known clock id for AIX, Linux, or Solaris. */
      id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
      /* Use known clock id for BSD or HP-UX. */
      id = CLOCK_VIRTUAL;
#else
      id = (clockid_t)-1;
#endif
    if (id != (clockid_t)-1 && clock_gettime(id, &ts) != -1)
      return (double)ts.tv_sec +
      (double)ts.tv_nsec / 1000000000.0;
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

  return -1;    /* Failed. */
}

static int tests_count = 0;
static test_function_info_t tests[TU_MAX_TEST_COUNT];
void tu_add_test(const char* suite_name, const char* test_name, test_function_t test_ptr) {
  if (tests_count == TU_MAX_TEST_COUNT) {
    tu_printf("Please increase TU_MAX_TEST_COUNT:%d, the test:%s are not added\n", TU_MAX_TEST_COUNT, test_name);
    return;
  }
  // tu_printf("add test:%s\n", test_name);
  tests[tests_count].suite_name = suite_name;
  tests[tests_count].test_name = test_name;
  tests[tests_count].test_ptr = test_ptr;
  ++tests_count;
}

static const char * get_valid_suite_name(const char* suite_name) {
  return suite_name ? suite_name : "global_suite";
}

static int suites_count = 0;
static test_suite_info_t suites[TU_MAX_SUITE_COUNT];
void tu_add_suite(const char* suite_name, test_function_t setup, test_function_t teardown) {
  const char* suite_name_valid = get_valid_suite_name(suite_name);
  if (suites_count == TU_MAX_SUITE_COUNT) {
    tu_printf("Please increase TU_MAX_SUITE_COUNT:%d, the test:%s are not added\n", TU_MAX_SUITE_COUNT, suite_name_valid);
    return;
  }
  // tu_printf("add suite:%s\n", suite_name_valid);
  suites[suites_count].suite_name = suite_name;
  suites[suites_count].setup = setup;
  suites[suites_count].teardown = teardown;
  suites[suites_count].test_count = 0;
  ++suites_count;
}

static void tu_run_test(test_function_info_t *test_info) {
  test_suite_info_t* suite = test_info->suite;
  const char* suite_name = test_info->suite_name;
  const char* test_name = test_info->test_name;
  test_function_t test_ptr = test_info->test_ptr;
  /*  Timers */
  double timer_real = tu_timer_real();
  double timer_cpu = tu_timer_cpu();

  if (suite && suite->setup) suite->setup();
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
  if (suite && suite->teardown) suite->teardown();
  test_info->timer_real = tu_timer_real() - timer_real;
  test_info->timer_cpu = tu_timer_real() - timer_cpu;
}

typedef int (*tu_qsort_compare_t)(const void *, const void *);

static int compare_name_string(const char*a, const char* b) {
  if (a == b) {
    return 0;
  }
  if (!a) {
    return -1;
  }
  if (!b) {
    return 1;
  }
  return strcmp(a, b);
}

static int compare_suite_info(const test_suite_info_t *a, const test_suite_info_t *b) {
  return compare_name_string(a->suite_name, b->suite_name);
}

static int compare_test_info(const test_function_info_t *a, const test_function_info_t *b) {
  int x = compare_name_string(a->suite_name, b->suite_name);
  if (x == 0) {
    x = compare_name_string(a->test_name, b->test_name);
  }
  return x;
}

typedef struct tu_results {
  int total_asserts;
  double timer_real;
  double timer_cpu;
} tu_results;

static void tu_run_suites(tu_results *results) {
  int test_pos = 0;
  int suite_pos = 0;
  tu_add_suite(NULL, NULL, NULL);
  qsort(suites, suites_count, sizeof(suites[0]), (tu_qsort_compare_t)compare_suite_info);
  qsort(tests, tests_count, sizeof(tests[0]), (tu_qsort_compare_t)compare_test_info);
  results->total_asserts = 0;
  results->timer_real = 0;
  results->timer_cpu = 0;
  for (; test_pos < tests_count; ++test_pos) {
    test_function_info_t* test = tests + test_pos;
    while (suite_pos < suites_count) {
      if (compare_name_string(suites[suite_pos].suite_name, test->suite_name) == 0) {
        break;
      }
      if (suites[suite_pos].test_count == 0) {
        tu_printf("There is no tests for suite:%s\n", get_valid_suite_name(suites[suite_pos].suite_name));
      }
      ++suite_pos;
    }
    if (compare_name_string(suites[suite_pos].suite_name, test->suite_name) != 0) {
      tu_printf("Can not found suite for test:%s with suite_name:%s\n", test->test_name, get_valid_suite_name(test->suite_name));
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
static void tu_report(tu_results *results) {
  tu_printf("\n\n%d tests, %d assertions, %d failures\n", tinyunit_run, results->total_asserts, tinyunit_fail);
  tu_printf("\nFinished in %.8f seconds (real) %.8f seconds (proc)\n\n", results->timer_real, results->timer_cpu);
}

int main(int argc, char *argv[]) {
  tu_results results;
  tu_run_suites(&results);
  tu_report(&results);
  return tinyunit_fail;
}
