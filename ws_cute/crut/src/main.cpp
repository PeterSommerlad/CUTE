/*********************************************************************************
 * This file is part of CUTE.
 *
 * Copyright (c) 2008-2018 Peter Sommerlad, IFS
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


import crut;
import std;



namespace cute {
using std::string_view_literals::operator""sv;
int a;
static_assert("::cute::a"sv == fqn<^^a>);
static_assert("::cute::fqn"sv == fqn<^^fqn>);
namespace { // anonymous namespace stops recursion
bool b;
}
static_assert("b"sv==fqn<^^b>);
constexpr auto lam{ [](int i){
	static_assert(std::string{"i"} == fqn<^^i>);
}};
static_assert("::cute::lam"sv == fqn<^^lam>);
void isFunctionScopeAParent(){
	int j;
	static_assert("j"sv == fqn<^^j>);
}
static_assert("::cute::isFunctionScopeAParent"sv == fqn<^^isFunctionScopeAParent>);
enum anEnum{ one, two};
static_assert(fqn<^^anEnum> == "::cute::anEnum"sv);
static_assert(fqn<^^one> == "::cute::anEnum::one"sv);
struct S { enum { red } e;
static_assert(fqn<^^red> == "::cute::S::red"sv);
};
static_assert(fqn<^^S::e> == "::cute::S::e"sv);
struct { int i;
static_assert(fqn<^^i> == "i"sv);
}  x[[maybe_unused]];
namespace {
// no fully qualified name
int a;
static_assert("a"sv == fqn<^^a>);
constexpr auto lam{ [](int i){
	static_assert(std::string{"i"} == fqn<^^i>);
}};
static_assert("lam"sv == fqn<^^lam>);
[[maybe_unused]]
void isFunctionScopeAParent(){
	int j;
	static_assert("j"sv == fqn<^^j>);
}
static_assert("isFunctionScopeAParent"sv == fqn<^^isFunctionScopeAParent>);
enum anEnum{ one, two};
static_assert(fqn<^^anEnum> == "anEnum"sv);
static_assert(fqn<^^one> == "anEnum::one"sv);
struct S { enum { red } e;
static_assert(fqn<^^red> == "S::red"sv);
};
static_assert(fqn<^^S::e> == "S::e"sv);
struct { int anonymousstructmembernotinsertet;
static_assert(fqn<^^anonymousstructmembernotinsertet> == "anonymousstructmembernotinsertet"sv);
}  x[[maybe_unused]];

}
}
// fully qualified name tests end






using namespace cute;

#define CUTE(func) cute::make_test<^^func>

// some brain dead test cases
namespace brain_dead_tests{
namespace {
static int simpleTestfunctionCalled=0;
void simpleTestFunction(){

	++simpleTestfunctionCalled;
	std::cerr << "simpleTestFunction run no:"<< simpleTestfunctionCalled << std::endl;
	ASSERT(true);
	throw std::exception();
}
void shouldFailButNotThrowStdException(){
	ASSERT(false);
	throw std::exception();
}
}
void test_SimpleTestFunctionThrows(){
	cute::assert_throws<std::exception>([&](){simpleTestFunction();});
}
void test_shouldFailThrowsFailure(){
	cute::assert_throws<cute::test_failure>([&](){shouldFailButNotThrowStdException();});
}
void test_fail_macro(){
	try {
		FAIL();
		throw "should have failed";
	} catch (cute::test_failure const &e){
		ASSERT_EQUAL(__FILE__,e.loc.file_name());
		std::string what=e.what();
		std::string fail("FAIL()");
		//fail = CUTE_FUNCNAME_PREFIX+fail;
		ASSERT_EQUAL(fail,e.reason);
		ASSERT_EQUAL(fail,what.substr(what.size()-fail.size()));
		ASSERT_EQUAL_DELTA(__LINE__,e.loc.line(),10);
	}
}
void test_t_assert_macro(){
	try {
		ASSERT(0);
		throw "should have failed";
	} catch (cute::test_failure const &e){
		ASSERT_EQUAL(__FILE__,e.loc.file_name());
		std::string what=e.what();
		std::string msg("void brain_dead_tests::test_t_assert_macro():ASSERT failed");
//		msg = CUTE_FUNCNAME_PREFIX+msg;
		ASSERT_EQUAL(msg,e.reason);
		ASSERT_EQUAL(msg,what.substr(what.size()-msg.size()));
		ASSERT_EQUAL_DELTA(__LINE__,e.loc.line(),10);
	}
}
void test_what(){
	cute::test_failure ex{"foo"};
	ASSERT_EQUAL(std::string("foo"),ex.what());
	ASSERT_EQUAL(__LINE__ -2,ex.loc.line());
	ASSERT_EQUAL(__FILE__,ex.loc.file_name());
}

}
using namespace cute;
namespace tests{
void aFirstTest(){
	ASSERT(true);
	ASSERT(false);
}

void aFirstEqualTest(){
	ASSERT_EQUAL(0,0);
	ASSERT_EQUAL(1,0);
}
void aFirstEqualMessageTest(){
	ASSERT_EQUALM("message",0,0);
	ASSERT_EQUALM("message\\with a	tab and \nnewline",1,0);
}
void aSuccessfulTest(){
	ASSERT(true);
}

struct SomeSuite{
	void aFirstTest(){
		ASSERT(true);
		ASSERT(false);
	}

	void aFirstEqualTest(){
		ASSERT_EQUAL(0,0);
		ASSERT_EQUAL(1,0);
	}
	void aSuccessfulTest(){

	}
};
}

namespace { // prevent from being collected into suite
void  test_tap_failure(){ throw cute::test_failure("must fail"); };
void test_tap_error(){
	throw "oops";
}
void test_tap_emptyrun() {
	std::ostringstream out;
	{
		cute::tap_listener<> listen(out);
	}
	ASSERT_EQUAL("",out.str());
}
}

struct tap_listener_suite {

std::ostringstream out{};
std::optional<cute::tap_listener<>> optlistener{cute::tap_listener<>{out}};
std::optional<cute::runner<cute::tap_listener<> >> run{*optlistener};


void test_tap_single_success(){
	{
		(*run)(cute::make_test<^^test_tap_emptyrun>);
		run.reset();optlistener.reset();
	}
	ASSERT_EQUAL(
		"ok 1 test_tap_emptyrun\n"
		"1..1\n",out.str());
}

void test_tap_single_failure(){
	{
		(*run)(cute::make_test<^^test_tap_failure>);
		run.reset();optlistener.reset();
	}

	ASSERT_EQUAL(
		"not ok 1 test_tap_failure\n"
		"# Assertion failed: must fail\n"
		"1..1\n",out.str());
}

void test_tap_suite(){
	{
		cute::suite suite;
		suite.push_back(cute::make_test<^^test_tap_failure>);
		suite.push_back(cute::make_test<^^test_tap_emptyrun>);
		(*run)(suite,"tap_test_suite");
		run.reset(); optlistener.reset();
	}
	ASSERT_EQUAL(
		"# Starting suite 'tap_test_suite' containing 2 tests\n"
		"not ok 1 test_tap_failure\n"
		"# Assertion failed: must fail\n"
		"ok 2 test_tap_emptyrun\n"
		"# Ending suite 'tap_test_suite'\n"
		"1..2\n",out.str());
}

void test_tap_multiple_suites(){
	{
		{
			cute::suite suite;
			suite.push_back(cute::make_test<^^test_tap_failure>);
			(*run)(suite,"tap_test_suite_1");
		}

		{
			cute::suite suite;
			suite.push_back(cute::make_test<^^test_tap_emptyrun>);
			(*run)(suite,"tap_test_suite_2");
		}
		run.reset();optlistener.reset();
	}

	ASSERT_EQUAL(
		"# Starting suite 'tap_test_suite_1' containing 1 tests\n"
		"not ok 1 test_tap_failure\n"
		"# Assertion failed: must fail\n"
		"# Ending suite 'tap_test_suite_1'\n"
		"# Starting suite 'tap_test_suite_2' containing 1 tests\n"
		"ok 2 test_tap_emptyrun\n"
		"# Ending suite 'tap_test_suite_2'\n"
		"1..2\n",out.str());
}

void test_tap_for_error(){
	{
		(*run)(CUTE(test_tap_error));
		run.reset();optlistener.reset();
	}
	ASSERT_EQUAL(
		"not ok 1 test_tap_error\n"
		"# Unexpected exception: oops\n"
		"1..1\n",out.str());
}
};


namespace cute_equals_test{
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

// the following should somehow use numeric_limits<> to be exact?
void test_equals_double(){
	ASSERT_EQUAL_DELTA(1e3,1001.0,1.0);
	ASSERT_EQUAL(10e14,1+10e14);
}
void test_equals_double_with_numberic_limits(){
	ASSERT_EQUAL(1.0,1.0+10*eps);
	cute::assert_throws<cute::test_failure>([&](){ASSERT_EQUAL(1.0,1.0+11*eps);});
}
void test_equals_float(){
	ASSERT_EQUAL(float(10e7),float(100+10e7)); // assume 6 digit "precision" delta
}
void test_equals_strings_fails(){
	try {
		ASSERT_EQUAL("error",std::string("eror"));
		throw "should have failed";
	}catch(cute::test_failure const &e){
		ASSERT_EQUAL_DELTA(std::source_location::current().line(),e.loc.line(),5);
		ASSERT_EQUAL(std::source_location::current().file_name(),e.loc.file_name());
	}
}
void test_diff_values(){
	ASSERT_EQUAL(" expected:\t1\tbut was:\t2\t",cute::diff_values(1,2));
	ASSERT_EQUAL(" expected:\t\"error\\n\"\tbut was:\t\"eror\\t\"\t",cute::diff_values("error\n",std::string("eror\t")));
}

void test_output_for_std_map_empty(){
	std::map<std::string,std::string> m;
	std::string result = cute::cute_to_string::to_string(m);
//	std::string exp="std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string>>>{}";
	// gcc 16.1 display_string doesn't show defaulted template arguments
	std::string exp="std::map<std::string, std::string >{}";
	ASSERT_EQUAL(exp,result);
}
//
void test_output_for_std_map() {
	std::map<std::string,std::string> m;
	m["one"]="two";
	m["three"]="four";
	m["five"]="six";
	std::string result = cute::cute_to_string::to_string(m);
//	std::string exp="std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string>>>{"
//"\n[five -> six],\n[one -> two],\n[three -> four]}";
//	std::string exp="std::map<std::string, std::string >{"
//"\n[five -> six],\n[one -> two],\n[three -> four]}";
	// default std::format:
	std::string exp=R"_(std::map<std::string, std::string >{"five": "six", "one": "two", "three": "four"})_";

	ASSERT_EQUAL(exp,result);
}
void test_output_for_std_map_with_non_defaulted_comparison() {
	std::map<std::string,std::string, std::less<>> m;
	m["one"]="two";
	m["three"]="four";
	m["five"]="six";
	std::string result = cute::cute_to_string::to_string(m);
//	std::string exp="std::map<std::string, std::string, std::less<void> >{"
//"\n[five -> six],\n[one -> two],\n[three -> four]}";
	// default std::format
	std::string exp=R"_(std::map<std::string, std::string, std::less<void> >{"five": "six", "one": "two", "three": "four"})_";

	ASSERT_EQUAL(exp,result);
}
void test_output_for_std_pair(){
	std::string result = cute::cute_to_string::to_string(std::pair<std::string,int>("answer",42));
//	ASSERT_EQUAL("[answer -> 42]",result);
	// default std::format
	ASSERT_EQUAL(R"_(std::pair<std::string, int>("answer", 42))_",result);
}
void test_output_for_std_tuple(){
	std::string result = cute::cute_to_string::to_string(std::tuple<int,int>(7,42)); // @suppress("Ambiguous problem")
//	ASSERT_EQUAL("std::tuple<int, int>{\n7,\n42}",result);
	// default std::format
	ASSERT_EQUAL("std::tuple<int, int>(7, 42)",result);
}
void test_output_for_vector_pair(){
	typedef std::vector<std::pair<int,int>> Vec;
	Vec v;
	v.push_back(std::make_pair(42,4));
	std::string result = cute::cute_to_string::to_string(v);
//	ASSERT_EQUAL("std::vector<std::pair<int, int>, std::allocator<std::pair<int, int>>>{\n[42 -> 4]}",result);
	// std::format default rendering:
	ASSERT_EQUAL("std::vector<std::pair<int, int> >[(42, 4)]",result);
}

void test_output_for_vector_set_int_empty(){
	std::vector<std::set<int> > v;
	std::string result = cute::cute_to_string::to_string(v);
	//std::string exp="std::vector<std::set<int, std::less<int>, std::allocator<int>>, std::allocator<std::set<int, std::less<int>, std::allocator<int>>>>{}";
	// default std::format rendering
	std::string exp="std::vector<std::set<int> >[]";
	ASSERT_EQUAL(exp,result);
}
struct No_output_operator{int x{42};} ;
constexpr No_output_operator xx{};

constexpr auto info = ^^No_output_operator::x;

static_assert(42 == xx.[:info:]); // check if splicing of data members work

void test_non_outputable(){
	No_output_operator x;
	std::string result = cute::cute_to_string::to_string(x);
	ASSERT_EQUAL("cute_equals_test::No_output_operator{.x: 42}",result);

}
struct testing{
	int a{42};
private:
	std::byte b{0};
	inline static std::byte c{};
};

void test_generic_class_output(){
	testing x;
	auto result = cute::cute_to_string::to_string(x);
	ASSERT_EQUAL("cute_equals_test::testing{.a: 42, .b: 0x00}",result);

}

class testwithbase:No_output_operator,  public testing{
	int x{44};
};
void test_generic_class_with_base_output(){
	testwithbase x;
	auto result = cute::cute_to_string::to_string(x);
	ASSERT_EQUAL("cute_equals_test::testwithbase{inaccessible No_output_operator, cute_equals_test::testing{.a: 42, .b: 0x00}, .x: 44}",result);

}

enum class color{red,yellow,green};

void test_generic_enum(){
	color c{color::red};
	auto result = cute::cute_to_string::to_string(c);
	ASSERT_EQUAL("cute_equals_test::color::red",result);
}

void test_vector_of_enum(){
	using enum color;
	std::vector<color> v{green, yellow, red, color{42}};
	auto result = cute::cute_to_string::to_string(v);
	ASSERT_EQUAL("std::vector<cute_equals_test::color>[cute_equals_test::color::green, cute_equals_test::color::yellow, cute_equals_test::color::red, cute_equals_test::color{42}]",result);
}
void test_set_of_enum(){
	using enum color;
	std::set<color> v{green, yellow, red, color{42}};
	auto result = cute::cute_to_string::to_string(v);
	ASSERT_EQUAL("std::set<cute_equals_test::color>{cute_equals_test::color::red, cute_equals_test::color::yellow, cute_equals_test::color::green, cute_equals_test::color{42}}",result);

}
void test_pair_with_enum(){
	using enum color;
	constexpr auto apair=std::make_pair(red,42);
	auto result=cute::cute_to_string::to_string(apair);
	ASSERT_EQUAL("(cute_equals_test::color::red, 42)",result);
}
void test_tuple_with_enum(){
	using enum color;
	std::string s{"hello"};
	auto tup=std::make_tuple(red,42,s);
	auto result=cute::cute_to_string::to_string(tup);
	ASSERT_EQUAL(R"_((cute_equals_test::color::red, 42, "hello"))_",result);
}

//



//
void test_equalsTwoNaNFails()
{
    cute::assert_throws<cute::test_failure>([](){ASSERT_EQUAL(std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN());});
}
//
void test_doubleEqualsWithANaNFails(){
	cute::assert_throws<cute::test_failure>([&](){ASSERT_EQUAL_DELTA(0.0,std::numeric_limits<double>::quiet_NaN(),1.0);});
}

// some more tests inspired by strong type wrappers
struct Bool {
	explicit
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



}

namespace test_cute {
	void test_fail_macro(){
		try {
			FAIL();
			throw "should have failed";
		} catch (cute::test_failure const &e){
			ASSERT_EQUAL(__FILE__,e.loc.file_name());
			std::string what=e.what();
			std::string fail("FAIL()");
			//fail = CUTE_FUNCNAME_PREFIX+fail;
			ASSERT_EQUAL(fail,e.reason);
			ASSERT_EQUAL(fail,what.substr(what.size()-fail.size()));
			ASSERT_EQUAL_DELTA(__LINE__,e.loc.line(),10);
		}
	}
	void test_t_assert_macro(){
		try {
			ASSERT(0);
			throw "should have failed";
		} catch (cute::test_failure const &e){
			ASSERT_EQUAL(__FILE__,e.loc.file_name());
			std::string what=e.what();
			std::string msg{":ASSERT failed"}; // cannot get expression from call
			msg = std::source_location::current().function_name()+msg;
			ASSERT_EQUAL(msg,e.reason);
			ASSERT_EQUAL(msg,what.substr(what.size()-msg.size()));
			ASSERT_EQUAL_DELTA(__LINE__,e.loc.line(),10);
		}
	}
	void test_what(){
		std::source_location here=std::source_location::current();
		cute::test_failure ex("foo",here);
		ASSERT_EQUAL(std::string("foo"),ex.what());
		ASSERT_EQUAL(here.line(),ex.loc.line());
		ASSERT_EQUAL(here.file_name(),ex.loc.file_name());
	}
}
namespace throws_tests {
namespace{ // helpers not tests
void no_exception() {
}

void throws_std_exception () {
	throw std::exception();
}
void test_doesntthrow() {
	[[maybe_unused]] int avoid_warning_of_no_effect=0;
	cute::assert_throws<std::exception>([&](){++avoid_warning_of_no_effect;});
}
void test_fails_throws(){
	cute::assert_throws<std::exception>(no_exception);
}
void test_throwing_with_demangle_failure_therefore_suite_should_be_inserted_instead_of_wrapped() {
	throw std::logic_error("NOT A VALID TYPE NAME");
}

}
void test_throws() {
	cute::assert_throws<std::exception>(throws_std_exception);
}
void test_throws_with_code(){
	cute::assert_throws<std::string>([&](){throw std::string("oops");});
}
void test_throws_with_message() {
	cute::assert_throwsm<std::exception>("oops",throws_std_exception);
}
void test_that_fails_throws_failure(){
	cute::assert_throws<cute::test_failure>(test_fails_throws);
}
void test_that_throws_throws(){
	cute::assert_throws<std::exception>(throws_std_exception);
}
void test_that_doesnt_throw_fails_when_expected_to_throws(){
	cute::assert_throws<cute::test_failure>(test_doesntthrow);
}
void test_assert_throws_is_safe_against_throwing_test_failure(){
	cute::assert_throws<cute::test_failure>([&](){ASSERT(false);});
	try {
		cute::assert_throws<cute::test_failure>([&](){ASSERT(true);});
		throw true;
	} catch (cute::test_failure const&) {
	} catch (bool const &){
		FAILM("ASSERT_THROWS falsely catched test_failure");
	}

}

void test_assert_throws_with_demangling_error(){
	cute::assert_throws<std::logic_error>(test_throwing_with_demangle_failure_therefore_suite_should_be_inserted_instead_of_wrapped);
}

void test_throws_with_message_shows_the_message(){
	std::string const message("this should be the message");
	try {
		cute::assert_throwsm<std::exception>(message,throws_tests::no_exception);
	} catch (cute::test_failure const &expectfail){
		ASSERT_EQUAL(message,expectfail.reason);
	}
}
}


namespace filter_runner_test{

void test() {
}

void thisIsAtest_cute_filter_runnerTest() {
	char const *argv[] = { "dummy", "test suite1", "testsuite2#test1", 0,
			"testsuite2#test3", 0, 0 };
	constexpr auto ori= cute::make_test<^^test>;
	cute::suite s{
	ori,
	cute::test{test,"test1", std::source_location::current()},
	cute::test{test,"test2", std::source_location::current()},
	cute::test{test,"test3", std::source_location::current()},
	cute::test{test,"test4", std::source_location::current()}};

	cute::counting_listener<> l;
	cute::runner<cute::counting_listener<> > run{l,	sizeof(argv) / sizeof(*argv) - 1, argv};
	run(s, "test suite1");
	ASSERT_EQUAL(1,l.numberOfSuites);
	run(s, "testsuite2");
	ASSERT_EQUAL(2, l.numberOfSuites);
	ASSERT_EQUAL(s.size()+2, l.numberOfTests);
	ASSERT_EQUAL(0, l.errors);
	ASSERT_EQUAL(0, l.failedTests);
	ASSERT_EQUAL(7, l.successfulTests);
	ASSERT_EQUAL(l.numberOfTestsInSuites,l.numberOfTests);
}

void test_cute_filter_runner_ArgvFilter() {
	char const *argv[] = { "dummy", "testsuite1", "testsuite2#test1",
			"testsuite2#test3", 0 };
	std::vector<std::string> args;
	std::copy(argv + 1, argv + sizeof(argv) / sizeof(*argv) - 1,
			std::back_inserter(args));
	cute::runner_aux::ArgvTestFilter filter1("", args);
	ASSERT(filter1.shouldRun("any"));
	cute::runner_aux::ArgvTestFilter filter2("testsuite1", args);
	ASSERT(filter2.shouldrunsuite);
	ASSERT(filter2.shouldRun("test"));
	ASSERT(filter2.shouldRun("test1"));
	ASSERT(filter2.shouldRun("test2"));
	ASSERT(filter2.shouldRun("test3"));
	ASSERT(filter2.shouldRun("test4"));
	cute::runner_aux::ArgvTestFilter filter3("dummy", args);

	ASSERT(!filter3.shouldrunsuite);
	cute::runner_aux::ArgvTestFilter filter4("testsuite2", args);
	ASSERT(filter4.shouldrunsuite);
	ASSERT(!filter4.shouldRun("test"));
	ASSERT(filter4.shouldRun("test1"));
	ASSERT(!filter4.shouldRun("test2"));
	ASSERT(filter4.shouldRun("test3"));
	ASSERT(!filter4.shouldRun("test4"));

}
}
namespace xmlfilelistenertest{
void thisIsAtest_xml_file_openerTest() {
	std::filesystem::path p{};
	p /= "root";
	p /= "test";
	char const *argv[2]={p.c_str(),0};
	cute::xml_file_opener xmlfile(1,argv);
	ASSERT_EQUAL("test.xml",xmlfile.filename);
	ASSERT(xmlfile.out.is_open());
	xmlfile.out.close();
	ASSERT(remove(xmlfile.filename));
}
void test_xml_file_with_no_name(){
	char const *argv[1]={0};
	cute::xml_file_opener xmlfile(0,argv);
	ASSERT_EQUAL("testresult.xml",xmlfile.filename);
	ASSERT(xmlfile.out.is_open());
	xmlfile.out.close();
	ASSERT(remove(xmlfile.filename));
}
void test_xml_file_with_path(){
	std::filesystem::path p{"."};
	p /= "";
	char const *argv[2]={p.c_str(),0};
	cute::xml_file_opener xmlfile(1,argv);
	ASSERT_EQUAL(".xml",xmlfile.filename);
	ASSERT(xmlfile.out.is_open());
	xmlfile.out.close();
	ASSERT(remove(xmlfile.filename));
}

}

constexpr auto xmltestloc = std::source_location::current();
namespace {
void test_error(){
	throw "oops";
}
void test_failure_fun() {
	throw cute::test_failure("must fail",xmltestloc);
}
void test_xml_chars_in_failure(){
	throw cute::test_failure("and & <or/> \"hello\"",xmltestloc);
}
void test_emptyrun() {
	std::ostringstream out{};
	{
		cute::xml_listener<> listen(out);
	}
	ASSERT_EQUAL("<testsuites>\n</testsuites>\n",out.str());
}

}
struct xmllistenertest {

std::ostringstream out{};
std::optional<cute::xml_listener<>> optlistener{out};
std::optional<cute::runner<cute::xml_listener<>>> run{*optlistener};

void reset(int=0){
	run.reset();
	optlistener.reset();
}


void test_single_success(){
	{
		(*run)(CUTE(test_emptyrun));
		reset();
	}
	ASSERT_EQUAL("<testsuites>\n\t\t<testcase classname=\"\" name=\"test_emptyrun\"/>\n</testsuites>\n",out.str());

}


void test_single_failure(){
	{
		(*run)(CUTE(test_failure_fun));
		reset();
	}

	ASSERT_EQUAL(std::format("<testsuites>\n\t\t<testcase classname=\"\" name=\"test_failure_fun\">\n"
			"\t\t\t<failure message=\"{}:{} must fail\">\nmust fail\n\t\t\t</failure>\n"
			"\t\t</testcase>\n</testsuites>\n", xmltestloc.file_name(),xmltestloc.line()),out.str());
}

void test_xml_masking(){
	{
		(*run)(CUTE(test_xml_chars_in_failure));
		reset();
	}
	ASSERT_EQUAL(std::format("<testsuites>\n\t\t<testcase classname=\"\" name=\"test_xml_chars_in_failure\">\n"
			"\t\t\t<failure message=\"{}:{} and &amp; &lt;or/&gt; &quot;hello&quot;\">\n"
			"and &amp; &lt;or/&gt; &quot;hello&quot;\n"
			"\t\t\t</failure>\n\t\t</testcase>\n</testsuites>\n",xmltestloc.file_name(),xmltestloc.line()),out.str());
}

void test_xml_suite(){
	{
		cute::suite suite;
		suite.push_back(CUTE(test_failure_fun));
		suite.push_back(CUTE(test_emptyrun));
		(*run)(suite,"xml_test_suite");
		reset();
	}
	ASSERT_EQUAL(std::format("<testsuites>\n\t<testsuite name=\"xml_test_suite\" tests=\"2\">\n"
			"\t\t<testcase classname=\"xml_test_suite\" name=\"test_failure_fun\">\n"
			"\t\t\t<failure message=\"{}:{} must fail\">\nmust fail\n\t\t\t</failure>\n"
			"\t\t</testcase>\n"
			"\t\t<testcase classname=\"xml_test_suite\" name=\"test_emptyrun\"/>\n"
			"\t</testsuite>\n</testsuites>\n",xmltestloc.file_name(),xmltestloc.line()),out.str());
}


void test_for_error(){
	{
		(*run)(CUTE(test_error));
		reset();
	}

	ASSERT_EQUAL("<testsuites>\n\t\t<testcase classname=\"\" name=\"test_error\">\n"
			"\t\t\t<error message=\"test_error oops\" type=\"unexpected exception\">\noops\n\t\t\t</error>\n"
			"\t\t</testcase>\n</testsuites>\n",out.str());
}

}; // xmllistenertest

namespace tostringtests {
using cute::cute_to_string::to_string;
void test_simple_to_string(){
	std::string res=cute::cute_to_string::to_string(42);
	ASSERT_EQUAL("42",res);
}
void test_simple_string_to_string(){
	ASSERT_EQUAL("\"hallo Peter\\n\"",to_string("hallo Peter\n"));
}
void test_simple_char_array_to_string(){
	char a[]="Hello\tWorld\n";
	ASSERT_EQUAL("\"Hello\\tWorld\\n\"",to_string(a));
}
void test_simple_char_ptr_to_string(){
	char const * const a ="Hello\tWorld\n";
	ASSERT_EQUAL("\"Hello\\tWorld\\n\"",to_string(a));
}
void test_string_view_to_string(){
	std::string_view sv{"Hello\tWorld\n"};
	ASSERT_EQUAL("\"Hello\\tWorld\\n\"",to_string(sv));
}

void test_int_array_to_string(){
	int a[3];
	a[0]=1;
	a[1]=2;
	a[2]=3;
//	ASSERT_EQUAL("int [3]{\n1,\n2,\n3}",to_string(a));
	// std::format default rendering
	ASSERT_EQUAL("int [3][1, 2, 3]",to_string(a));
}
void test_byte_to_string(){
	std::byte b{42};
	std::string exp="0x2A";
	std::string res=to_string(b);
	ASSERT_EQUAL(exp,res);
}

void test_pair_to_string(){
//	ASSERT_EQUAL("[1 -> hallo]",to_string(std::make_pair(1,std::string("hallo"))));
	// std::format default rendering
	ASSERT_EQUAL(R"_(std::pair<int, std::string >(1, "hallo"))_",to_string(std::make_pair(1,std::string("hallo"))));
}
void test_tuple_to_string(){
//	ASSERT_EQUAL("std::tuple<int, int>{\n7,\n42}",to_string(std::tuple<int,int>(7,42)));
	// std::format default rendering
	ASSERT_EQUAL("std::tuple<int, int>(7, 42)",to_string(std::tuple<int,int>(7,42)));
}
void test_vector_pair_to_string(){
	typedef std::vector<std::pair<int,int> > Vec;
	Vec v;
	constexpr auto vecname = std::meta::display_string_of(std::meta::dealias(^^Vec));

	v.push_back(std::make_pair(42,4));
//	ASSERT_EQUAL(std::format("{}",vecname)+"{\n[42 -> 4]}",to_string(v));
	// std::format default rendering
	ASSERT_EQUAL(std::format("{}",vecname)+"[(42, 4)]",to_string(v));
}

void test_set_to_string(){
	std::set<int> s;
	s.insert(42); s.insert(1); s.insert(100);
	std::string res=to_string(s);
	constexpr auto setname = std::meta::display_string_of(^^std::set<int>);
//	ASSERT_EQUAL(std::format("{}",setname)+"{\n1,\n42,\n100}",res);
	// std::format default rendering
	ASSERT_EQUAL(std::format("{}",setname)+"{1, 42, 100}",res);
}
void test_multiset_to_string(){
	std::multiset<int> s;
	s.insert(42); s.insert(42); s.insert(100);
	std::string res=to_string(s);
	constexpr auto setname = std::meta::display_string_of(^^std::multiset<int>);

//	ASSERT_EQUAL(std::format("{}",setname)+"{\n42,\n42,\n100}",res);
	// std::format default rendering
	ASSERT_EQUAL(std::format("{}",setname)+"{42, 42, 100}",res);
}

void test_map_to_string(){
	std::map<std::string,std::string> m;
	constexpr auto mapname = "std::map<std::string, std::string >";
	//cute::makeNameFor<decltype(m)>; // a bit tautological... could use literal

	m["one"]="two";
	m["three"]="four";
	m["five"]="six";
//	std::string exp=std::format("{}",mapname)+"{\n[five -> six],\n[one -> two],\n[three -> four]}";
	// std::format default rendering
	std::string exp=std::format("{}",mapname)+R"_({"five": "six", "one": "two", "three": "four"})_";
	std::string res=to_string(m);
	ASSERT_EQUAL(exp,res);
}
void test_multimap_to_string(){
	std::multimap<std::string, std::string > m;
	constexpr auto mapname = "std::multimap<std::string, std::string >";
	//cute::makeNameFor<decltype(m)>; // a bit tautological... could use literal

	m.insert(std::pair<std::string,std::string>("one","two"));
	m.insert(std::pair<std::string,std::string>("one","three"));
	m.insert(std::pair<std::string,std::string>("three","four"));
//	std::string exp=std::format("{}",mapname)+"{\n[one -> two],\n[one -> three],\n[three -> four]}"; // fragile!
	// std::format default rendering
	std::string exp=std::format("{}",mapname)+R"_({"one": "two", "one": "three", "three": "four"})_"; // fragile!
	std::string res=to_string(m);
	ASSERT_EQUAL(exp,res);
}
void test_vector_of_byte_to_string(){
	std::vector<std::byte> v{static_cast<std::byte>(21), static_cast<std::byte>(42)};
	constexpr auto vecname = std::meta::display_string_of(^^std::vector<std::byte>);

//	std::string exp=std::format("{}",vecname)+"{\n0x15,\n0x2A}";
	// std::format default rendering
	std::string exp=std::format("{}",vecname)+"[0x15, 0x2A]";
	std::string res=to_string(v);
	ASSERT_EQUAL(exp,res);
}
}

namespace cute_relops_test {
void test_cute_assert_less_success() {
	ASSERT_LESS(4,5);
}
void test_cute_assert_less_fail(){
	assert_throws<cute::test_failure>([]{ASSERT_LESS(4,2);});
}
void test_cute_assert_greater_success(){
	ASSERT_GREATER(10.0,9.9);
}
void test_cute_assert_greater_fail(){
	assert_throws<cute::test_failure>([]{ASSERT_GREATER(2,3);});
}
void test_cute_assert_less_equal_success(){
	ASSERT_LESS_EQUAL(4,4);
}
void test_cute_assert_less_equal_fail(){
	assert_throws<cute::test_failure>([]{ASSERT_LESS_EQUAL(4,3);});
}
void test_cute_assert_greater_equal_success(){
	int const x=4;
	int const y=4;
	ASSERT_GREATER_EQUAL(x,y);
}
void test_cute_assert_greater_equal_fail(){
	assert_throws<cute::test_failure>([]{ASSERT_GREATER_EQUAL(4,5);});
}
void test_cute_assert_not_euqal_to_success(){
	ASSERT_NOT_EQUAL_TO(1,2);
}
void test_cute_assert_not_equal_to_fail(){
	assert_throws<cute::test_failure>([]{ASSERT_NOT_EQUAL_TO(4,4);});
}

}

namespace cute_data_driven_test {
struct test_data {
	bool check{};
	cute::source_location loc{};
};

test_data table[] = {
//#line 100 /* for writing stable tests */
		{true},
		{false}
};

void thisIsAtest_cute_data_drivenTest() {
	ASSERT(table[0].check,table[0].loc);
	assert_throws<cute::test_failure>([]{ASSERTM("a test",table[1].check,table[1].loc);});
}
void test_cute_data_driven_test_iteration(){
	for(auto t:table)
		if (t.check) ASSERT(t.check,t.loc);
		else assert_throws<cute::test_failure>([&]{ASSERT(t.check,t.loc);});
}

using l=cute::source_location;

struct test_eq_data {
	double input{},expected{};
	l loc; // default value leads to position of closing brace
} eq_table [] = {
		{ 4,16, l{}},
		{ 2.5,6.25,l{} }
}; // lines above are 1036 and 1037
constexpr std::size_t linesum_to_expect = 2*__LINE__ - (2+3);
double square(double x){return x*x;}

void test_cute_data_driven_equality_demo(){
	for(test_eq_data t: eq_table){
		ASSERT_EQUAL(t.expected,square(t.input),t.loc);
	}
}
void test_cute_data_driven_equal_delta_demo(){
	for(test_eq_data t: eq_table){
		ASSERT_EQUAL_DELTA(t.expected,square(t.input),0.1,t.loc);
	}
}

void test_cute_data_driven_less_demo(){
	for(test_eq_data t: eq_table){
		ASSERT_LESS(t.input,t.expected,t.loc);
	}
}
void test_cute_data_driven_greater_demo(){
	for(test_eq_data t: eq_table){
		ASSERT_GREATER(t.expected,t.input,t.loc);
	}
}
void test_cute_data_driven_not_equal_to_demo(){
	std::size_t linenumbersum{};
	for(test_eq_data t: eq_table){
		ASSERT_NOT_EQUAL_TO(t.input,t.expected,t.loc);
		linenumbersum += t.loc.line();
	}
	ASSERT_EQUAL(linesum_to_expect,linenumbersum);
}
void test_cute_data_driven_less_equal_demo(){
	for(test_eq_data t: eq_table){
		ASSERT_LESS_EQUAL(t.input,t.expected,t.loc);
	}
}
void test_cute_data_driven_greater_equal_demo(){
	for(test_eq_data t: eq_table){
		ASSERT_GREATER_EQUAL(t.expected,square(t.input),t.loc);
	}
}

}

#define CUTE_SUITE_TEST(s) cute::suite_test{s}


namespace cute_suite_tests {
namespace {
// dummy tests
std::size_t counter=0;
void testOK(){ ++counter; }
void testFails() { --counter; FAIL();}
}
// tests:
void test_suite_OK(){
	cute::suite s;
	counter =0;
	s += {testOK,"testOK"};
	s +=  {testOK,"testOK"};
	s +=  {testOK,"testOK"};
	CUTE_SUITE_TEST(s)();
	ASSERT_EQUAL(s.size(),counter);
}
void test_suite_fails(){
	cute::suite suite_that_fails;
	counter =0;
	suite_that_fails.push_back(CUTE(testOK)); // 1
	suite_that_fails.push_back( CUTE(testOK)); // 2
	suite_that_fails.push_back(CUTE(testFails)); // 1
	suite_that_fails.push_back( CUTE(testFails)); // 0 -> should not be reached
	auto t = CUTE_SUITE_TEST(suite_that_fails);
	try {
		t();
		throw "should have failed";
	} catch (cute::test_failure &){
		ASSERT_EQUAL(1u,counter);
	}
}
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
namespace {
cute::suite thelambdasuite={
		{[]{ ASSERTM("a test",true);},"first"},
		{[]{ ASSERTM("another test",true);},"second"}
};
}

void test_lambda_suite(){

	cute::counting_listener<> lis;
	cute::suite s;
	s += thelambdasuite;
	cute::runner{lis}(s,"lambda suite");
	ASSERT(lis.numberOfTests>=1);
}


}

namespace runner_tests{
struct mock_listener {
	using size_t=std::size_t;
	size_t begincount;
	size_t endcount;
	size_t startcount;
	size_t successcount;
	size_t failurecount;
	size_t errorcount;
	size_t suitetestcount;
	std::vector<std::string> infomessages;
	std::vector<std::string>  errormessages;
	std::vector<std::string>  successmessages;
	mock_listener()
	:begincount(0),endcount(0),startcount(0)
	,successcount(0),failurecount(0),errorcount(0)
	,suitetestcount(0){}
	void begin(suite const &,char const *info,size_t n){
		++begincount;
		suitetestcount += n;
		infomessages.push_back(info);
	}
	void end(suite const &,char const *){++endcount;}
	void start(test const &){++startcount;}
	void success(test const &, char const *ok){
		++successcount;
		successmessages.push_back(ok);
	}
	void failure(test const &,test_failure const &){++failurecount;}
	void error(test const &,char const *what){
		++errorcount;
		errormessages.push_back(what);
	}
};
namespace {
void test_success(){}
void test_failing(){ ASSERT(false);}
void test_error_cstr(){ throw "error";}
void test_error_exception() { throw std::exception();}
void test_error_string(){ throw std::string("error");}

}
void test_cute_runner(){
	mock_listener l{};
	cute::runner run{l};
	suite s{};
	s += CUTE(test_success);
	ASSERT(run(s,"single success test suite"));
	ASSERT_EQUAL(1,l.suitetestcount);
	s += CUTE(test_failing);
	s += CUTE(test_error_cstr);
	s += CUTE(test_error_string);
	s += CUTE(test_error_exception);
	bool result=run(s,"test_cute_runner_suite");
	ASSERT(!result);
	ASSERT_EQUAL(2,l.begincount);
	ASSERT_EQUAL(2,l.endcount);
	ASSERT_EQUAL(2,l.successcount);
	ASSERT_EQUAL(1,l.failurecount);
	ASSERT_EQUAL(3,l.errorcount);
	ASSERT_EQUAL(6,l.suitetestcount);
	ASSERT_EQUAL(2u,l.infomessages.size());
	ASSERT_EQUAL("single success test suite",l.infomessages[0]);
	ASSERT_EQUAL("test_cute_runner_suite",l.infomessages[1]);
	ASSERT_EQUAL(3u,l.errormessages.size());
	ASSERT_EQUAL("error",l.errormessages[0]);
	ASSERT_EQUAL("error",l.errormessages[1]);
	std::string errormsg2=l.errormessages[2];
	std::string errmsgexpected="exception";
	ASSERT_EQUAL(errmsgexpected,errormsg2.substr(errormsg2.size()-errmsgexpected.size()));
	ASSERT_EQUAL(2u,l.successmessages.size());
	ASSERT_EQUAL("OK",l.successmessages[0]);

}
}


int main(int argc, char const *argv[]){
	using namespace tests;
	cute::ide_listener<> fornothing{std::cout};
	runner runit{fornothing, argc,argv};
//	runit(cute::collectTestsWithName<^^tests>);
//	runit(cute::collectTestsWithName<^^tests::SomeSuite>);
	runit(cute::collectTestsWithName<^^brain_dead_tests>);
	runit(cute::collectTestsWithName<^^throws_tests>);
	runit(cute::collectTestsWithName<^^tap_listener_suite>);
	runit(cute::collectTestsWithName<^^test_cute>);
	runit(cute::collectTestsWithName<^^cute_equals_test>);
	runit(cute::collectTestsWithName<^^filter_runner_test>);
	runit(cute::collectTestsWithName<^^xmlfilelistenertest>);
	runit(cute::collectTestsWithName<^^xmllistenertest>);
	runit(cute::collectTestsWithName<^^tostringtests>);
	runit(cute::collectTestsWithName<^^cute_relops_test>);
	runit(cute::collectTestsWithName<^^cute_data_driven_test>);
	runit(cute::collectTestsWithName<^^cute_suite_tests>);
	runit(cute::collectTestsWithName<^^runner_tests>);


#if 0
	suite s;
	// the following test produces one of the 2 expected errors, since it throws
	//s += CUTE(simpleTestFunction);
	s += CUTE(test_SimpleTestFunctionThrows);
	s += SimpleTestFunctionCalledTest();
	s += CUTE(test_shouldFailThrowsFailure);
	suite throwing=test_cute_expect();
	s.insert(s.end(),throwing.begin(),throwing.end());
	s += CUTE_SUITE_TEST(test_repeated_test());
	s += CUTE(test_cute_runner);
	s += CUTE_SUITE_TEST(test_cute_suite_test());
	s += CUTE(test_cute_suite);
	s += CUTE(test_lambda_suite);
	s += CUTE_SUITE_TEST(test_cute_test_incarnate());
	s += CUTE_SUITE_TEST(test_cute_test());
	s += CUTE_SUITE_TEST(test_cute_testmember());
	s += CUTE_SUITE_TEST(test_cute());
	//---
	s += CUTE_INCARNATE(to_incarnate_without);
	s += CUTE_INCARNATE_WITH_CONTEXT(to_incarnate,std::ref(std::cout));
	s += CUTE_CONTEXT_MEMFUN(std::ref(std::cerr),to_incarnate,operator());
#ifndef DONT_USE_IOSTREAM
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::counting_listener<cute::ide_listener<> > > l(xmlfile.out);
#else
	cute::counting_listener<cute::ide_listener<>> l{};
#endif
	cute::makeRunner(l,argc,argv)(s,"all cute tests");
	cout << flush;
	cerr << flush;
	cerr << l.numberOfTests << " Tests - expect " << s.size() << endl;
	cerr << l.failedTests << " failed - expect 0 failures" << endl;
	cerr << l.errors << " errors - expect 0 errors" << endl;
	return int(l.failedTests);//silence warning
#endif
}
