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

#include "cute_testmember.h"

#include "test_cute_testmember.h"
#include "cute_equals.h"
#include "cute_suite_test.h"
namespace {
struct TestClass{
	static int callcounter;
	int i;
	TestClass():i(1){}
	TestClass(int j):i(j){} // for incarnation
	void test1(){
		++callcounter;
		ASSERT_EQUAL(1,i++);
	}
	void test2() const {
		++callcounter;
		ASSERT(true);
	}
	void test3() {
		++callcounter;
		ASSERT_EQUAL(2,i++);
		++i;
	}
	void test_incarnate(){
		++callcounter;
		ASSERT_EQUAL(42,i++);
	}
	void test_incarnate_const() const {
		++callcounter;
		ASSERT_EQUAL(43,i);
	}
};
int TestClass::callcounter=0;
void test_members_simple(){
	cute::suite s3;
	TestClass::callcounter=10;
	s3.push_back(CUTE_SMEMFUN(TestClass,test1));
	s3.push_back(CUTE_SMEMFUN(TestClass,test2));
	s3.push_back(CUTE_SMEMFUN(TestClass,test1));
	cute::test t=CUTE_SUITE_TEST(s3);
	t();
	ASSERT_EQUAL(13,TestClass::callcounter);
}
void test_members_object(){
	TestClass tc;
	cute::suite s3;
	TestClass::callcounter=20;
	s3.push_back(CUTE_MEMFUN(tc,TestClass,test1));
	s3.push_back(CUTE_MEMFUN(tc,TestClass,test2));
	s3.push_back(CUTE_MEMFUN(tc,TestClass,test3));
	cute::test t=CUTE_SUITE_TEST(s3);
	t();
	ASSERT_EQUAL(4,tc.i);
	ASSERT_EQUAL(23,TestClass::callcounter);
}
void test_members_incarnate(){
	cute::suite s;
	s.push_back(CUTE_CONTEXT_MEMFUN(42,TestClass,test_incarnate));
	s.push_back(CUTE_CONTEXT_MEMFUN(43,TestClass,test_incarnate_const));
	cute::test t= CUTE_SUITE_TEST(s);
	TestClass::callcounter=30;
	t();
	ASSERT_EQUAL(32,TestClass::callcounter);
}


}

cute::suite test_cute_testmember(){
	cute::suite s;
	s.push_back(CUTE(test_members_simple));
	s.push_back(CUTE(test_members_object));
	s.push_back(CUTE(test_members_incarnate));
	return s;
}
