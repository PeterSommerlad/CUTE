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
 * Copyright 2008-2009 Peter Sommerlad
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

