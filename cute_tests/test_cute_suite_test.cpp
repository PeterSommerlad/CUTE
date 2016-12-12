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
