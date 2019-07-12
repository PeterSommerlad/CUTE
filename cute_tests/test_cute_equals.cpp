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

#include "test_cute_equals.h"
#include "cute_base.h"
#include "cute_equals.h"
#include "cute_throws.h"
#include <limits>
#include <set>
#include <sstream>
#include <string>
#ifdef USE_STD11
#include <tuple>
#endif
#include <utility>
#include <vector>


void test_equals_OK() {
	int fourtytwo = 42;
	unsigned long fourtytwoL = 42UL;
	char fourtytwoC = '\042';
	ASSERT_EQUAL(42,fourtytwo);
	ASSERT_EQUAL(42UL,fourtytwoL);
	ASSERT_EQUAL(42u,fourtytwoL);
	ASSERT_EQUAL(42UL,fourtytwo);
	ASSERT_EQUAL('\042',fourtytwoC);
	ASSERT_EQUAL(042,fourtytwoC);
	char const * f42s = "42";
	ASSERT_EQUAL("42",f42s); // requires do_equal overload for char const * if there is no constant folding
	ASSERT_EQUAL("42",std::string(f42s));
	ASSERT_EQUAL(std::string("42"),f42s);
	ASSERT_EQUAL(std::string("42"),std::string(f42s));
	char  f42snc[]="42";
	ASSERT_EQUAL("42",f42snc);
}

void test_assertEqualsDelta() {
	ASSERT_EQUAL_DELTA(42,45,5); // roughly the same...
}
void test_equals_int_fails() {
	try {
		ASSERT_EQUAL(42,43);
		throw "should have failed"; // make this another error!
	} catch(cute::test_failure &){
	}
}
const double eps=std::numeric_limits<double>::epsilon();
void test_equals_double_fails() {
	try {
		ASSERT_EQUAL(1.0,1.0+11*eps);
		throw "should have failed"; // make this another error!
	} catch(cute::test_failure &){
	}
}void test_equals_double_zero(){
	ASSERT_EQUAL(0.0,0.0);
}
void test_equals_double_one_one_plus_eps(){
		ASSERT_EQUAL(1.0,1.0+eps);
}
void test_equals_double_minusone_minusone_plus_eps(){
		ASSERT_EQUAL(-1.0,-1.0+eps);
}
// how to solve the following
void test_equals_int_signed_char(){
	ASSERT_EQUAL(-1,static_cast<signed char>(-1));
}
void test_equals_int_double(){
	ASSERT_EQUAL(-42,-42.0);
}
void test_equals_float_unsigned_long(){
	ASSERT_EQUAL(42.0f,42UL);
}
void test_equals_unsinged_minus_one_signed_minus_one(){
	ASSERT_EQUAL(static_cast<unsigned char>(-1),static_cast<signed char>(-1));
}
void test_equals_signed_limit_short(){
	ASSERT_EQUAL(std::numeric_limits<signed short>::max(),static_cast<unsigned long>(std::numeric_limits<signed short>::max()));
}
void test_equals_unsigned_limit_short(){
	ASSERT_EQUAL(std::numeric_limits<unsigned short>::max(),static_cast<unsigned long>(std::numeric_limits<unsigned short>::max()));
}
void test_equals_unsignedshortlimit_long(){
	unsigned short max = std::numeric_limits<unsigned short>::max();
	ASSERT_EQUAL(max,static_cast<long>(max));
}
void test_equal_bool_bool(){
	ASSERT_EQUAL(true,true);
}
void test_equal_bool_int(){
	ASSERT_EQUAL(true,1); // TODO: warning on MSVC on unsafe comparison of int and bool
}
void test_equal_int_bool(){
	ASSERT_EQUAL(0,false); // TODO: warning on MSVC on unsafe comparison of int and bool
}
enum dummy { zero,one};
void test_equal_enum_int(){
	// cannot use an (anonymous) local enum for the following
	ASSERT_EQUAL(zero,0);
	ASSERT_EQUAL(one,1);
}
// the following should somehow use numeric_limits<> to be exact?
void test_equals_double(){
	ASSERT_EQUAL_DELTA(1e3,1001.0,1.0);
	ASSERT_EQUAL(10e14,1+10e14);
}
void test_equals_double_with_numberic_limits(){
	ASSERT_EQUAL(1.0,1.0+10*eps);
    ASSERT_THROWS(ASSERT_EQUAL(1.0,1.0+11*eps),cute::test_failure);
}
void test_equals_float(){
	ASSERT_EQUAL(float(10e7),float(100+10e7)); // assume 6 digit "precision" delta
}
void test_equals_strings_fails(){
	try {
		ASSERT_EQUAL("error",std::string("eror"));
		throw "should have failed";
	}catch(cute::test_failure const &e){
		ASSERT_EQUAL_DELTA(__LINE__,e.lineno,5);
		ASSERT_EQUAL(__FILE__,e.filename);
	}
}
void test_diff_values(){
	ASSERT_EQUAL(" expected:\t1\tbut was:\t2\t",cute::diff_values(1,2));
	ASSERT_EQUAL(" expected:\t" "error\\n" "\tbut was:\t" "eror\\t\t",cute::diff_values("error\n",std::string("eror\t")));
}
void test_backslashQuoteTabNewline(){
	std::string in("Hallo");
	ASSERT(in == cute::cute_to_string::backslashQuoteTabNewline(in));
	std::string shouldQuote("Hi\nPeter\\tab\t ");
	std::string shouldQuoteQuoted("Hi\\nPeter\\\\tab\\t ");
	ASSERT(shouldQuoteQuoted == cute::cute_to_string::backslashQuoteTabNewline(shouldQuote));
}

#include <map>
void test_output_for_std_map_empty(){
	std::map<std::string,std::string> m;
	std::ostringstream out;
	cute::cute_to_string::to_stream(out,m);
	std::string exp="std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >{}";
	ASSERT_EQUAL(exp,out.str());
}


void test_output_for_std_map() {
	std::map<std::string,std::string> m;
	m["one"]="two";
	m["three"]="four";
	m["five"]="six";
	std::ostringstream out;
	cute::cute_to_string::to_stream(out,m);
	std::string exp="std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >{"
"\n[five -> six],\n[one -> two],\n[three -> four]}";

	ASSERT_EQUAL(exp,(out.str()));
}
void test_output_for_std_pair(){
	std::ostringstream out;
	cute::cute_to_string::to_stream(out,std::pair<std::string,int>("answer",42));
	ASSERT_EQUAL("[answer -> 42]",out.str());
}
#ifdef USE_STD11
void test_output_for_std_tuple(){
	std::ostringstream out;
	cute::cute_to_string::to_stream(out,std::tuple<int,int>(7,42)); // @suppress("Ambiguous problem")
	ASSERT_EQUAL("std::tuple<int, int>{\n7,\n42}",out.str());
}
#endif
void test_output_for_vector_pair(){
	typedef std::vector<std::pair<int,int> > Vec;
	Vec v;
	v.push_back(std::make_pair(42,4));
	std::ostringstream os;
	cute::cute_to_string::to_stream(os,v);
	ASSERT_EQUAL("std::vector<std::pair<int, int>, std::allocator<std::pair<int, int> > >{\n[42 -> 4]}",(os.str()));
}

void test_output_for_vector_set_int_empty(){
	std::vector<std::set<int> > v;
	std::ostringstream os;
	cute::cute_to_string::to_stream(os,v);
	std::string exp="std::vector<std::set<int, std::less<int>, std::allocator<int> >, std::allocator<std::set<int, std::less<int>, std::allocator<int> > > >{}";
	ASSERT_EQUAL(exp,(os.str()));
}
struct No_output_operator{};

void test_non_outputable(){
	No_output_operator x;
	std::ostringstream os;
	cute::cute_to_string::to_stream(os,x);
	std::string exp="no operator<<(ostream&, ";
	exp += cute::demangle(typeid(No_output_operator).name());
	exp+=')';
	ASSERT_EQUAL(exp,os.str());

}




void test_has_end_member_for_vector(){
	std::vector<int> v;
	v.push_back(42); v.push_back(1);
#ifdef USE_STD11
	static_assert(cute::cute_to_string::to_string_detail::has_begin_end_const_member<std::vector<int> const >::value,"");
	static_assert(cute::cute_to_string::to_string_detail::has_begin_end_const_member<std::vector<int>  >::value,"");
	static_assert(cute::cute_to_string::to_string_detail::has_begin_end_const_member<std::vector<std::vector<int> >  >::value,"");
#else
	ASSERT(cute::cute_to_string::to_string_detail::has_begin_end_const_member<std::vector<int> const >::value);
	ASSERT(cute::cute_to_string::to_string_detail::has_begin_end_const_member<std::vector<int>  >::value);
	ASSERT(cute::cute_to_string::to_string_detail::has_begin_end_const_member<std::vector<std::vector<int> >  >::value);

	#endif
}
void test_has_begin_end_member_for_string(){
#ifdef USE_STD11
	static_assert(cute::cute_to_string::to_string_detail::has_begin_end_const_member<std::string>::value,"");
#else
	ASSERT(cute::cute_to_string::to_string_detail::has_begin_end_const_member<std::string>::value);
#endif
}


void test_not_has_end_member_for_int(){
#ifdef USE_STD11
	static_assert(! cute::cute_to_string::to_string_detail::has_begin_end_const_member<int>::value,"");
#else
	ASSERT(! cute::cute_to_string::to_string_detail::has_begin_end_const_member<int>::value);
#endif
}

void test_equalsTwoNaNFails()
{
    ASSERT_THROWS(ASSERT_EQUAL(std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN()),cute::test_failure);
}

void test_doubleEqualsWithANaNFails(){
	ASSERT_THROWS(ASSERT_EQUAL_DELTA(0.0,std::numeric_limits<double>::quiet_NaN(),1.0),cute::test_failure);
}

// some more tests inspired by strong type wrappers
struct Bool {
#ifdef USE_STD11
	explicit
#endif
	operator bool() const {
		return val;
	}
	explicit Bool(bool init=false):val(init){}
	bool val;
};

struct InterestingEqual{

	friend Bool operator==(InterestingEqual const &l, InterestingEqual const &r){
		return Bool(l.val==r.val);
	}
	friend Bool operator>=(InterestingEqual const &l, InterestingEqual const &r){
		return Bool(l.val>=r.val);
	}
	friend InterestingEqual abs(InterestingEqual const &i) {
		return InterestingEqual(i.val < 0? -i.val:i.val);
	}
	friend InterestingEqual operator-(InterestingEqual const &l, InterestingEqual const &r) {
		return InterestingEqual(l.val - r.val);
	}
	explicit InterestingEqual(int init=0):val(init){}
	int val;
};

void test_Customized_abs(){
	InterestingEqual fourty(40);
	InterestingEqual fourtytwo(42);
	InterestingEqual three(3);
	ASSERT_EQUAL_DELTA(fourty,fourtytwo,three);

}
void test_CustomizedEqualWithNon_bool_result(){
	InterestingEqual fourtytwo(42);
	ASSERT_EQUAL(fourtytwo,InterestingEqual(42));
}





cute::suite test_cute_equals(){
	cute::suite s;
	s.push_back(CUTE(test_equals_float));
	s.push_back(CUTE(test_equals_double_zero));
	s.push_back(CUTE(test_equals_double_one_one_plus_eps));
	s.push_back(CUTE(test_equals_double_minusone_minusone_plus_eps));
	s.push_back(CUTE(test_equals_int_signed_char));
	s.push_back(CUTE(test_equals_int_double));
	s.push_back(CUTE(test_equals_float_unsigned_long));
	s.push_back(CUTE(test_equals_unsinged_minus_one_signed_minus_one));
	s.push_back(CUTE(test_equals_signed_limit_short));
	s.push_back(CUTE(test_equals_unsigned_limit_short));
	s.push_back(CUTE(test_equals_unsignedshortlimit_long));
	s.push_back(CUTE(test_equal_bool_bool));
	s.push_back(CUTE(test_equal_bool_int));
	s.push_back(CUTE(test_equal_int_bool));
	s.push_back(CUTE(test_equal_enum_int));
	s.push_back(CUTE(test_equals_double_with_numberic_limits));
	s.push_back(CUTE(test_has_end_member_for_vector));
	s.push_back(CUTE(test_not_has_end_member_for_int));
	s.push_back(CUTE(test_has_begin_end_member_for_string));
	s.push_back(CUTE(test_output_for_std_map));
	s.push_back(CUTE(test_output_for_std_pair));
#ifdef USE_STD11
	s.push_back(CUTE(test_output_for_std_tuple));
#endif
	s.push_back(CUTE(test_output_for_std_map_empty));
	s.push_back(CUTE(test_output_for_vector_pair));
	s.push_back(CUTE(test_output_for_vector_set_int_empty));
	s.push_back(CUTE(test_non_outputable));
	s.push_back(CUTE(test_doubleEqualsWithANaNFails));
	s.push_back(CUTE(test_backslashQuoteTabNewline));
	s.push_back(CUTE(test_equals_OK));
	s.push_back(CUTE(test_equals_int_fails));
	s.push_back(CUTE(test_assertEqualsDelta));
	s.push_back(CUTE(test_equals_double));
	s.push_back(CUTE(test_equals_double_fails));
	s.push_back(CUTE(test_equals_strings_fails));
	s.push_back(CUTE(test_diff_values));
	s.push_back(CUTE(test_equalsTwoNaNFails));
	s.push_back(CUTE(test_Customized_abs));
	s.push_back(CUTE(test_CustomizedEqualWithNon_bool_result));
	return s;
}
