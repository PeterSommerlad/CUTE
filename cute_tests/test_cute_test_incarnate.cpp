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

#include "test_cute_test_incarnate.h"
#include "cute_test_incarnate.h"
#include "cute_equals.h"
namespace {
	struct IncarnationTest {
		static int counter;
		IncarnationTest(){
			counter = 42;
		}
		~IncarnationTest(){
			counter = 4242;
		}
		void operator()(){
			++counter;
			ASSERT_EQUAL(43,counter);
		}
	};
	int IncarnationTest::counter = 0;
	void test_simple_incarnate(){
		IncarnationTest::counter = 0;
		cute::test t = CUTE_INCARNATE(IncarnationTest);
		ASSERT_EQUAL(0,IncarnationTest::counter);
		t();
		ASSERT_EQUAL(4242,IncarnationTest::counter);
	}

	struct IncarnateContextTest {
		int &counter;
		IncarnateContextTest(int &c):counter(c){
			counter=10;
		}
		IncarnateContextTest(IncarnateContextTest &other):counter(other.counter){}
		void operator()(){
			++counter;
			ASSERT_EQUAL(11,counter);
		}
		~IncarnateContextTest(){
			counter=1010;
		}
	};
	void test_context_incarnate(){
		int counter=0;
		cute::test t = CUTE_INCARNATE_WITH_CONTEXT(IncarnateContextTest,boost_or_tr1::ref(counter));
		ASSERT_EQUAL(0,counter);
		t();
		ASSERT_EQUAL(1010,counter);
	}
}

cute::suite test_cute_test_incarnate(){
	cute::suite s;
	s += CUTE(test_simple_incarnate);
	s += CUTE(test_context_incarnate);
	return s;
}

