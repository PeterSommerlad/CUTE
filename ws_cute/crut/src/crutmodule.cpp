/*********************************************************************************
 * This file is the module version of CUTE3X.
 *
 * Copyright (c) 2026 Peter Sommerlad
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

export module crut;
import std;

// cute as module...


namespace cute {

export
template<std::meta::info i>
requires (std::meta::has_identifier(i))
constexpr auto fqn {
[](){
	using namespace std::meta;
	using std::define_static_string;
	if (has_parent(i)) {
		constexpr auto parent = parent_of(i);
		if constexpr (is_type(parent) || is_namespace(parent)){
			std::string result{"::"};
			result += identifier_of(i);
			if  (parent == ^^::) {
				return define_static_string(result);
			} else if constexpr (has_identifier(parent)){
				return define_static_string(fqn<parent> + result);
			} else if constexpr (is_type(parent) && is_enum_type(parent) && is_enumerator(i) && has_parent(parent)) {
				// an anonymous enum's enumerator is inserted into the outer scope
				constexpr auto grandparent = parent_of(parent);
				if  (has_identifier(grandparent) && (is_type(grandparent) || is_namespace(grandparent))){
					return define_static_string( fqn<grandparent> + result);
				}
			}
		}
	}
	return std::define_static_string(std::meta::identifier_of(i));
}()};

export
template<typename T>
constexpr auto makeNameFor { // allow template speci
	[](){
		using namespace std::meta;
		std::string name = std::define_static_string(display_string_of(remove_const(remove_reference(^^T))));
		// patch for std::string
		using str=std::string; // gcc modules make this a using declaration that cannot reflect
		constexpr auto stdstr= "std::string";
		// on gcc display_string_of already removes the inline namespace and default template arguments
		// might need to DIY when more complicated situations arise.
		constexpr auto toreplace = std::meta::display_string_of(std::meta::dealias(^^str));
		static_assert(stdstr != toreplace,toreplace);

		while(name.contains(toreplace)){
			name.replace(name.find(toreplace),toreplace.size(),stdstr);
		}
		return std::define_static_string(name);
		// patch compiler-intrinsic details not easily possible due to lack of reflection apis
	}()
};

export
template<typename T>
constexpr auto demangleTypeOf(T ){
	constexpr auto result=makeNameFor<T>;
	return result;
}


namespace cute_to_string {
template <typename T>
concept is_to_string_able = (requires (T t) { { std::to_string(t) }  -> std::same_as<std::string>;});

template<typename T>
concept non_formattable_class = std::is_class_v<T> && (not std::formattable<T,char>);

template<typename T>
concept non_formattable_enum = std::is_enum_v<T> && (not std::formattable<T,char>);

template<typename T>
constexpr bool tuple_or_pair_like{false};
template<typename F,typename S>
constexpr bool tuple_or_pair_like<std::pair<F,S>>{true};
template<typename ...E>
constexpr bool tuple_or_pair_like<std::tuple<E...>>{true};
template<typename T>
constexpr bool tuple_like = tuple_or_pair_like<std::remove_cvref_t<T>>;

template<typename T>
concept non_formattable_tuple =  tuple_like<T> && (not std::formattable<T,char>);

// quote all reasonable string types
	export{
		template<std::size_t N>
		constexpr std::string to_string(char const  (&s)[N]){
			return std::format("{:?}",s); // implicit quoted
		}
		constexpr std::string to_string(char const *const &s){
			return std::format("{:?}",s); // implicit quoted
		}
		constexpr std::string to_string(std::string const &s){
			return std::format("{:?}",s);
		}
		constexpr std::string to_string(std::string_view const &s){
			return std::format("{:?}",s);
		}
		constexpr auto to_string(is_to_string_able auto const &something){
			return std::to_string(something);
		}

		// special cases
		constexpr std::string to_string(std::byte b){
			return std::format("0x{:02X}",static_cast<std::underlying_type_t<std::byte>>(b));
		}
	}

		template<typename T>
		constexpr std::string generic_to_string(T const &){
			return std::format("don't know how to print type {}",cute::makeNameFor<T>);
		}
		template<non_formattable_class CL>
		constexpr std::string generic_to_string(CL const &o);
		template<non_formattable_class CL>
		requires tuple_like<CL>
		constexpr std::string generic_to_string(CL const &p);
		template<non_formattable_class CL>
		requires (std::ranges::range<CL>)
		constexpr std::string generic_to_string(CL const &o);
		template<non_formattable_enum E>
		constexpr std::string generic_to_string(E const &o);



		// this is the interface:
export
		template <typename T>
        requires (not cute_to_string::is_to_string_able<T>)
		std::string to_string(T const &t) {
			if constexpr (std::convertible_to<T,std::string>){
				return t; // only std::string and char const * like will be quoted...
			}
			if constexpr (std::formattable<T,char>) {
				return std::format("{}{}",makeNameFor<decltype(t)>,std::format("{}",t));
			} else {
				return generic_to_string(t);
			}
		}

// helper meta functions to mimic std::format formatting

template<typename T>
constexpr bool some_std_map{false};
template<typename ...KVCA>
constexpr bool some_std_map<std::map<KVCA...>>{true};
template<typename ...KVCA>
constexpr bool some_std_map<std::multimap<KVCA...>>{true};
template<typename ...KVCA>
constexpr bool some_std_map<std::unordered_map<KVCA...>>{true};
template<typename ...KVCA>
constexpr bool some_std_map<std::unordered_multimap<KVCA...>>{true};

// tests some_std_map
static_assert(some_std_map<std::map<int,int,std::greater<>>>);
static_assert(some_std_map<std::pmr::map<int,int,std::greater<>>>);
static_assert(some_std_map<std::multimap<int,int>>);
static_assert(some_std_map<std::unordered_multimap<int,int>>);
static_assert(not some_std_map<int>);

template<typename T>
constexpr bool some_std_set{false};
template<typename ...KCA>
constexpr bool some_std_set<std::set<KCA...>>{true};
template<typename ...KCA>
constexpr bool some_std_set<std::multiset<KCA...>>{true};
template<typename ...KCA>
constexpr bool some_std_set<std::unordered_set<KCA...>>{true};
template<typename ...KCA>
constexpr bool some_std_set<std::unordered_multiset<KCA...>>{true};

// sequence ranges are wrapped with [], associative ranges with {}
template<typename T, bool=true>
constexpr char openrange{'['};
template<typename T>
constexpr char openrange<T,some_std_map<T> || some_std_set<T>>{'{'};

static_assert('[' == openrange<std::vector<int>>);
static_assert('{' == openrange<std::unordered_set<int>>);

template<typename T, bool=true>
constexpr char closerange{']'};
template<typename T>
constexpr char closerange<T,some_std_map<T> || some_std_set<T>> {'}'};
static_assert(']' == closerange<std::vector<int>>);
static_assert('}' == closerange<std::unordered_set<int>>);

template<typename K, typename V>
std::string to_string_map_element(std::pair<K,V> const &mapentry){
	std::string result{};
	result.append(cute::cute_to_string::to_string(mapentry.first));
	result.append(": ");
	result.append(cute::cute_to_string::to_string(mapentry.second));
	return result;
}

template<std::ranges::range aSequenceRange>
constexpr std::string range_elements_to_string(aSequenceRange const &rng){
	std::string result{};
	bool first=true;
	for (auto element:rng){
		if (first) first=false;
		else result.append(", ");
		result.append(cute::cute_to_string::to_string(element));
	}

	return result;
}


template<non_formattable_class aMap>
requires some_std_map<aMap>
constexpr std::string range_elements_to_string(aMap const &rng){
	std::string result{};
	bool first=true;
	for (auto element:rng){
		if (first) first=false;
		else result.append(", ");
		result.append(cute::cute_to_string::to_string_map_element(element));
	}

	return result;
}
// tuple-like are wrapped with(), unless key-value in maps
template<non_formattable_class CL>
requires tuple_like<CL>
constexpr std::string generic_to_string(CL const &p){
	std::string result{"("};
	auto const &[...elts] = p;
	bool first=true;
	template for (auto const &elt: p){ // cannot use std::ranges::join here
		if(first) first=false;
		else result.append(", ");
		result.append(cute::cute_to_string::to_string(elt));
	}
	result.append(")");
	return result;
}


template<non_formattable_class CL>
requires (std::ranges::range<CL>)
constexpr std::string generic_to_string(CL const &rng){
	// diy range of non-formattable element. format: vector/array uses [], map,set
	std::string result{makeNameFor<decltype(rng)>};
	result.push_back(openrange<CL>);
	result.append(range_elements_to_string(rng));
	result.push_back(closerange<CL>);
	return result;
}


template<non_formattable_class CL>
constexpr std::string generic_to_string(CL const &o){
	constexpr std::meta::info theclass = ^^CL;
	constexpr auto name = std::meta::display_string_of(theclass);

	static constexpr auto datamembers = std::define_static_array(
			nonstatic_data_members_of(theclass,std::meta::access_context::unchecked())
			| std::views::filter([](std::meta::info mem){return has_identifier(mem);})
			); // cannot inline, because template for requires constant expression and members_of creates a vector
			// filter might be redundant...
	static constexpr auto bases = std::define_static_array(
			bases_of(theclass,std::meta::access_context::unchecked()));

	std::string result;
	bool first=true;
	template for (constexpr auto base: bases){
		constexpr auto thebase = type_of(base);
		using basetype = typename [: thebase :];
		if (first) first=false;
		else result.append(", ");
		if constexpr (is_accessible(base,std::meta::access_context::current())){
			result.append(cute::cute_to_string::to_string<basetype>(o));
		} else {
			result.append("inaccessible ");
			if constexpr (has_identifier(base)){
				result.append(identifier_of(base));
			}
		}
		}
	template for (constexpr auto mem : datamembers){
			if (first)  first = false;
			else result.append(", ");
			result.append(".");
			result.append(std::meta::identifier_of(mem));
			result.append(": ");
			result.append(cute::cute_to_string::to_string(o.[:mem:]));
	}
	return std::format("{}{{{}}}",name,result);
}

template<non_formattable_enum E>
constexpr std::string generic_to_string(E const &e){
	constexpr std::meta::info theenum = ^^E;
	static_assert(is_enum_type(theenum));
    constexpr static auto enumerators = std::define_static_array(enumerators_of(^^E));


	constexpr auto name = std::meta::display_string_of(theenum);
    std::string result(name);
    bool found=false;
    template for (constexpr auto enumerator : enumerators)
    {
    	constexpr E current = [:enumerator:];
    	if (e == current){
    		found = true;
    		result.append("::");
    		result.append(identifier_of(enumerator));
    	}
    }

    if (not found){
    	result.append("{");
        result.append(to_string(std::to_underlying(e)));
        result.append("}");
    }

    return result;
}



	}
/// end of to_string infrastructure

// trampoline to enable std::formatter specialization
export
struct source_location: std::source_location{
	constexpr source_location(std::source_location loc=std::source_location::current()):std::source_location{loc}{} // deliberately implicit
	template<typename Char=char, typename Traits=std::char_traits<Char>>
	friend std::basic_ostream<Char,Traits>& operator<<(std::basic_ostream<Char,Traits>& out, source_location const &loc){
		return out << loc.file_name() << ':'<<loc.line() << ':' << loc.column() <<':'<<loc.function_name();
	}
	// would be nice to have this...
//	constexpr std::size_t offset() const {
//		return 0;
//	}
};

	export
	struct test_failure {
		std::string reason{};
		cute::source_location loc{std::source_location::current()};
		char const * what() const { return reason.c_str(); }
	};
	// provide legacy names for assertions to ease migration and make it more obvious that these are tests
	export
	constexpr void ASSERTM(char const *message,bool condition, std::source_location loc=std::source_location::current()){
		if (not condition) throw test_failure{ std::format("{}:{}",loc.function_name(),message),loc};
	}
	export
	constexpr void ASSERT(bool condition, std::source_location loc=std::source_location::current()){
		ASSERTM("ASSERT failed",condition,loc);
	}
	export
	constexpr void FAILM(char const *message,std::source_location loc=std::source_location::current()){
		throw test_failure{message,loc};
	}
	export
	constexpr void FAIL(std::source_location loc=std::source_location::current()){
		FAILM("FAIL()", loc);
	}


export // for testing purposes only
template <typename ExpectedValue, typename ActualValue>
constexpr std::string diff_values(ExpectedValue const &expected
		, ActualValue const & actual
		, char const *left="expected"
				, char const *right="but was"){
	// construct a simple message...to be parsed by IDE support
	std::string res;
	res += ' ';
	res += left;
	res+=":\t" + cute_to_string::to_string(expected)+'\t'; // tabs are significant for Cevelop
	//	res+=":\t" + cute_to_string::backslashQuoteTabNewline(cute_to_string::to_string(expected))+'\t';
	res += right;
	res +=":\t"+cute_to_string::to_string(actual)+'\t';
	//  	res +=":\t"+cute_to_string::backslashQuoteTabNewline(cute_to_string::to_string(actual))+'\t';
	return res;
}



	constexpr auto do_equals(char const * const &expected, char const * const &actual){
		return std::string_view{expected} == std::string_view{actual};
	}

	template<std::integral EXP, std::integral ACT>
	constexpr auto do_equals(EXP const &expected, ACT const &actual){
		if constexpr (sizeof(EXP) > sizeof(ACT)) {
			return expected == static_cast<EXP>(actual); // promote to bigger
		} else if constexpr (sizeof(ACT) > sizeof(EXP)) {
			return static_cast<ACT>(expected) == actual; // promote to bigger
		} else 	if constexpr (std::is_signed_v<EXP> != std::is_signed_v<ACT>){
			return expected == static_cast<EXP>(actual); // avoid comparison warning
		} else {
			return expected == actual;
		}
	}
	template <typename EXP, typename ACT, typename DEL>
	constexpr auto do_equals_with_delta(EXP const &expected
			,ACT const &actual
			,DEL const &delta) {
		using common_may_be_unsigned = std::common_type_t<EXP,ACT,DEL>;

		constexpr auto compute_common_type=[](){
			if constexpr (std::is_unsigned_v<common_may_be_unsigned>){
				return std::make_signed_t<common_may_be_unsigned>{};
			} else {
				return common_may_be_unsigned{};
			} // alternative would have been a template specialization with concepts on namespace scope
		}; // never called....

		using common = decltype(compute_common_type());
		using std::abs; // allow for user-defined types with abs overload
		return (abs(static_cast<common>(delta)) >= abs(static_cast<common>(expected)-static_cast<common>(actual)));
	}
	template<typename EXP, typename ACT>
	requires (std::floating_point<EXP> or std::floating_point<ACT>)
	constexpr auto do_equals(EXP const &expected, ACT const &actual){
		using common = std::common_type_t<EXP,ACT>;
		common const automatic_delta_masking_last_significant_digit{
			(10*std::numeric_limits<common>::epsilon())*(expected?expected:EXP{1})
		};
		return do_equals_with_delta(expected,actual,automatic_delta_masking_last_significant_digit);
	}

	template<typename EXP, typename ACT>
	constexpr auto do_equals(EXP const &expected, ACT const &actual){
		return expected == actual;
	}


	export 	template<typename MSG, typename EXP, typename ACT >
	constexpr void ASSERT_EQUALM(MSG const &msg, EXP const &expected, ACT const &actual, std::source_location loc=std::source_location::current()){
		if (do_equals(expected,actual)) return; // success
		if not consteval { // std::format not yet consteval in GCC 16.1
			if constexpr (std::formattable<EXP,char>
			&& std::formattable<ACT,char>){
				throw test_failure{std::format("{}{}",std::format("{:?}",msg),cute::diff_values(expected,actual)),loc};
			} else {
				constexpr auto expname=std::meta::display_string_of(^^EXP);
				constexpr auto actname=std::meta::display_string_of(^^ACT);
				throw test_failure{std::format("{} cannot format type {} or {}",std::format("{:?}",msg),expname, actname), loc};
			}
		}
		throw test_failure{msg, loc};
	}
	export	template<typename EXP, typename ACT>
	constexpr void ASSERT_EQUAL(EXP const &expected, ACT const &actual, std::source_location loc=std::source_location::current()){
		ASSERT_EQUALM("equality failed:",expected,actual,loc);
	}

	export 	template<typename MSG, typename EXP, typename ACT, typename DEL>
	constexpr void ASSERT_EQUAL_DELTAM(MSG const &msg, EXP const &expected, ACT const &actual, DEL const &delta, std::source_location loc=std::source_location::current()){
		if (do_equals_with_delta(expected,actual,delta)) return; // success
		if (not std::is_constant_evaluated()) {
			if constexpr (std::formattable<EXP,char>
			&& std::formattable<ACT,char>){
				throw test_failure{std::format("{}{}",std::format("{:?}",msg),cute::diff_values(expected,actual)),loc};
			} else {
				constexpr auto expname=cute::fqn<^^EXP>;
				constexpr auto actname=cute::fqn<^^ACT>;
				throw test_failure{std::format("{} cannot format type {} or {}",std::format("{:?}",msg),expname, actname), loc};
			}
		}
		throw test_failure{msg, loc};

	}
	export	template<typename EXP, typename ACT, typename DEL>
	constexpr void ASSERT_EQUAL_DELTA(EXP const &expected, ACT const &actual, DEL const &delta, std::source_location loc=std::source_location::current()){
		ASSERT_EQUAL_DELTAM("equality with error margin failed",expected,actual,delta,loc);
	}


template <typename LeftValue, typename RightValue>
std::string compare_values(LeftValue const &left
		, RightValue const & right){
	return cute::diff_values(left,right,"left","right");
}

template <template<typename> class RELOP,typename TL, typename TR>
void assert_relop(TL const &left
		, TR const &right
		,std::string const &msg
		,cute::source_location loc) {
	if (RELOP<TL>()(left,right)) return;
	throw test_failure(msg + compare_values(left,right),loc);
}

export
void ASSERT_LESSM(std::string const &msg, auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	assert_relop<std::less>(left,right,msg,loc);
}
export
void ASSERT_LESS(auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	ASSERT_LESSM(cute::cute_to_string::to_string(left)+ " < " + cute::cute_to_string::to_string(right), left, right, loc);
}
export
void ASSERT_LESS_EQUALM(std::string const &msg, auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	assert_relop<std::less_equal>(left,right,msg,loc);
}
export
void ASSERT_LESS_EQUAL(auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	ASSERT_LESS_EQUALM(cute::cute_to_string::to_string(left)+ " < " + cute::cute_to_string::to_string(right), left, right, loc);
}
export
void ASSERT_GREATERM(std::string const &msg, auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	assert_relop<std::greater>(left,right,msg,loc);
}
export
void ASSERT_GREATER(auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	ASSERT_GREATERM(cute::cute_to_string::to_string(left)+ " < " + cute::cute_to_string::to_string(right), left, right, loc);
}
export
void ASSERT_GREATER_EQUALM(std::string const &msg, auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	assert_relop<std::greater_equal>(left,right,msg,loc);
}
export
void ASSERT_GREATER_EQUAL(auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	ASSERT_GREATER_EQUALM(cute::cute_to_string::to_string(left)+ " < " + cute::cute_to_string::to_string(right), left, right, loc);
}
export
void ASSERT_NOT_EQUAL_TOM(std::string const &msg, auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	assert_relop<std::not_equal_to>(left,right,msg,loc);
}
export
void ASSERT_NOT_EQUAL_TO(auto const & left, auto const &right, cute::source_location loc=std::source_location::current()){
	ASSERT_NOT_EQUAL_TOM(cute::cute_to_string::to_string(left)+ " < " + cute::cute_to_string::to_string(right), left, right, loc);
}
struct assert_throws_failure_exception {
	struct cute::test_failure original;
	assert_throws_failure_exception(std::string const &r,std::source_location loc):
		original{r,loc}{}
};

export		template<typename EXC, typename MSG, std::invocable LAMBDA_EXPR >
constexpr void assert_throwsm(MSG const &msg,LAMBDA_EXPR &&expr,std::source_location loc= std::source_location::current()){
	try {
		{ expr() ; }
		throw cute::assert_throws_failure_exception(msg,loc); \
	} catch(EXC const &){ \
	} catch(cute::assert_throws_failure_exception const &atf){throw atf.original;} \

}
export 		template<typename EXC, std::invocable LAMBDA_EXPR >
	void assert_throws(LAMBDA_EXPR &&expr,std::source_location loc= std::source_location::current()){
		constexpr auto lamname = std::meta::display_string_of(^^LAMBDA_EXPR);
		assert_throwsm<EXC>(std::format("expecting {} to throw",lamname),std::forward<LAMBDA_EXPR>(expr),loc);
}


template<std::meta::info func>
concept isVoidFunc = is_function(func) && parameters_of(func).size()==0 && return_type_of(func) == ^^void;
template<std::meta::info memfun>
concept isVoidMemberFunction = is_function(memfun)
						   && !is_special_member_function(memfun)
						   && parameters_of(memfun).size()==0
						   && return_type_of(memfun) == ^^void;

constexpr auto voidMemfunc = [](std::meta::info memfun) {
	return is_function(memfun)
			&& !is_special_member_function(memfun)
			   && parameters_of(memfun).size()==0
			   && return_type_of(memfun) == ^^void;
};
constexpr auto voidfunc = [](auto info){
	return is_function(info) && parameters_of(info).size()==0 && return_type_of(info) == ^^void;
};


namespace { // tests:
	[[maybe_unused]] void aTest(){}
	[[maybe_unused]] int notATest(){return 0;}
	[[maybe_unused]] void alsoNotATest(int){}
	static_assert(isVoidFunc<^^aTest>);
	static_assert(not isVoidFunc<^^notATest>);
	static_assert(not isVoidFunc<^^alsoNotATest>);
}


// a bit more limited than old CUTE but usable at compile time, only function_ref/function pointers are possible tests
// for dynamic/parameterized fixtures use a table-driven test
	export struct test{
		constexpr void operator()()const{ theTest(); }
		constexpr std::string name()const{ return std::string{theName};}
		constexpr auto line() const { return definition.line();}
		constexpr auto file_name() const { return definition.file_name();}

		using test_function_ref = void(*)(); // because std::function_ref<void()> is not yet constexpr, due to reinterpret_cast...
		test_function_ref theTest;
		std::string_view theName; // OK, when populated by reflection at compile time. or from literal
		cute::source_location definition{std::source_location::current()};
	};

	export using suite=std::vector<test>; // legacy dynamic test suite support
	export template<typename s>
	concept aSuite=std::ranges::forward_range<s> && std::same_as<test const &,std::ranges::range_const_reference_t<s>>;

	static_assert(aSuite<suite>);

	export
	constexpr suite &operator+=(suite &left, aSuite auto const &right){
		left.insert_range(end(left),right);
		return left;
	}
	export
	constexpr
	suite &operator+=(suite &left, test right){
		left.push_back(std::move(right));
		return left;
	}
	export struct suite_test {
		suite s;
		constexpr void operator()(){
			for(auto t:s){
				t();
			}
		}
	};



	export template<std::meta::info func>
	requires isVoidFunc<func>
	constexpr test make_test
	{  [: func :],(has_identifier(func)?identifier_of(func):"-"),source_location_of(func) };


	template<std::meta::info theClass, std::meta::info theMemFunc>
	constexpr auto makeFun{
		 []{ typename[: theClass :]{}.[: theMemFunc :]();}
	};
	template <std::meta::info theClass, std::meta::info func>
	requires isVoidMemberFunction<func>
	constexpr auto make_fixture_test_name {[]{
		using std::string_view_literals::operator""sv;
		std::string result{};
		result += (has_identifier(theClass)?identifier_of(theClass):"-"sv);
		result +="::";
		result += has_identifier(func)?identifier_of(func):"-";
		return std::string_view(std::define_static_string(result));}()
	};

	export template <std::meta::info theClass, std::meta::info func>
	requires isVoidMemberFunction<func>
	constexpr test make_fixture_test
	{ makeFun<theClass,func>,make_fixture_test_name<theClass,func>,source_location_of(func)};



	consteval auto collectTestFunctions(std::meta::info ns){
		if (is_namespace(ns)){
			return define_static_array( members_of(ns,std::meta::access_context::unchecked())|std::views::filter(voidfunc)  );
		} else if (is_class_type(ns)){
			return define_static_array( members_of(ns,std::meta::access_context::unchecked()) | std::views::filter(voidMemfunc));
		} else {
			return define_static_array(std::vector<std::meta::info>{}); // empty array...
		}
	}

	consteval auto collectTestClasses(std::meta::info ns){
		if (is_namespace(ns)){
			return define_static_array( members_of(ns,std::meta::access_context::unchecked())|std::views::filter(std::meta::is_type)|std::views::filter(std::meta::is_class_type)  );
		} else {
			return define_static_array(std::vector<std::meta::info>{});
		}
	}

	consteval auto countTests(std::meta::info ns){
		auto sz = std::size(collectTestFunctions(ns));
	    for(auto clazz: collectTestClasses(ns)){
			sz += std::size(collectTestFunctions(clazz));
		}
		return sz;
	}


	export template<std::meta::info ns>
	requires (is_namespace(ns) || is_class_type(ns))
	constexpr auto collectTests{
		 [](){
			 constexpr auto sz = countTests(ns);
			 std::array<test, sz > l;

			 std::size_t index{};
			 template for (constexpr auto t:  collectTestFunctions(ns)){
				 if constexpr (is_namespace(ns))
				   l[index++] = make_test<t>;
				 else // is_class_type
				   l[index++] = make_fixture_test<ns,t>;
			 }
			 template for (constexpr auto c: collectTestClasses(ns)){
				 template for (constexpr auto t: collectTestFunctions(c)){
					 l[index++] = make_fixture_test<c,t>;
				 }
			 }
			 return l;}()
	};

	export template<std::meta::info ns>
	requires (is_namespace(ns) || is_class_type(ns))
	constexpr auto collectTestsWithName {
		 std::make_pair(collectTests<ns>,std::define_static_string(std::meta::identifier_of(ns)))
	};

export
struct null_listener{ // defines API/contract of runner parameter Listener
		constexpr void begin(aSuite auto const &, char const * /*info*/, std::size_t /*n_of_tests*/){}
		constexpr void end(aSuite auto const &, char const * /*info*/){}
		constexpr void start(test const &){}
		constexpr void success(test const &,char const * /*msg*/){}
		constexpr void failure(test const &,test_failure const &){}
		constexpr void error(test const &,char const * /*what*/){}
	};
	inline namespace runner_aux {
	export // only for testing purposes
	class ArgvTestFilter
	{
	    std::flat_set<std::string, std::less<>> match{}; // gcc 16.1 std::set not yet constexpr
	    constexpr bool shouldRunSuite(std::string_view const info, std::vector<std::string> const &args)
	    {
	        if(!args.size() || !info.size())
	            return true;
	        if(args.end() != std::ranges::find_if(args, [&info](auto const & s){
	        	return s.starts_with(info) && (size(s) == size(info) || s[size(info)]=='#'); })){
	               match.insert_range(args | std::views::transform( [&info](auto s){
	        	   if (size(s) > size(info) && (s.starts_with(info) && s[size(info)]=='#')){
	        		   s = s.substr(size(info)+1);
	        	   } else {
	        		   s.clear(); // no match
	        	   }
	        	   return s;
	           }) );
	           match.erase(std::string{}); // get rid of empty string
	           return true;
	        }
	        return false;
	    }
	public:
	    bool const shouldrunsuite;
		constexpr ArgvTestFilter(std::string_view const info, std::vector<std::string> const &args)
		:shouldrunsuite(shouldRunSuite(info,args)){}
	    constexpr bool shouldRun(std::string_view const name) const
	    {
	        return match.empty() || match.contains(name);
	    }
	};
	}
	export
	template <typename Listener=null_listener>
	struct runner{
		Listener& listener;
	    std::vector<std::string> const args{};
		constexpr runner(Listener &l, int const argc =0,  char const *const * const argv=nullptr )
	    :listener{l}
	    ,args{std::span<char const * const>{argv,argv+argc}
	                 | std::views::drop(1)
	                 | std::views::filter([](char const *s){ return s!= nullptr;})
	                 | std::ranges::to<std::vector<std::string> >()
	          }{}
		constexpr bool operator()(const test & t) const
	    {
	        return runit(t);
	    }
		template<std::ranges::forward_range suite>
		requires std::same_as<test const &,std::ranges::range_const_reference_t<suite>>
	    constexpr bool operator ()(suite const &s, const char *info = "") const
	    {
	    	ArgvTestFilter const filter{info,args};

	        bool result = true;
	        if(filter.shouldrunsuite){
	        	auto const numberOfTestsToRun = std::ranges::count_if(s,[&filter](test const &t){return filter.shouldRun(t.name());});
	            listener.begin(s, info, numberOfTestsToRun);
	            for(test const &t : s){
	                if (filter.shouldRun(t.name())) result = this->runit(t) && result;
	            }
	            listener.end(s, info);
	        }

	        return result;
	    }
		template<aSuite sut>
		constexpr bool operator()(std::pair<sut,char const *> const &suitewithname) const {
			return this->operator()(suitewithname.first,suitewithname.second);
		}
	private:


	   constexpr  bool runit(test const & t) const
	    {
	        try {
	            listener.start(t);
	            t();
	            listener.success(t, "OK");
	            return true;
	        } catch(cute::test_failure const & e){
	            listener.failure(t, e);
	        } catch(std::exception const & exc){
	            listener.error(t, /*demangle*/(exc.what())/*.c_str()*/); // TODO
	        } catch(std::string const & s){
	            listener.error(t, s.c_str());
	        } catch(char const * const&cs) {
				listener.error(t,cs);
			} catch(...) {
				listener.error(t,"unknown exception thrown");
			}
			return false;
		}
	};

	export
	template <typename Listener=null_listener>
	struct counting_listener:Listener{
		counting_listener() = default;

		counting_listener(Listener const &s)
		:Listener{s}
		{}
		constexpr void begin(aSuite auto const &s,char const *info, std::size_t n_of_tests){
			++numberOfSuites;
			numberOfTestsInSuites+=n_of_tests;
			Listener::begin(s,info, n_of_tests);
		}
		constexpr void end(aSuite auto const &s, char const * info){
			Listener::end(s,info);
		}

		constexpr void start(test const &t){
			++numberOfTests;
			Listener::start(t);
		}
		constexpr void success(test const &t,char const *msg){
			++successfulTests;
			Listener::success(t,msg);
		}
		constexpr void failure(test const &t,test_failure const &e){
			++failedTests;
			Listener::failure(t,e);
		}
		constexpr void error(test const &t,char const * const what){
			++errors;
			Listener::error(t,what);
		}
		std::size_t numberOfTests{0};
		std::size_t successfulTests{0};
		std::size_t failedTests{0};
		std::size_t errors{0};
		std::size_t numberOfSuites{0};
		std::size_t numberOfTestsInSuites{0};

	};

	export
	template <typename Listener=null_listener>
	struct ide_listener: Listener
	{
		ide_listener(std::ostream &os=std::cout):out(os) {}
		void begin(aSuite auto const &t,char const *info, std::size_t n_of_tests){
			std::println(out,"\n#beginning {} {}",info,n_of_tests);
			Listener::begin(t,info,n_of_tests);
		}
		void end(aSuite auto const &t, char const *info){
			std::println(out,"\n#ending {}", info);
			Listener::end(t,info);
		}
		void start(test const &t){
			std::println(out,"\n#starting {} {}",t.name(),t.definition);
			Listener::start(t);
		}
		void success(test const &t, char const *msg){
			std::println(out, "\n#success {} {}", t.name(), msg);
			Listener::success(t,msg);
		}
		void failure(test const &t,test_failure const &e){
			std::println(out, "\n#failure {} {}:{} {}",maskBlanks(t.name()), e.loc.file_name(),e.loc.line(), e.reason);
			Listener::failure(t,e);
		}
		void error(test const &t, char const *const what){
			std::println(out, "\n#error {} {}",maskBlanks(t.name()), what);
			Listener::error(t,what);
		}
		constexpr static std::string maskBlanks(std::string in) {
			std::ranges::replace(in,' ','_');
			return in;
		}
	protected:
		std::ostream &out;
	};

	auto basename(std::filesystem::path path){
		path = path.stem();
		path += ".xml";
		return path;
	}

	export struct xml_file_opener {
		std::filesystem::path filename;
		std::ofstream out;
		xml_file_opener(int argc, char const *const* const argv)
		:filename(argc>0&&argv[0]?basename(argv[0]):"testresult.xml")
		,out(filename.c_str()){}
	};

	export
	template <typename Listener=null_listener>
	class xml_listener:public Listener
	{
	protected:
		std::string mask_xml_chars(std::string in){
			std::string::size_type pos=0;
			while((pos=in.find_first_of("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x0b\x0c\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\"&'<>", pos, 34))!=std::string::npos){
				switch(in[pos]){
				case '&': in.replace(pos,1,"&amp;"); pos +=5; break;
				case '<': in.replace(pos,1,"&lt;"); pos += 4; break;
				case '>': in.replace(pos,1,"&gt;"); pos += 4; break;
				case '"': in.replace(pos,1,"&quot;"); pos+=6; break;
				case '\'':in.replace(pos,1,"&apos;"); pos+=6; break;
				default:
					char c = in[pos];
					std::string replacement = std::format("0x{:02X}" , static_cast<std::uint8_t>(c));
					in.replace(pos, 1, replacement); pos += replacement.size(); break;
					break;
				}
			}
			return in;
		}
		std::ostream &out;
		std::string current_suite;
	public:
		xml_listener(std::ostream &os):out(os) {
			std::println(out, "<testsuites>");
		}
		~xml_listener(){
			std::println(out, "</testsuites>");
		}

		void begin(aSuite auto const &t,char const *info, std::size_t n_of_tests){
			current_suite=mask_xml_chars(info);
			std::println(out,"\t" R"_(<testsuite name="{}" tests="{}">)_",current_suite,  n_of_tests);
			Listener::begin(t,info, n_of_tests);
		}
		void end(aSuite auto const &t, char const *info){
			std::println(out,  "\t</testsuite>");
			current_suite.clear();
			Listener::end(t,info);
		}
		void start(test const &t){
			std::print(out,"\t\t" R"_(<testcase classname="{}" name="{}")_", current_suite,mask_xml_chars(t.name()));

			Listener::start(t);
		}
		void success(test const &t, char const *msg){
			std::println(out, "/>");
			Listener::success(t,msg);
		}
		void failure(test const &t,test_failure const &e){
			std::println(out,  ">\n\t\t\t<failure message=\"{0}:{1} {2}\">\n{2}\n\t\t\t</failure>\n\t\t</testcase>",
					mask_xml_chars(e.loc.file_name()), e.loc.line(), mask_xml_chars(e.reason) );
			Listener::failure(t,e);

		}
		void error(test const &t, char const *what){
			std::println(out,  ">\n\t\t\t<error message=\"{0} {1}\" type=\"unexpected exception\">\n{1}\n\t\t\t</error>\n\t\t</testcase>",
					mask_xml_chars(t.name()),mask_xml_chars(what) );
			Listener::error(t,what);
		}
	};


	export
	template<typename Listener=null_listener>
	struct tap_listener : Listener {
		// a listener for the Test Anything Protocol
		// see https://perlmaven.com/tap-test-anything-protocol
		tap_listener(std::ostream & out = std::cout) : out(out),nofTests(0) { }

		~tap_listener() {
			if (nofTests) {
				std::println(out, "1..{}", nofTests);
			}
		}

		void begin(aSuite auto const & suite, char const * info, std::size_t n_of_tests) {
			std::println(out, "# Starting suite '{}' containing {} tests",info, n_of_tests );
			Listener::begin(suite, info, n_of_tests);
		}

		void end(aSuite auto const & suite, char const * info) {
			std::println(out, "# Ending suite '{}'", info);
			Listener::end(suite, info);
		}

		void start(test const & test) {
			++nofTests;
			Listener::start(test);
		}

		void success(test const & test, char const * msg) {
			std::println(out , "ok {} {}", nofTests, test.name() );
			Listener::success(test, msg);
		}

		void failure(test const & test, test_failure const & reason) {
			std::println(out, "not ok {} {}", nofTests , test.name() );
			std::println(out, "# Assertion failed: {}", reason.what());
			Listener::failure(test, reason);
		}

		void error(test const & test, char const * const what) {
			std::println(out, "not ok {} {}", nofTests , test.name() );
			std::println(out, "# Unexpected exception: {}", what);
			Listener::error(test, what);
		}

	private:
		std::ostream & out;
		std::size_t nofTests;
	};

} // namespace cute
// global namespace:

// namespace std specialization:
template <> struct std::formatter<cute::source_location> : std::formatter<std::string_view> {
	// could provide special syntax like chrono formatters: %l %c %i %u for line, column, file_name, function_name
	  template<typename ParseContext>
	  constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	  template <typename FormatContext>
	  auto format(const cute::source_location& loc, FormatContext& ctx) const
	      -> decltype(ctx.out()) {

	    return std::formatter<std::string_view>::format(
	    		std::format("{}:{}:{}:{}",loc.file_name(),loc.line(),loc.column(),loc.function_name()), ctx);
	  }
	};


namespace statictests {
constexpr void aTest(){
	cute::ASSERT(true);
}
constexpr void aComparingTest(){
	cute::ASSERT_EQUAL(1,1);
}
}
// the following static_assert uses a nasty trick to obtain a listener object reference to a temporary
static_assert(cute::runner{cute::null_listener{} = cute::null_listener{}}(cute::collectTestsWithName<^^statictests>));
