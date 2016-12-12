/*********************************************************************************
 * This file is part of CUTE.
 *
 * CUTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CUTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with CUTE.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Peter Sommerlad
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
