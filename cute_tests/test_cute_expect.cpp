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

#include "test_cute_expect.h"
#include "cute_throws.h"
#include "cute_equals.h"
#include <stdexcept>

using namespace cute;
namespace {
void no_exception() {
}

void throws_std_exception () {
	throw std::exception();
}
void test_throws() {
	ASSERT_THROWS( throws_std_exception() , std::exception);
}
void test_doesntthrow() {
	int avoid_warning_of_no_effect=0;
	ASSERT_THROWS(++avoid_warning_of_no_effect,std::exception);
}
void test_throws_with_code(){
	ASSERT_THROWS( throw std::string("oops"), std::string);
}
void test_throws_with_message() {
	ASSERT_THROWSM("oops",throws_std_exception(),std::exception);
}
void test_fails_throws(){
	ASSERT_THROWS(CUTE(no_exception)(),std::exception);
}
void test_that_fails_throws_failure(){
	ASSERT_THROWS(CUTE(test_fails_throws)(),cute::test_failure);
}
void test_that_throws_throws(){
	ASSERT_THROWS(CUTE(throws_std_exception)(),std::exception);
}
void test_that_doesnt_throw_fails_when_expected_to_throws(){
	ASSERT_THROWS(CUTE(test_doesntthrow)(),cute::test_failure);
}
void test_assert_throws_is_safe_against_throwing_test_failure(){
	ASSERT_THROWS(ASSERT(false),cute::test_failure);
	try {
		ASSERT_THROWS(ASSERT(true),cute::test_failure);
		throw true;
	} catch (cute::test_failure const&) {
	} catch (bool const &){
		FAILM("ASSERT_THROWS falsely catched test_failure");
	}

}
void test_throwing_with_demangle_failure_therefore_suite_should_be_inserted_instead_of_wrapped() {
	throw std::logic_error("NOT A VALID TYPE NAME");
}
void test_assert_throws_with_demangling_error(){
	ASSERT_THROWS(test_throwing_with_demangle_failure_therefore_suite_should_be_inserted_instead_of_wrapped(),std::logic_error);
}
}
cute::suite test_cute_expect() {
	cute::suite s;
	s += CUTE(test_that_fails_throws_failure);
	s += CUTE(test_that_throws_throws);
	s += CUTE(test_throws);
	s += CUTE(test_that_doesnt_throw_fails_when_expected_to_throws);
	s += CUTE(test_throws_with_code);
	s += CUTE(test_throws_with_message);
	s += CUTE(test_assert_throws_is_safe_against_throwing_test_failure);
	//s.push_back(CUTE(test_throwing_with_demangle_failure_therefore_suite_should_be_inserted_instead_of_wrapped));
	s.push_back(CUTE(test_assert_throws_with_demangling_error));
	return s;
}

