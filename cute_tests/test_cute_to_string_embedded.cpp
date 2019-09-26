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

#include "test_cute_to_string_embedded.h"
#include "cute.h"
#include "cute_determine_version.h"

#undef CUTE_TO_STRING_H_
#define DONT_USE_IOSTREAM 1
#define cute_to_string cute_to_string_embedded
#include "cute_to_string.h"
#undef cute_to_string
#undef DONT_USE_IOSTREAM // also works without...

using cute::cute_to_string_embedded::to_string;
void test_embedded_to_string(){
	ASSERT_EQUAL("no to_string",to_string(42.0));
}

void test_embedded_string_to_string(){
	ASSERT_EQUAL("Hello World\n",to_string(std::string("Hello World\n")));
}

void test_embedded_charptr_to_string(){
	ASSERT_EQUAL("Hello World\n",to_string("Hello World\n"));
}
void test_embedded_integral_to_string(){
	ASSERT_EQUAL("0",to_string(0));
	ASSERT_EQUAL("42",to_string(42));
}
#ifdef USE_STD11
void test_embedded_integral_long_long_to_string(){
	if (sizeof(1L)<sizeof(1LL)){
		ASSERT_EQUAL("1LL",to_string(1LL));
		ASSERT_EQUAL("0x1LL",to_string(1ull));
	} else {
		ASSERT_EQUAL("1L",to_string(1LL));
		ASSERT_EQUAL("0x1L",to_string(1ULL));
	}
}
void test_embedded_unsigned_long_long_to_string(){
	if (sizeof(42ull)>sizeof(unsigned long)){
		ASSERT_EQUAL("0x2ALL",to_string(42ull));
	} else {
		ASSERT_EQUAL("0x2AL",to_string(42ull));
	}
}
#endif
void test_embedded_unsigned_to_string(){
	ASSERT_EQUAL("0x0",to_string(0u));

	ASSERT_EQUAL("0x2A",to_string(42u));
}
void test_embedded_unsigned_long_to_string(){
	std::string expect("0x0");
	if (sizeof(unsigned)<sizeof(unsigned long)) expect+="L";
	ASSERT_EQUAL(expect,to_string(0x0ul));
	expect="0x2A";
	if (sizeof(unsigned)<sizeof(unsigned long)) expect+="L";
	ASSERT_EQUAL(expect,to_string(42ul));
}

void test_embedded_minint_to_string(){
	ASSERT_EQUAL("-32768",to_string(std::numeric_limits<short>::min()));
	ASSERT_EQUAL("-32767",to_string(std::numeric_limits<short>::min()+1));
	ASSERT_EQUAL("32767",to_string(std::numeric_limits<short>::max()));
	// assuming 16 bit shorts and 32 bit ints
	ASSERT_EQUAL("-2147483648",to_string(std::numeric_limits<int>::min()));
}
void test_embedded_pointer_to_string(){
	void *foo=0;
	std::string expect(sizeof(void*)*2,'0');
	expect = "p"+expect;
	ASSERT_EQUAL(expect,to_string(foo));
}

cute::suite make_suite_test_cute_to_string_embedded(){
	cute::suite s;
	s.push_back(CUTE(test_embedded_to_string));
	s.push_back(CUTE(test_embedded_string_to_string));
	s.push_back(CUTE(test_embedded_charptr_to_string));
	s.push_back(CUTE(test_embedded_integral_to_string));
	s.push_back(CUTE(test_embedded_unsigned_to_string));
	s.push_back(CUTE(test_embedded_unsigned_long_to_string));
	s.push_back(CUTE(test_embedded_minint_to_string));
	s.push_back(CUTE(test_embedded_pointer_to_string));
#ifdef USE_STD11
	s.push_back(CUTE(test_embedded_unsigned_long_long_to_string));
	s.push_back(CUTE(test_embedded_integral_long_long_to_string));
#endif
	return s;
}



