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
