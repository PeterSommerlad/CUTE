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

