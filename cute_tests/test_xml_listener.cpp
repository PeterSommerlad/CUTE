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



