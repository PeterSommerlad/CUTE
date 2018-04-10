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

#include "test_cute_suite.h"
#include "cute_determine_version.h"
#include "cute_suite.h"
#include "cute_equals.h"

namespace {
	void atest(){}
}
void test_cute_suite(){
	cute::suite s;
	ASSERT_EQUAL(0u,s.size());
	s += CUTE(atest);
	s += CUTE(atest);
	ASSERT_EQUAL(2u,s.size());
	s += s;
	ASSERT_EQUAL(4u,s.size());
}
#if  defined(USE_STD11)
namespace {
#if !defined (_MSC_VER)
cute::suite thesuite={
		{"first",[]{ ASSERTM("a test",true);}},
		{"second",[]{ ASSERTM("another test",true);}}
};
#endif
}
#include "cute_runner.h"
#include "cute_counting_listener.h"
#endif

void test_lambda_suite(){

#if defined(USE_STD11)
#if defined(_MSC_VER)
	// only local lambdas are possible
cute::test thesuite("LambdaTest",[]{ASSERTM("lambda test",true);});
#endif
	cute::counting_listener<> lis;
	cute::suite s;
	s += thesuite;
	cute::makeRunner(lis)(s,"lambda suite");
	ASSERT(lis.numberOfTests>=1);
#endif
}
