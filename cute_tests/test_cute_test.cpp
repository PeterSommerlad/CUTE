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
#include "cute_test.h"

#include "test_cute_test.h"
#include "cute_equals.h"

namespace for_cute_equals_test {
struct Test{
	void operator()(){}
};
void aTestFunction(){
}
}
namespace {
void test_cute_macro(){
	using namespace for_cute_equals_test;
	cute::test t = CUTE(aTestFunction);
	ASSERT_EQUAL("aTestFunction",t.name());
}
void test_functor(){
	cute::test t = for_cute_equals_test::Test();
	std::string typesuffix= "for_cute_equals_test::Test";
	ASSERT_EQUAL(typesuffix,t.name().substr(t.name().size()-typesuffix.size()));
}
}
cute::suite test_cute_test(){
	cute::suite s;
	s.push_back( CUTE(test_cute_macro));
	s.push_back(CUTE(test_functor));
	return s;
}
