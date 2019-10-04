/*********************************************************************************
 * This file is part of CUTE.
 *
 * Copyright (c) 2006-2018 Peter Sommerlad, IFS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************************/

#include "test_tap_listener.h"
#ifdef USE_STD11
#include "cute.h"
#include "tap_listener.h"
#include <sstream>
#include "cute_runner.h"
#define MAKE_RUNNER_RUN_TO_OUT \
		cute::tap_listener<> listen(out);\
		cute::runner<cute::tap_listener<> > run(listen)


void test_tap_emptyrun() {
	std::ostringstream out;
	{
		cute::tap_listener<> listen(out);
	}
	ASSERT_EQUAL("",out.str());
}
void test_tap_single_success(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT ;
		run(CUTE(test_tap_emptyrun));
	}
	ASSERT_EQUAL(
		"ok 1 test_tap_emptyrun\n"
		"1..1\n",out.str());
}

void test_tap_failure() {
	throw cute::test_failure("must fail","dummy.cpp",42);
}

void test_tap_single_failure(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT ;
		run(CUTE(test_tap_failure));
	}

	ASSERT_EQUAL(
		"not ok 1 test_tap_failure\n"
		"# Assertion failed: must fail\n"
		"1..1\n",out.str());
}

void test_tap_suite(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT;
		cute::suite suite;
		suite.push_back(CUTE(test_tap_failure));
		suite.push_back(CUTE(test_tap_emptyrun));
		run(suite,"tap_test_suite");
	}
	ASSERT_EQUAL(
		"# Starting suite 'tap_test_suite' containing 2 tests\n"
		"not ok 1 test_tap_failure\n"
		"# Assertion failed: must fail\n"
		"ok 2 test_tap_emptyrun\n"
		"# Ending suite 'tap_test_suite'\n"
		"1..2\n",out.str());
}

void test_tap_multiple_suites(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT;
		{
			cute::suite suite;
			suite.push_back(CUTE(test_tap_failure));
			run(suite,"tap_test_suite_1");
		}

		{
			cute::suite suite;
			suite.push_back(CUTE(test_tap_emptyrun));
			run(suite,"tap_test_suite_2");
		}
	}

	ASSERT_EQUAL(
		"# Starting suite 'tap_test_suite_1' containing 1 tests\n"
		"not ok 1 test_tap_failure\n"
		"# Assertion failed: must fail\n"
		"# Ending suite 'tap_test_suite_1'\n"
		"# Starting suite 'tap_test_suite_2' containing 1 tests\n"
		"ok 2 test_tap_emptyrun\n"
		"# Ending suite 'tap_test_suite_2'\n"
		"1..2\n",out.str());
}


void test_tap_error(){
	throw "oops";
}

void test_tap_for_error(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT ;
		run(CUTE(test_tap_error));
	}
	ASSERT_EQUAL(
		"not ok 1 test_tap_error\n"
		"# Unexpected exception: oops\n"
		"1..1\n",out.str());
}
#endif

cute::suite make_suite_test_tap_listener(){
	cute::suite s;
#ifdef USE_STD11
	s.push_back(CUTE(test_tap_emptyrun));
	s.push_back(CUTE(test_tap_single_success));
	s.push_back(CUTE(test_tap_single_failure));
	s.push_back(CUTE(test_tap_suite));
	s.push_back(CUTE(test_tap_multiple_suites));
	s.push_back(CUTE(test_tap_for_error));
#endif
	return s;
}
