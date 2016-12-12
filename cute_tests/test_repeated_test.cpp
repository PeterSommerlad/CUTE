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

#include "test_repeated_test.h"
#include "cute_repeated_test.h"
#include "cute_base.h"
#include "cute_equals.h"
using namespace cute;
namespace {
struct TestRepetition {
	int n;
	TestRepetition(){ n = 0;}
	void operator()(){ ++n; }
};
void repeat_test(){
	TestRepetition toRepeat;
	CUTE_REPEAT(boost_or_tr1::ref(toRepeat),5)();
	ASSERT_EQUAL(5,toRepeat.n);
}
void would_fail_if_run(){
	FAIL();
}
void repeat_0_test(){
	CUTE_REPEAT(CUTE(would_fail_if_run),0)();
}
}

suite test_repeated_test(){
	suite s;
	s += CUTE(repeat_test);
	s += CUTE(repeat_0_test);
	return s;
}
