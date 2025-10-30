/*********************************************************************************
 * This file is part of CUTE.
 *
 * Copyright (c) 2008-2018 Peter Sommerlad, IFS
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

#include "cute_sh.h"
//#include "cute_runner.h"
//#include "cute_counting_listener.h"
//#include "ide_listener.h"
//#include "xml_listener.h"
#include <iostream>

#include "test_cute_equals.h"
#include "test_cute_expect.h"
#include "test_repeated_test.h"
#include "test_cute_runner.h"
#include "test_cute_suite_test.h"
#include "test_cute_suite.h"
#include "test_cute_test_incarnate.h"
#include "test_cute_test.h"
#include "test_cute_testmember.h"
#include "test_cute.h"
#include "test_cute_to_string.h"
#include "test_cute_to_string_embedded.h"
#include "test_xml_listener.h"
#include "test_xml_file_opener.h"
#include "test_cute_filter_runner.h"
#include "test_cute_relops.h"
#include "test_cute_data_driven.h"
#include "test_tap_listener.h"

#ifdef DONT_USE_IOSTREAM
// provide IDE listener nevertheless
namespace cute {
	template <typename Listener=null_listener>
	struct ide_listener: Listener
	{
		ide_listener(std::ostream &os=std::cout):out(os) {}
		void begin(suite const &t,char const *info, size_t n_of_tests){
			out << std::dec << "\n#beginning " << info << " " << n_of_tests << '\n' << std::flush;
			Listener::begin(t,info,n_of_tests);
		}
		void end(suite const &t, char const *info){
			out << "\n#ending " << info << '\n' << std::flush;
			Listener::end(t,info);
		}
		void start(test const &t){
			out << "\n#starting " << t.name() << '\n' << std::flush;
			Listener::start(t);
		}
		void success(test const &t, char const *msg){
			out << "\n#success " <<  maskBlanks(t.name()) << " " << msg << '\n' << std::flush;
			Listener::success(t,msg);
		}
		void failure(test const &t,test_failure const &e){
			out << std::dec <<  "\n#failure " << maskBlanks(t.name()) << " " << e.filename << ":" << e.lineno << " " << (e.reason) << '\n' << std::flush;
			Listener::failure(t,e);
#ifdef _MSC_VER
			std::ostringstream os;
			os << std::dec << e.filename << "(" << e.lineno << ") : failure: " <<e.reason << " in " << t.name() << '\n' << std::flush;
			OutputDebugString(os.str().c_str());
#endif
		}
		void error(test const &t, char const *what){
			out << "\n#error " << maskBlanks(t.name()) << " " << what << '\n' << std::flush;
			Listener::error(t,what);
#ifdef _MSC_VER
			std::ostringstream os;
			os << what << " error in " << t.name() << '\n' << std::flush;
			OutputDebugString(os.str().c_str());
#endif
		}
		static std::string maskBlanks(std::string in) {
			std::transform(in.begin(),in.end(),in.begin(),blankToUnderscore());
			return in;
		}
	protected:
		struct blankToUnderscore{
            char operator()(char in) const {
			if (in == ' ') return '_';
			return in;
		}
        };
		std::ostream &out;
	};
}
#endif


using namespace cute;
// some brain dead test cases to find out my bug using function
// names without & as function pointers
static int simpleTestfunctionCalled=0;
void simpleTestFunction(){
	++simpleTestfunctionCalled;
	std::cerr << "simpleTestFunction run no:"<< simpleTestfunctionCalled << std::endl;
	ASSERT(true);
	throw std::exception();
}
struct SimpleTestFunctionCalledTest {
	void operator()() const{
		ASSERT_EQUALM("look at cute::test ctor overload",1,simpleTestfunctionCalled);
	}
};
void shouldFailButNotThrowStdException(){
	ASSERT(false);
	throw std::exception();
}
// demonstrates how to write equality tests
void test2(){
	ASSERT_EQUAL(1,1);
	ASSERT_EQUAL(1,2);
}
// demonstrates how to write test functors
struct test3{
	void operator()() const{
		throw std::exception();
	}
};
// demonstrates how test objects are incarnated
struct to_incarnate{
	std::ostream &out;
	to_incarnate(std::ostream &os):out(os){
		out << "born" << std::endl;
	}
	to_incarnate(to_incarnate &other):out(other.out){} // copy ctor would be deleted and is required by std::function ctor
	~to_incarnate() {
		out << "killed" << std::endl;
	}
	void operator()(){
		out << "tested" << std::endl;
	}
};
struct to_incarnate_without : to_incarnate {
	to_incarnate_without():to_incarnate(std::cout){}
};
void test_SimpleTestFunctionThrows(){
	ASSERT_THROWS(simpleTestFunction(),std::exception);
}
void test_shouldFailThrowsFailure(){
	ASSERT_THROWS(shouldFailButNotThrowStdException(),cute::test_failure);
}
int main(int argc, char const *argv[]){
	using namespace std;
	suite s;
	s += make_suite_test_cute_data_driven();
	s += make_suite_test_cute_relops();
	s += make_suite_test_cute_filter_runner();
	s += make_suite_test_xml_listener();
	s += make_suite_test_xml_file_opener();
	s += make_suite_test_cute_to_string_embedded();
	s += make_suite_test_tap_listener();
	s += test_cute_to_string();
	s += test_cute_equals();
	// the following test produces one of the 2 expected errors, since it throws
	//s += CUTE(simpleTestFunction);
	s += CUTE(test_SimpleTestFunctionThrows);
	s += SimpleTestFunctionCalledTest();
	s += CUTE(test_shouldFailThrowsFailure);
	suite throwing=test_cute_expect();
	s.insert(s.end(),throwing.begin(),throwing.end());
	s += CUTE_SUITE_TEST(test_repeated_test());
	s += CUTE(test_cute_runner);
	s += CUTE_SUITE_TEST(test_cute_suite_test());
	s += CUTE(test_cute_suite);
	s += CUTE(test_lambda_suite);
	s += CUTE_SUITE_TEST(test_cute_test_incarnate());
	s += CUTE_SUITE_TEST(test_cute_test());
	s += CUTE_SUITE_TEST(test_cute_testmember());
	s += CUTE_SUITE_TEST(test_cute());
	//---
	s += CUTE_INCARNATE(to_incarnate_without);
	s += CUTE_INCARNATE_WITH_CONTEXT(to_incarnate,std::ref(std::cout));
	s += CUTE_CONTEXT_MEMFUN(std::ref(std::cerr),to_incarnate,operator());
#ifndef DONT_USE_IOSTREAM
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::counting_listener<cute::ide_listener<> > > l(xmlfile.out);
#else
	cute::counting_listener<cute::ide_listener<>> l{};
#endif
	cute::makeRunner(l,argc,argv)(s,"all cute tests");
	cout << flush;
	cerr << flush;
	cerr << l.numberOfTests << " Tests - expect " << s.size() << endl;
	cerr << l.failedTests << " failed - expect 0 failures" << endl;
	cerr << l.errors << " errors - expect 0 errors" << endl;
	return int(l.failedTests);//silence warning
}
