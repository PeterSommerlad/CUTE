/*********************************************************************************
 * This file is part of CUTE.
 *
 * Copyright (c) 2007-2018 Peter Sommerlad, IFS
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
	CUTE(mySimpleTest)
	,mySimpleTest
	,CUTE(anotherTest)
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
