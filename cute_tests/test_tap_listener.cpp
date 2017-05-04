#include "cute.h"
#include "tap_listener.h"
#include "test_tap_listener.h"
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

cute::suite make_suite_test_tap_listener(){
	cute::suite s;
	s.push_back(CUTE(test_tap_emptyrun));
	s.push_back(CUTE(test_tap_single_success));
	s.push_back(CUTE(test_tap_single_failure));
	s.push_back(CUTE(test_tap_suite));
	s.push_back(CUTE(test_tap_multiple_suites));
	s.push_back(CUTE(test_tap_for_error));
	return s;
}
