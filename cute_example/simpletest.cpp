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
 * Copyright 2007 Peter Sommerlad
 *
 *********************************************************************************/

#include "cute.h"

int lifeTheUniverseAndEverything = 6*7;

void mySimpleTest(){
    ASSERT_EQUAL(42, lifeTheUniverseAndEverything);
}

#include "cute_runner.h"
#include "ide_listener.h"
#include <iostream>

void main1(){
	cute::ide_listener<> listener{};
    if (cute::makeRunner(listener)(mySimpleTest)){
        std::cout << "success\n";
    } else {
        std::cout << "failure\n";
    }   
}

void main2(){
    cute::ide_listener<> listener{};
    cute::makeRunner(listener)(mySimpleTest);
}

#include "cute_test.h"
#include "cute_equals.h"

int anotherTest(){
	ASSERT_EQUAL(42, lifeTheUniverseAndEverything);
	return 0;
}

cute::test tests[]={
#ifdef __GNUG__
	CUTE(mySimpleTest)
	,mySimpleTest
	,CUTE(anotherTest)
#else /* for MSVC... */
	CUTE(mySimpleTest)
	,CUTE(mySimpleTest)
	,CUTE(reinterpret_cast<void(*)()>(anotherTest))
#endif
};

struct ATestFunctor {
	void operator()(){
		ASSERT_EQUAL_DELTA(42.0, static_cast<double>(lifeTheUniverseAndEverything), 0.001);
	}
};

#include "cute_suite.h"

int main3(){
	cute::ide_listener<> listener{};
    auto run = cute::makeRunner(listener);
    cute::suite s(tests, tests + (sizeof(tests) / sizeof(tests[0])));
	s += ATestFunctor();
	return run(s, "suite");
}

int main(){
	main1();
	main2();
	main3();
}
