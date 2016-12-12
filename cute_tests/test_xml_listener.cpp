#include "cute.h"
#include "xml_listener.h"
#include "test_xml_listener.h"
#include <sstream>
#include "cute_runner.h"
#define MAKE_RUNNER_RUN_TO_OUT \
		cute::xml_listener<> listen(out);\
		cute::runner<cute::xml_listener<> > run(listen)


void test_emptyrun() {
	std::ostringstream out;
	{
		cute::xml_listener<> listen(out);
	}
	ASSERT_EQUAL("<testsuites>\n</testsuites>\n",out.str());
}
void test_single_success(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT ;
		run(CUTE(test_emptyrun));
	}
	ASSERT_EQUAL("<testsuites>\n\t\t<testcase classname=\"\" name=\"test_emptyrun\"/>\n</testsuites>\n",out.str());

}

void test_failure() {
	throw cute::test_failure("must fail","dummy.cpp",42);
}

void test_single_failure(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT ;
		run(CUTE(test_failure));
	}

	ASSERT_EQUAL("<testsuites>\n\t\t<testcase classname=\"\" name=\"test_failure\">\n"
			"\t\t\t<failure message=\"dummy.cpp:42 must fail\">\nmust fail\n\t\t\t</failure>\n"
			"\t\t</testcase>\n</testsuites>\n",out.str());
}
void test_xml_chars_in_failure(){
	throw cute::test_failure("and & <or/> \"hello\"","dummy.cpp",42);
}

void test_xml_masking(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT;
		run(CUTE(test_xml_chars_in_failure));
	}
	ASSERT_EQUAL("<testsuites>\n\t\t<testcase classname=\"\" name=\"test_xml_chars_in_failure\">\n"
			"\t\t\t<failure message=\"dummy.cpp:42 and &amp; &lt;or/&gt; &quot;hello&quot;\">\n"
			"and &amp; &lt;or/&gt; &quot;hello&quot;\n"
			"\t\t\t</failure>\n\t\t</testcase>\n</testsuites>\n",out.str());
}

void test_xml_suite(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT;
		cute::suite suite;
		suite.push_back(CUTE(test_failure));
		suite.push_back(CUTE(test_emptyrun));
		run(suite,"xml_test_suite");
	}
	ASSERT_EQUAL("<testsuites>\n\t<testsuite name=\"xml_test_suite\" tests=\"2\">\n"
			"\t\t<testcase classname=\"xml_test_suite\" name=\"test_failure\">\n"
			"\t\t\t<failure message=\"dummy.cpp:42 must fail\">\nmust fail\n\t\t\t</failure>\n"
			"\t\t</testcase>\n"
			"\t\t<testcase classname=\"xml_test_suite\" name=\"test_emptyrun\"/>\n"
			"\t</testsuite>\n</testsuites>\n",out.str());
}

void test_error(){
	throw "oops";
}

void test_for_error(){
	std::ostringstream out;
	{
		MAKE_RUNNER_RUN_TO_OUT ;
		run(CUTE(test_error));
	}

	ASSERT_EQUAL("<testsuites>\n\t\t<testcase classname=\"\" name=\"test_error\">\n"
			"\t\t\t<error message=\"test_error oops\" type=\"unexpected exception\">\noops\n\t\t\t</error>\n"
			"\t\t</testcase>\n</testsuites>\n",out.str());
}






cute::suite make_suite_test_xml_listener(){
	cute::suite s;
	s.push_back(CUTE(test_emptyrun));
	s.push_back(CUTE(test_single_success));
	s.push_back(CUTE(test_single_failure));
	s.push_back(CUTE(test_xml_masking));
	s.push_back(CUTE(test_xml_suite));
	s.push_back(CUTE(test_for_error));
	return s;
}



