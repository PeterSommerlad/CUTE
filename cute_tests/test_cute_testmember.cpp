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
