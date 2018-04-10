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

#include "cute_suite_test.h"
#include "test_cute_suite_test.h"

#include "cute_base.h"
#include "cute_equals.h"

namespace {
// dummy tests
size_t counter=0;
void testOK(){ ++counter; }
void testFails() { --counter; FAIL();}
// tests:
void test_suite_OK(){
	cute::suite s;
	counter =0;
	s += CUTE(testOK);
	s += CUTE(testOK);
	s += CUTE(testOK);
	cute::test t = CUTE_SUITE_TEST(s);
	t();
	ASSERT_EQUAL(s.size(),counter);
}
void test_suite_fails(){
	cute::suite suite_that_fails;
	counter =0;
	suite_that_fails.push_back(CUTE(testOK)); // 1
	suite_that_fails.push_back( CUTE(testOK)); // 2
	suite_that_fails.push_back(CUTE(testFails)); // 1
	suite_that_fails.push_back( CUTE(testFails)); // 0 -> should not be reached
	cute::test t = CUTE_SUITE_TEST(suite_that_fails);
	try {
		t();
		throw "should have failed";
	} catch (cute::test_failure &){
		ASSERT_EQUAL(1u,counter);
	}
}
}
cute::suite test_cute_suite_test(){
	cute::suite s;
	s.push_back(CUTE(test_suite_OK));
	s.push_back(CUTE(test_suite_fails));
	return s;
}
