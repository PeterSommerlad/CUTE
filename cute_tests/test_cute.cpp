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

#include "test_cute.h"
#include "cute_base.h"
#include "cute_equals.h"

namespace {
	void test_fail_macro(){
		try {
			FAIL();
			throw "should have failed";
		} catch (cute::test_failure const &e){
			ASSERT_EQUAL(__FILE__,e.filename);
			std::string what=e.what();
			std::string fail("FAIL()");
			fail = CUTE_FUNCNAME_PREFIX+fail;
			ASSERT_EQUAL(fail,e.reason);
			ASSERT_EQUAL(fail,what.substr(what.size()-fail.size()));
			ASSERT_EQUAL_DELTA(__LINE__,e.lineno,10);
		}
	}
	void test_t_assert_macro(){
		try {
			ASSERT(0);
			throw "should have failed";
		} catch (cute::test_failure const &e){
			ASSERT_EQUAL(__FILE__,e.filename);
			std::string what=e.what();
			std::string msg("0");
			msg = CUTE_FUNCNAME_PREFIX+msg;
			ASSERT_EQUAL(msg,e.reason);
			ASSERT_EQUAL(msg,what.substr(what.size()-msg.size()));
			ASSERT_EQUAL_DELTA(__LINE__,e.lineno,10);
		}
	}
	void test_what(){
		cute::test_failure ex("foo","file",42);
		ASSERT_EQUAL(std::string("foo"),ex.what());
		ASSERT_EQUAL(42,ex.lineno);
		ASSERT_EQUAL("file",ex.filename);
	}
}


cute::suite test_cute(){
	cute::suite s;
	s.push_back(CUTE(test_what));
	s.push_back(CUTE(test_fail_macro));
	s.push_back(CUTE(test_t_assert_macro));
	return s;

}
