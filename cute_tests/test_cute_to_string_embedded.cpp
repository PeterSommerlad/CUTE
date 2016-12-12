#include "cute.h"
#include "test_cute_to_string_embedded.h"

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
void test_embedded_integral_long_long_to_string(){
	if (sizeof(1L)<sizeof(1LL)){
		ASSERT_EQUAL("1LL",to_string(1LL));
		ASSERT_EQUAL("0x1LL",to_string(1ull));
	} else {
		ASSERT_EQUAL("1L",to_string(1LL));
		ASSERT_EQUAL("0x1L",to_string(1ULL));
	}
}

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
void test_embedded_unsigned_long_long_to_string(){
	if (sizeof(42ull)>sizeof(unsigned long)){
		ASSERT_EQUAL("0x2ALL",to_string(42ull));
	} else {
		ASSERT_EQUAL("0x2AL",to_string(42ull));
	}
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
	s.push_back(CUTE(test_embedded_unsigned_long_long_to_string));
	s.push_back(CUTE(test_embedded_minint_to_string));
	s.push_back(CUTE(test_embedded_pointer_to_string));
	s.push_back(CUTE(test_embedded_integral_long_long_to_string));
	return s;
}



