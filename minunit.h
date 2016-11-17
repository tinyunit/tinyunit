/*
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
#ifndef __MINUNIT_H__
#define __MINUNIT_H__

#ifdef __cplusplus
	extern "C" {
#endif

#if defined(_WIN32)

#if defined(_MSC_VER) && _MSC_VER < 1900
  #define snprintf _snprintf
  #define __func__ __FUNCTION__
#endif

#endif /* _WIN32 */


#include <stdio.h>
#include <math.h>

/*  Maximum length of last message */
#define MINUNIT_MESSAGE_LEN 1024
/*  Accuracy with which floats are compared */
#define MINUNIT_EPSILON 1E-12

/*  Misc. counters */
extern int minunit_run;
extern int minunit_assert;
extern int minunit_fail;
extern int minunit_status;

/*  Timers */
extern double minunit_real_timer;
extern double minunit_proc_timer;

/*  Test setup and teardown function pointers */
extern void (*minunit_setup)();
extern void (*minunit_teardown)();

extern char* minunit_get_last_message();
extern double mu_timer_real();
extern double mu_timer_cpu();

#define minunit_last_message minunit_get_last_message()

/*  Definitions */
#define MU_TEST(method_name) static void method_name()
#define MU_TEST_SUITE(suite_name) static void suite_name()

#define MU__SAFE_BLOCK(block) do {\
	block\
} while(0)

/*  Run test suite and unset setup and teardown functions */
#define MU_RUN_SUITE(suite_name) MU__SAFE_BLOCK(\
	suite_name();\
	minunit_setup = NULL;\
	minunit_teardown = NULL;\
)

/*  Configure setup and teardown functions */
#define MU_SUITE_CONFIGURE(setup_fun, teardown_fun) MU__SAFE_BLOCK(\
	minunit_setup = setup_fun;\
	minunit_teardown = teardown_fun;\
)

/*  Test runner */
#define MU_RUN_TEST(test) MU__SAFE_BLOCK(\
	if (minunit_real_timer==0 && minunit_proc_timer==0) {\
		minunit_real_timer = mu_timer_real();\
		minunit_proc_timer = mu_timer_cpu();\
	}\
	if (minunit_setup) (*minunit_setup)();\
	minunit_status = 0;\
	test();\
	minunit_run++;\
	if (minunit_status) {\
		minunit_fail++;\
		printf("F");\
		printf("\n%s\n", minunit_last_message);\
	}\
	fflush(stdout);\
	if (minunit_teardown) (*minunit_teardown)();\
)

/*  Report */
#define MU_REPORT() MU__SAFE_BLOCK(\
	double minunit_end_real_timer;\
	double minunit_end_proc_timer;\
	printf("\n\n%d tests, %d assertions, %d failures\n", minunit_run, minunit_assert, minunit_fail);\
	minunit_end_real_timer = mu_timer_real();\
	minunit_end_proc_timer = mu_timer_cpu();\
	printf("\nFinished in %.8f seconds (real) %.8f seconds (proc)\n\n",\
		minunit_end_real_timer - minunit_real_timer,\
		minunit_end_proc_timer - minunit_proc_timer);\
)

/*  Assertions */
#define mu_check(test) MU__SAFE_BLOCK(\
	minunit_assert++;\
	if (!(test)) {\
		snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, #test);\
		minunit_status = 1;\
		return;\
	} else {\
		printf(".");\
	}\
)

#define mu_fail(message) MU__SAFE_BLOCK(\
	minunit_assert++;\
	snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, message);\
	minunit_status = 1;\
	return;\
)

#define mu_assert(test, message) MU__SAFE_BLOCK(\
	minunit_assert++;\
	if (!(test)) {\
		snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, message);\
		minunit_status = 1;\
		return;\
	} else {\
		printf(".");\
	}\
)

#define mu_assert_int_eq(expected, result) MU__SAFE_BLOCK(\
	int minunit_tmp_e;\
	int minunit_tmp_r;\
	minunit_assert++;\
	minunit_tmp_e = (expected);\
	minunit_tmp_r = (result);\
	if (minunit_tmp_e != minunit_tmp_r) {\
		snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "%s failed:\n\t%s:%d: %d expected but was %d", __func__, __FILE__, __LINE__, minunit_tmp_e, minunit_tmp_r);\
		minunit_status = 1;\
		return;\
	} else {\
		printf(".");\
	}\
)

#define mu_assert_double_eq(expected, result) MU__SAFE_BLOCK(\
	double minunit_tmp_e;\
	double minunit_tmp_r;\
	minunit_assert++;\
	minunit_tmp_e = (expected);\
	minunit_tmp_r = (result);\
	if (fabs(minunit_tmp_e-minunit_tmp_r) > MINUNIT_EPSILON) {\
		int minunit_significant_figures = 1 - log10(MINUNIT_EPSILON);\
		snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "%s failed:\n\t%s:%d: %.*g expected but was %.*g", __func__, __FILE__, __LINE__, minunit_significant_figures, minunit_tmp_e, minunit_significant_figures, minunit_tmp_r);\
		minunit_status = 1;\
		return;\
	} else {\
		printf(".");\
	}\
)


#ifdef __cplusplus
}
#endif

#endif /* __MINUNIT_H__ */
