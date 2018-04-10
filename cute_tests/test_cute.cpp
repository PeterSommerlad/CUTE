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
