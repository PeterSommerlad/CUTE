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

#include "cute_indexed.h"

struct Point3d {
    int x, y, z;
};

Point3d makePoint(int i) {
#ifdef FAIL_INDEXED_TEST
    return Point3d{i, i + 1, i - 1};
#else
    return i == 5 ? Point3d{i, i, i} : Point3d{i, i + 1, i - 1};
#endif 
}

struct Exception {
    int code_; 
    explicit Exception(int code) : code_(code) { }
};

void throwException(int code) {
#ifdef FAIL_CHECKED_THROWS_TEST
    throw Exception(code + 1);
#else 
    throw Exception(code);
#endif
}

void testIndexed() {
    for(int i = 0; i < 10; ++i) {
        const auto refPoint = i == 5 ? Point3d{i, i, i} : Point3d{i, i + 1, i - 1};
        const auto point = makePoint(i);
        ASSERT_EQUALI(i, refPoint.x, point.x);
        ASSERT_EQUALI(i, refPoint.y, point.y);
        ASSERT_EQUALI(i, refPoint.z, point.z);
    }
}

void testCheckedThrow() {
    ASSERT_THROWS(throwException(0), Exception);
    ASSERT_THROWS_CHECK(throwException(0), Exception, 
        ASSERT_EQUAL(0, e.code_));
}
  

int main4() {
    static const cute::test tests[] = {
        CUTE(testIndexed),
        CUTE(testCheckedThrow),
    };

    cute::ide_listener<> listener{};
    auto run = cute::makeRunner(listener);
    cute::suite s(tests, tests + (sizeof(tests) / sizeof(tests[0])));
    return run(s, "new test suite");
}

int main(){
    main1();
    main2();
    main3();
    main4();
}
