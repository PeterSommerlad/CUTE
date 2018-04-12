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

#include "cute_data_driven.h"
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_cute_data_driven.h"

struct test_data {
	bool check;
	cute::test_failure failure;
};

test_data table[] = {
//#line 100 /* for writing stable tests */
		{true, DDT() },
		{false, DDTM(" should fail")}
};

void thisIsAtest_cute_data_drivenTest() {
	ASSERT_DDT(table[0].check,table[0].failure);
	ASSERT_THROWS(ASSERT_DDTM(table[1].check,"a test",table[1].failure),cute::test_failure);
}
void test_cute_data_driven_test_iteration(){
	test_data const*const end=table+sizeof(table)/sizeof(*table);
	for(test_data *it=table; it != end; ++it)
		if (it->check) ASSERT_DDT(it->check,it->failure);
		else ASSERT_THROWS(ASSERT_DDT(it->check,it->failure),cute::test_failure);
}

struct test_eq_data {
	double input,expected;
	cute::test_failure failure;
} eq_table [] = {
		{ 4,16,DDT() },
		{ 2.5,6.25,DDTM("compare well?")}
};
double square(double x){return x*x;}

void test_cute_data_driven_equality_demo(){
	test_eq_data const*const end=eq_table+sizeof(eq_table)/sizeof(*eq_table);
	for(test_eq_data *it=eq_table; it != end; ++it){
		ASSERT_EQUAL_DDT(it->expected,square(it->input),it->failure);
	}
}
void test_cute_data_driven_equal_delta_demo(){
	test_eq_data const*const end=eq_table+sizeof(eq_table)/sizeof(*eq_table);
	for(test_eq_data *it=eq_table; it != end; ++it){
		ASSERT_EQUAL_DELTA_DDT(it->expected,square(it->input),0.1,it->failure);
	}
}

void test_cute_data_driven_less_demo(){
	test_eq_data const*const end=eq_table+sizeof(eq_table)/sizeof(*eq_table);
	for(test_eq_data *it=eq_table; it != end; ++it){
		ASSERT_LESS_DDT(it->input,it->expected,it->failure);
	}
}
void test_cute_data_driven_greater_demo(){
	test_eq_data const*const end=eq_table+sizeof(eq_table)/sizeof(*eq_table);
	for(test_eq_data *it=eq_table; it != end; ++it){
		ASSERT_GREATER_DDT(it->expected,(it->input),it->failure);
	}
}
void test_cute_data_driven_not_equal_to_demo(){
	test_eq_data const*const end=eq_table+sizeof(eq_table)/sizeof(*eq_table);
	for(test_eq_data *it=eq_table; it != end; ++it){
		ASSERT_NOT_EQUAL_TO_DDT(it->expected,(it->input),it->failure);
	}
}
void test_cute_data_driven_less_equal_demo(){
	test_eq_data const*const end=eq_table+sizeof(eq_table)/sizeof(*eq_table);
	for(test_eq_data *it=eq_table; it != end; ++it){
		ASSERT_LESS_EQUAL_DDT(square(it->input),it->expected,it->failure);
	}
}
void test_cute_data_driven_greater_equal_demo(){
	test_eq_data const*const end=eq_table+sizeof(eq_table)/sizeof(*eq_table);
	for(test_eq_data *it=eq_table; it != end; ++it){
		ASSERT_GREATER_EQUAL_DDT(it->expected,square(it->input),it->failure);
	}
}



cute::suite make_suite_test_cute_data_driven(){
	cute::suite s;
	s.push_back(CUTE(thisIsAtest_cute_data_drivenTest));
	s.push_back(CUTE(test_cute_data_driven_test_iteration));
	s.push_back(CUTE(test_cute_data_driven_equality_demo));
	s.push_back(CUTE(test_cute_data_driven_equal_delta_demo));
	s.push_back(CUTE(test_cute_data_driven_less_demo));
	s.push_back(CUTE(test_cute_data_driven_greater_demo));
	s.push_back(CUTE(test_cute_data_driven_not_equal_to_demo));
	s.push_back(CUTE(test_cute_data_driven_less_equal_demo));
	s.push_back(CUTE(test_cute_data_driven_greater_equal_demo));
	return s;
}



