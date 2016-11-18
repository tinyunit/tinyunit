#include "minunit.h"

#define MU_MAX_TEST_COUNT 65536
#define MU_MAX_SUITE_COUNT 65536

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
int minunit_run = 0;
int minunit_assert = 0;
int minunit_fail = 0;
int minunit_status = 0;

/*  Last message */
static char mu_last_message_static[MINUNIT_MESSAGE_LEN];

/* TODO: Use TLS, Thread Local Storage */
char* mu_get_last_message()
{
  return mu_last_message_static;
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
double mu_timer_real()
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
double mu_timer_cpu()
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

static int tets_count = 0;
static test_function_info_t tests[MU_MAX_TEST_COUNT];
void mu_add_test(const char* suite_name, const char* test_name, test_function_t test_ptr) {
  if (tets_count == MU_MAX_TEST_COUNT) {
    printf("Please increase MU_MAX_TEST_COUNT:%d, the test:%s are not added\n", MU_MAX_TEST_COUNT, test_name);
    return;
  }
  printf("add test:%s\n", test_name);
  tests[tets_count].suite_name = suite_name;
  tests[tets_count].test_name = test_name;
  tests[tets_count].test_ptr = test_ptr;
  ++tets_count;
}

static int suite_count = 0;
static test_suite_info_t suites[MU_MAX_SUITE_COUNT];
void mu_add_suite(const char* suite_name, test_function_t setup, test_function_t teardown) {
  if (suite_count == MU_MAX_SUITE_COUNT) {
    printf("Please increase MU_MAX_SUITE_COUNT:%d, the test:%s are not added\n", MU_MAX_SUITE_COUNT, suite_name);
    return;
  }
  printf("add suite:%s\n", suite_name);
  suites[suite_count].suite_name = suite_name;
  suites[suite_count].setup = setup;
  suites[suite_count].teardown = teardown;
  ++suite_count;
}

void mu_run_test(test_function_info_t *test_info) {
  test_suite_info_t* suite = test_info->suite;
  const char* suite_name = test_info->suite_name;
  const char* test_name = test_info->test_name;
  test_function_t test_ptr = test_info->test_ptr;
  /*  Timers */
  double timer_real = mu_timer_real();
  double timer_cpu = mu_timer_cpu();
  if (suite && suite->setup) suite->setup();
  minunit_assert = 0;
  minunit_status = 0;
  test_ptr();
  test_info->assert_count = minunit_assert;
    minunit_run++;
  if (minunit_status) {
    minunit_fail++;
    if (suite_name) {
      printf("F:%s:%s\n", suite_name, test_name);
    } else {
      printf("F:%s\n", test_name);
    }
    printf("%s\n", mu_last_message);
  }
  fflush(stdout);
  if (suite && suite->teardown) suite->teardown();
  test_info->timer_real = mu_timer_real() - timer_real;
  test_info->timer_cpu = mu_timer_real() - timer_cpu;
}

void mu_run_tests() {
}


/*  Report */
void mu_report() {
  printf("\n\n%d tests, %d assertions, %d failures\n", minunit_run, minunit_assert, minunit_fail);
  /*
  printf("\nFinished in %.8f seconds (real) %.8f seconds (proc)\n\n",
    minunit_end_real_timer - minunit_real_timer,
    minunit_end_proc_timer - minunit_proc_timer);*/
}

int main(int argc, char *argv[]) {
  mu_run_tests();
  mu_report();
  return 0;
}
