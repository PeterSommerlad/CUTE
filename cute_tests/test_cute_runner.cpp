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

#include "cute_runner.h"
#include "cute_equals.h"

#include "mock_listener.h"
using namespace cute;
using namespace cute_test;
namespace {
void test_success(){}
void test_failing(){ ASSERT(false);}
void test_error_cstr(){ throw "error";}
void test_error_exception() { throw std::exception();}
void test_error_string(){ throw std::string("error");}

}
void test_cute_runner(){
	cute_test::mock_listener l;
	cute::runner<mock_listener> run(l);
	suite s;
	s += CUTE(test_success);
	ASSERT(run(s,"single success test suite"));
	ASSERT_EQUAL(1,l.suitetestcount);
	s += CUTE(test_failing);
	s += CUTE(test_error_cstr);
	s += CUTE(test_error_string);
	s += CUTE(test_error_exception);
	bool result=run(s,"test_cute_runner_suite");
	ASSERT(!result);
	ASSERT_EQUAL(2,l.begincount);
	ASSERT_EQUAL(2,l.endcount);
	ASSERT_EQUAL(2,l.successcount);
	ASSERT_EQUAL(1,l.failurecount);
	ASSERT_EQUAL(3,l.errorcount);
	ASSERT_EQUAL(6,l.suitetestcount);
	ASSERT_EQUAL(2u,l.infomessages.size());
	ASSERT_EQUAL("single success test suite",l.infomessages[0]);
	ASSERT_EQUAL("test_cute_runner_suite",l.infomessages[1]);
	ASSERT_EQUAL(3u,l.errormessages.size());
	ASSERT_EQUAL("error",l.errormessages[0]);
	ASSERT_EQUAL("error",l.errormessages[1]);
	std::string errormsg2=l.errormessages[2];
	std::string errmsgexpected="exception";
	ASSERT_EQUAL(errmsgexpected,errormsg2.substr(errormsg2.size()-errmsgexpected.size()));
	ASSERT_EQUAL(2u,l.successmessages.size());
	ASSERT_EQUAL("OK",l.successmessages[0]);

}
