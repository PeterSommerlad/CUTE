#include "test_cute_filter_runner.h"
#include "cute_runner.h"
#include "cute.h"
#include "cute_counting_listener.h"
#include <iterator>

void test() {
}

void thisIsAtest_cute_filter_runnerTest() {
	char const *argv[] = { "dummy", "test suite1", "testsuite2#test1", 0,
			"testsuite2#test3", 0, 0 };
	cute::suite s;
	s += CUTE(test);
	s += cute::test(test, "test1");
	s += cute::test(test, "test2");
	s += cute::test(test, "test3");
	s += cute::test(test, "test4");

	cute::counting_listener<> l;
	cute::runner<cute::counting_listener<> > run = cute::makeRunner(l,
			sizeof(argv) / sizeof(*argv) - 1, argv);
	run(s, "test suite1");
	run(s, "testsuite2");
	ASSERT_EQUAL(2, l.numberOfSuites);
	ASSERT_EQUAL(s.size()+2, l.numberOfTests);
	ASSERT_EQUAL(0, l.errors);
	ASSERT_EQUAL(0, l.failedTests);
	ASSERT_EQUAL(7, l.successfulTests);
	ASSERT_EQUAL(l.numberOfTestsInSuites,l.numberOfTests);
}

void test_cute_filter_runner_ArgvFilter() {
	char const *argv[] = { "dummy", "testsuite1", "testsuite2#test1",
			"testsuite2#test3", 0 };
	std::vector<std::string> args;
	std::copy(argv + 1, argv + sizeof(argv) / sizeof(*argv) - 1,
			std::back_inserter(args));
	cute::runner_aux::ArgvTestFilter filter1("", args);
	ASSERT(filter1.shouldRun("any"));
	cute::runner_aux::ArgvTestFilter filter2("testsuite1", args);
	ASSERT(filter2.shouldrunsuite);
	ASSERT(filter2.shouldRun("test"));
	ASSERT(filter2.shouldRun("test1"));
	ASSERT(filter2.shouldRun("test2"));
	ASSERT(filter2.shouldRun("test3"));
	ASSERT(filter2.shouldRun("test4"));
	cute::runner_aux::ArgvTestFilter filter3("dummy", args);

	ASSERT(!filter3.shouldrunsuite);
	cute::runner_aux::ArgvTestFilter filter4("testsuite2", args);
	ASSERT(filter4.shouldrunsuite);
	ASSERT(!filter4.shouldRun("test"));
	ASSERT(filter4.shouldRun("test1"));
	ASSERT(!filter4.shouldRun("test2"));
	ASSERT(filter4.shouldRun("test3"));
	ASSERT(!filter4.shouldRun("test4"));

}

cute::suite make_suite_test_cute_filter_runner() {
	cute::suite s;
	s.push_back(CUTE(thisIsAtest_cute_filter_runnerTest));
	s.push_back(CUTE(test_cute_filter_runner_ArgvFilter));
	return s;
}

