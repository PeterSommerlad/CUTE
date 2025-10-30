/*********************************************************************************
 * This file is the single header version of CUTE.
 *
 * Copyright (c) 2007-2025 Peter Sommerlad
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

#ifndef CUTE_SH_H_
#define CUTE_SH_H_

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#ifdef __GNUG__ // also for clang...
#include <cxxabi.h> // __cxa_demangle
#include <cstdlib> // ::free()
#endif

#include <string>
#ifndef DONT_USE_IOSTREAM
#include <iostream>
#include <fstream>
#include <sstream>
#endif
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <cstddef>
#include <utility>


// single header cute, assume at least C++17 for simplification


// TODO: need to consistently use C++17, and simplify code
// TODO: support move, forwarding and rvalue references




namespace cute {
namespace cute_to_string {
		static inline std::string backslashQuoteTabNewline(std::string const &input){
			std::string result;
			result.reserve(input.size());
			for (std::string::size_type i=0; i < input.length() ; ++i){
				switch(input[i]) {
					case '\n': result += "\\n"; break;
					case '\t': result += "\\t"; break;
					case '\\': result += "\\\\"; break;
					case '\r': result += "\\r"; break;
					default: result += input[i]; break;
				}
			}
			return result;
		}
		// common overloads of interface that work without an ostream
		static inline std::string to_string(char const *const &s){
			return s;
		}
		static inline std::string to_string(std::string const &s){
			return s;
		}
		template <typename T, typename= std::enable_if_t<std::is_unsigned_v<T>>>
		std::string hexit(T const &t){ // must be an unsigned type
			std::string hexed; // assume SSO, so reserve(2*sizeof(T)) is not needed
			if (t < 0x10) hexed+='0';
			if (t == 0) hexed+='0';
			for (T x=t;x>0;x /= 16){
				hexed += "0123456789ABCDEF"[x%16];
			}
			reverse(hexed.begin(),hexed.end());
			return hexed;
		}
	}
}

// needs adaptation for different compilers
// dependency to demangle is a given,
// otherwise we have to use macros everywhere
#ifdef __GNUG__ // also for clang...
namespace cute {
	extern inline std::string demangle(char const *name);

namespace cute_impl_demangle {
inline std::string plain_demangle(char const *name){
	if (!name) return "unknown";
	char const *toBeFreed = abi::__cxa_demangle(name,0,0,0);
	std::string result(toBeFreed?toBeFreed:name);
	::free(const_cast<char*>(toBeFreed));
	return result;
}
#if defined(_LIBCPP_ABI_NAMESPACE) || defined(_LIBCPP_NAMESPACE) || defined(_GLIBCXX_USE_CXX11_ABI)
inline void patch_library_namespace(std::string &mightcontaininlinenamespace) {
// libc++ uses inline namespace std::_LIBCPP_NAMESPACE:: for its classes. This breaks the tests relying on meta information. re-normalize the names back to std::
// libstdc++ (at least in version 6.3.1) puts some STL classes into the inline namespace std::_GLIBCXX_NAMESPACE_CXX11 if in C++11 mode
	std::string::size_type pos=std::string::npos;
#define XNS(X) #X
#define NS(X) XNS(X)
#if defined( _LIBCPP_NAMESPACE)
#define TOREPLACE "::" NS(_LIBCPP_NAMESPACE)
#elif defined(_LIBCPP_ABI_NAMESPACE)
#define TOREPLACE  "::" NS(_LIBCPP_ABI_NAMESPACE)
#else
#define TOREPLACE "::" NS(_GLIBCXX_NAMESPACE_CXX11)
#endif
	std::string const nothing;
	while (std::string::npos != (pos= mightcontaininlinenamespace.find(TOREPLACE)))
			mightcontaininlinenamespace.erase(pos,sizeof(TOREPLACE)-1);
#undef NS
#undef XNS
#undef TOREPLACE
}
inline void patchresultforstring(std::string& result) {
	static const std::string stringid=plain_demangle(typeid(std::string).name());
	std::string::size_type pos=std::string::npos;
	while(std::string::npos != (pos=result.find(stringid))){
		if (!result.compare(pos+stringid.size(),2," >",2)) result.erase(pos+stringid.size(),1); // makes templates look nice
		result.replace(pos,stringid.size(),"std::string");
	}
	patch_library_namespace(result);
}
#endif

}
inline std::string demangle(char const *name){
	if (!name) return "unknown";
	std::string result(cute_impl_demangle::plain_demangle(name));
#if defined(_LIBCPP_ABI_NAMESPACE) || defined(_LIBCPP_NAMESPACE) || defined(_GLIBCXX_USE_CXX11_ABI)
	cute_impl_demangle::patchresultforstring(result);
#endif
	return result;
}
}
#else
namespace cute {
#ifdef _MSC_VER
namespace cute_demangle_impl {

inline void removeMSKeyword(std::string &name,std::string const &kw){
	std::string::size_type pos=std::string::npos;
	while (std::string::npos != (pos= name.find(kw)))
			name.erase(pos,kw.size());

}
inline void patchresultforstring(std::string& result) {
	static const std::string stringid=(typeid(std::string).name());
	std::string::size_type pos=std::string::npos;
	while(std::string::npos != (pos=result.find(stringid))){
		if (!result.compare(pos+stringid.size(),2," >",2)) result.erase(pos+stringid.size(),1); // makes templates look nice
		result.replace(pos,stringid.size(),"std::string");
	}
}

inline void patchMSMangling(std::string &name){
	patchresultforstring(name);
	removeMSKeyword(name,"class ");
	removeMSKeyword(name,"struct ");
	for (std::string::iterator i=name.begin(); i != name.end(); ++i){
		if (*i==','){  i = name.insert(i+1,' ');}
	}
	std::string::size_type pos=0;
	while(std::string::npos !=(pos=name.find(" ,",pos))){
		name.erase(pos,1);
		++pos;
	}
}
}
inline std::string demangle(char const *name){
	std::string result(name?name:"unknown");
	cute_demangle_impl::patchMSMangling(result);
	return result;
}

#else
// this default works reasonably with MSVC71 and 8, hopefully for others as well
inline std::string demangle(char const *name){
	return std::string(name?name:"unknown");
}
#endif
}
#endif


namespace cute {
namespace cute_to_string {
namespace to_string_detail {
template <class CONT>
struct has_begin_end_const_member {
	template <typename T, T, T> struct type_check;
	template <typename C> static typename C::const_iterator test(
			type_check<typename C::const_iterator (C::*)()const noexcept,&C::begin, &C::end>*);
	template <typename C> static char test(...);
	enum e { value = (sizeof(char) != sizeof(test<CONT>(0)))
	};
}; // this doesn't work with VS library for set/map due to implementation in parent
   // no useful workaround possible
   //-> internal compiler errors for VS2010/12/12CTP Nov 12
} // detail
}
}

#ifndef DONT_USE_IOSTREAM
namespace cute {
namespace cute_to_string {
		template <typename T>
		std::ostream &to_stream(std::ostream &os,T const &t); // recursion needs forward
		inline std::ostream &to_stream(std::ostream &os,std::byte t); // recursion needs forward

		// the following code was stolen and adapted from Boost Exception library.
		// it avoids compile errors, if a type used with ASSERT_EQUALS doesn't provide an output shift operator
		namespace to_string_detail {
			template <class T,class CharT,class Traits>
			char operator<<( std::basic_ostream<CharT,Traits> &, T const & );
			template <class T,class CharT,class Traits>
			struct is_output_streamable_impl {
				static std::basic_ostream<CharT,Traits> & f();
				static T const & g();
				enum e { value = (sizeof(char) != sizeof(f()<<g())) }; // assumes sizeof(char)!=sizeof(ostream&)
			};
			// specialization for pointer types to map char * to operator<<(std::ostream&,char const *)
			template <class T,class CharT,class Traits>
			struct is_output_streamable_impl<T*,CharT,Traits> {
				static std::basic_ostream<CharT,Traits> & f();
				static T const * g();
				enum e { value = (sizeof(char) != sizeof(f()<<g())) }; // assumes sizeof(char)!=sizeof(ostream&)
			};
		}
		template <class T, class CharT=char, class Traits=std::char_traits<CharT> >
		struct is_output_streamable {
			enum e { value=to_string_detail::is_output_streamable_impl<T,CharT,Traits>::value };
		};
		// detect standard container conforming begin() end() iterator accessors.
		// might employ begin/end traits from c++0x for loop in the future. --> select_container
		template <typename T>
		struct printItWithDelimiter
		{
			std::ostream &os;
			bool first; // allow use of for_each algorithm
			printItWithDelimiter(std::ostream &os):os(os),first(true){}
			void operator()(T const &t){
				if (!first) os<<',';
				else first=false;
				os << '\n'; // use newlines so that CUTE's plug-in result viewer gives nice diffs
				cute_to_string::to_stream(os,t);
			}
		};
		//try print_pair with specialization of template function instead:
		// the generic version prints about missing operator<< that is the last resort
		template <typename T>
		std::ostream &print_pair(std::ostream &os,T const &){
			return os << "no operator<<(ostream&, " <<cute::demangle(typeid(T).name())<<')';
		}
		//the std::pair overload is useful for std::map etc. however,
		template <typename K, typename V>
		std::ostream &print_pair(std::ostream &os,std::pair<K,V> const &p){
			os << '[' ;
			cute_to_string::to_stream(os,p.first);
			os << " -> ";
			cute_to_string::to_stream(os,p.second);
			os << ']';
			return os;
		}
		// overload for Arrays
		template <typename T, size_t N>
		std::ostream &print_pair(std::ostream &os,T const (&t)[N]){
			printItWithDelimiter<T> printer(os);
			os << cute::demangle(typeid(T).name()) <<'['<<N<<']'<< '{';
			std::for_each(t,t+N,printer);
			return os << '}';
		}

		template<std::size_t Value>
		using size = std::integral_constant<std::size_t, Value>;
		struct empty { template<typename ...Types>empty(Types const &...){} };
		template<typename ...Types, std::size_t _, std::size_t Head, std::size_t ...Indices>
		std::ostream &print_tuple(std::ostream &os, std::tuple<Types...> const &t, size<_> const, std::index_sequence<Head, Indices...>){
			empty{os << '\n'
			      ,cute_to_string::to_stream(os, std::get<Head>(t))
			      ,(os << ",\n", cute_to_string::to_stream(os, std::get<Indices>(t)))...};
			return os;
		}
		template<typename ...Types, std::size_t _>
		std::ostream &print_tuple(std::ostream &os, std::tuple<Types...> const &t, size<_> const s) {
			return print_tuple(os, t, s, std::index_sequence_for<Types...>{});
		}
		template<typename ..._>
		std::ostream &print_tuple(std::ostream &os, std::tuple<_...> const &t, size<1> const){
			return os << '\n', cute_to_string::to_stream(os, std::get<0>(t));
		}
		template<typename ..._>
		std::ostream &print_tuple(std::ostream &os, std::tuple<_...> const &, size<0>){
			return os;
		}
		// overload for std::tuple<...>
		template<typename ...Types>
		std::ostream &print_pair(std::ostream &os, std::tuple<Types...> const &t){
			os << cute::demangle(typeid(decltype(t)).name()) << '{';
			auto olflags = os.flags();
			os << std::boolalpha;
			print_tuple(os, t, size<sizeof...(Types)>{});
			os.flags(olflags);
			os << '}';
			return os;
		}
		template <typename T, bool select>
		struct select_container {
			std::ostream &os;
			select_container(std::ostream &os):os(os){}
			std::ostream& operator()(T const &t){
				printItWithDelimiter<typename T::value_type> printer(os);
				os << cute::demangle(typeid(T).name()) << '{';
				std::for_each(t.begin(),t.end(),printer);
				return os << '}';
			}
		};

		template <typename T>
		struct select_container<T,false> {
			std::ostream &os;
			select_container(std::ostream &os):os(os){}
			std::ostream & operator()(T const &t){
				//  look for std::pair. a future with tuple might be useful as well, but not now.
				return print_pair(os,t); // here a simple template function overload works.
			}
		};

		template <typename T, bool select>
		struct select_built_in_shift_if {
			std::ostream &os;
			select_built_in_shift_if(std::ostream &ros):os(ros){}
			std::ostream& operator()(T const &t){
				return os << t ; // default uses operator<<(std::ostream&,T const&) if available
			}
		};

		template <typename T>
		struct select_built_in_shift_if<T,false> {
			std::ostream &os;
			select_built_in_shift_if(std::ostream &ros):os(ros){}
			std::ostream & operator()(T const &t){
				// if no operator<< is found, try if it is a container or std::pair
				return select_container<T,bool(to_string_detail::has_begin_end_const_member<T>::value) >(os)(t);
			}
		};
		template <typename T>
		std::ostream &to_stream(std::ostream &os,T const &t){
			select_built_in_shift_if<T,cute_to_string::is_output_streamable<T>::value > out(os);
			return out(t);
		}
		inline std::ostream &to_stream(std::ostream &os,std::byte b){
			return os << "0x" << hexit(static_cast<std::underlying_type_t<std::byte>>(b));
		}
#ifdef _MSC_VER
		// special overloads because VC can not detect begin/end
		inline std::ostream& to_stream(std::ostream &os,std::string const &s){
			return os<<s;
		} // needed to compensate for following overload, hope nothing else matches
		template <template<typename,typename,typename> class S,
		          typename K, typename CMP, typename ALLOC>
		std::ostream &to_stream(std::ostream &os,S<K,CMP,ALLOC> const &t){
			printItWithDelimiter<typename S<K,CMP,ALLOC>::value_type> printer(os);
			os << cute::demangle(typeid(S<K,CMP,ALLOC>).name()) << '{';
			std::for_each(t.begin(),t.end(),printer);
			return os << '}';
		}
		template <template<typename,typename,typename,typename> class M,typename K, typename V, typename CMP, typename ALLOC>
		std::ostream &to_stream(std::ostream &os,M<K,V,CMP,ALLOC> const &t){
			printItWithDelimiter<typename M<K,V,CMP,ALLOC>::value_type> printer(os);
			os << cute::demangle(typeid(M<K,V,CMP,ALLOC>).name()) << '{';
			std::for_each(t.begin(),t.end(),printer);
			return os << '}';
		}
#endif

		// this is the interface:
		template <typename T>
		std::string to_string(T const &t) {
			std::ostringstream os;
			to_stream(os,t);
			return os.str();
		}
	}
}
#else /* for embedded without iostreams */
// traits
namespace cute{
namespace cute_to_string {
		template <typename T>
		void adjust_long(T const &,std::string &to_adjust){ // assumes T is an integral type
			if (sizeof(T) <= sizeof(int)) return; // don't mark int sized integrals with L
			if (sizeof(T)>=sizeof(long)) to_adjust+='L';
			if (sizeof(T)> sizeof(long)) to_adjust+='L'; // if there is support for (unsigned) long long
		}
		template <typename T>
		std::string to_string_embedded_int_signed(T const &t, std::true_type ){
			std::string convert; // t is an integral value
			T x=t;
			bool negative=t<0;
			bool minint=false;
			if (x == std::numeric_limits<T>::min()){ // can not easily convert it, assuming 2s complement
				minint=true;
				x++; // add 1, but 1 might be incompatible type, so use ++
			}
			if (x < 0) x = static_cast<T>(-x);
			if (x == 0) convert += '0';
			while (x > 0) {
				convert += "0123456789"[x%10];
				x /= 10;
			}
			if (minint) ++ convert[0]; // adjust last digit
			if (negative) convert += '-';
			reverse(convert.begin(),convert.end());
			cute::cute_to_string::adjust_long(t,convert);
			return convert;
		}
		template <typename T>
		std::string to_string_embedded_int_signed(T const &t, std::false_type ){
			// manual hex conversion to avoid ostream dependency for unsigned values
			std::string hexed="0x"+cute::cute_to_string::hexit(t);
			cute::cute_to_string::adjust_long(t,hexed);
			return hexed;
		}
		template <typename T>
		std::string to_string_embedded_int(T const &t, std::true_type ){
			return to_string_embedded_int_signed(t,std::is_signed<T>());
		}
		template <typename T>
		std::string to_string_embedded_int(T const &, std::false_type ){
			return "no to_string";
		}
		// convenience for pointers.... useful?
		template <typename T>
		std::string to_string(T * const&t) {
			std::string result;
			result = cute::cute_to_string::hexit(reinterpret_cast<std::uintptr_t>(t));
			result.insert(0u,sizeof(T*)*2-result.size(),'0');
			result.insert(0,1,'p');
			return result;
		}

		// this is the interface:
		template <typename T>
		std::string to_string(T const &t) {
			return to_string_embedded_int(t, std::is_integral<T>());
		}
		// overloads for special cases
		inline std::string to_string(std::byte b){
			return cute::cute_to_string::to_string(static_cast<std::underlying_type_t<std::byte>>(b));
		}
		template<size_t N>
		std::string to_string(char const (&t)[N]){
			return t;
		}
		template<typename T,size_t N>
		std::string to_string(T const (&t)[N]){
			std::string result{demangle(typeid(T).name())};
			result+="[";
			result+=cute::cute_to_string::to_string(int(N));
			result+="]{\n";
			for(size_t i=0; i < N ;){
				result += cute::cute_to_string::to_string(t[i]);
				if (++i < N) result += ",\n";
			}
			result += "}";
			return result;
		}
	}
}
#endif

/// end of to_string infrastructure

namespace cute {
	struct test_failure {
		std::string reason;
		std::string filename;
		int lineno;

		test_failure(std::string const &r,char const *f, int line)
		:reason(r),filename(f),lineno(line)
		{ 	}
		char const * what() const { return reason.c_str(); }
	};
}

#if !defined(_MSC_VER)
#define CUTE_FUNCNAME_PREFIX std::string(__func__)+": "
#else
#if defined( _MSC_VER ) || defined(__GNUG__)
// use -D CUTE_FUNCNAME_PREFIX if you want to use non-local lambdas with test macros
#if !defined(CUTE_FUNCNAME_PREFIX)
#define CUTE_FUNCNAME_PREFIX std::string(__FUNCTION__)+": "
#endif
#else // could provide #defines for other compiler-specific extensions... need to experiment, i.e., MS uses __FUNCTION__
#define CUTE_FUNCNAME_PREFIX std::string("")
#endif
#endif
#define ASSERTM(msg,cond) do { if (!(cond)) throw cute::test_failure(CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg),__FILE__,__LINE__);} while(false)
#define ASSERT(cond) ASSERTM(#cond,cond)
#define FAIL() ASSERTM("FAIL()",false)
#define FAILM(msg) ASSERTM(msg,false)

namespace cute {
	// you could provide your own overload for diff_values for your app-specific types
	// be sure to use tabs as given below, then the CUTE eclipse plug-in will parse correctly
	template <typename ExpectedValue, typename ActualValue>
	std::string diff_values(ExpectedValue const &expected
						, ActualValue const & actual
						, char const *left="expected"
						, char const *right="but was"){
		// construct a simple message...to be parsed by IDE support
		std::string res;
		res += ' ';
		res += left;
		res+=":\t" + cute_to_string::backslashQuoteTabNewline(cute_to_string::to_string(expected))+'\t';
		res += right;
		res +=":\t"+cute_to_string::backslashQuoteTabNewline(cute_to_string::to_string(actual))+'\t';
		return res;
	}
}



namespace cute {


	namespace cute_do_equals {
		// provide some template meta programming tricks to select "correct" comparison for floating point and integer types
		template <typename ExpectedValue, typename ActualValue, typename DeltaValue>
		bool do_equals_floating_with_delta(ExpectedValue const &expected
				,ActualValue const &actual
				,DeltaValue const &delta) {
			using std::abs; // allow for user-defined types with abs overload
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
			return bool(abs(delta) >= abs(expected-actual)); // Accommodate non-standard boolean type with explicit conversion
#pragma GCC diagnostic pop
		}
		template <typename ExpectedValue, typename ActualValue, bool select_non_floating_point_type>
		bool do_equals_floating(ExpectedValue const &expected
					,ActualValue const &actual,const std::integral_constant<bool, select_non_floating_point_type>&){
			return bool(expected==actual); // normal case for most types uses operator==!, accommodate non bool operator ==
		}
		template <typename ExpectedValue, typename ActualValue>
		bool do_equals_floating(ExpectedValue const &expected
					,ActualValue const &actual,const std::true_type&){
			const ExpectedValue automatic_delta_masking_last_significant_digit=(10*std::numeric_limits<ExpectedValue>::epsilon())*expected;
			return do_equals_floating_with_delta(expected,actual,automatic_delta_masking_last_significant_digit);
		}
		// TMP-overload dispatch for floating points 2 bool params --> 4 overloads
		template <typename ExpectedValue, typename ActualValue, bool select_non_integral_type>
		bool do_equals(ExpectedValue const &expected
					,ActualValue const &actual
					,const std::integral_constant<bool, select_non_integral_type>&/*exp_is_integral*/
					,const std::integral_constant<bool, select_non_integral_type>&/*act_is_integral*/){
			return do_equals_floating(expected,actual,std::is_floating_point<ExpectedValue>());
		}
		template <typename ExpectedValue, typename ActualValue, bool select_non_integral_type>
		bool do_equals(ExpectedValue const &expected
					,ActualValue const &actual
					,const std::false_type&,const std::false_type&){
			return do_equals_floating(expected,actual,std::is_floating_point<ActualValue>());
		}
		template <typename ExpectedValue, typename ActualValue, bool select_non_integral_type>
		bool do_equals(ExpectedValue const &expected
					,ActualValue const &actual
					,const std::integral_constant<bool, select_non_integral_type>&/*exp_is_integral*/
					,const std::true_type&){
			return do_equals_floating(expected,actual,std::is_floating_point<ExpectedValue>());
		}
		template <typename ExpectedValue, typename ActualValue, bool select_non_integral_type>
		bool do_equals(ExpectedValue const &expected
					,ActualValue const &actual
					,const std::true_type&,const std::integral_constant<bool, select_non_integral_type>&/*act_is_integral*/){
			return do_equals_floating(expected,actual,std::is_floating_point<ActualValue>());
		}
		template <bool select_non_floating_point_type, typename ...ExpectedTypes, typename ...ActualTypes>
		bool do_equals(std::tuple<ExpectedTypes...> const &expected
					,std::tuple<ActualTypes...> const &actual,const std::integral_constant<bool, select_non_floating_point_type>&){
					return expected==actual;
		}
		// can I get rid of the following complexity by doing a do_equals_integral
		// parameterized by is_signed<ExpectedValue>==is_signed<ActualValue> or nofBits<A> < nofBits<B>


		// this is an optimization for avoiding if and sign-extend overhead if both int types are the same as below
		template <typename IntType>
		bool do_equals(IntType const &expected
					,IntType const &actual
					,const std::true_type&,const std::true_type&){
			return expected==actual;
		}
		// bool cannot be made signed, therefore we need the following three overloads, also to avoid ambiguity
		template <typename IntType>
		bool do_equals(bool const &expected
					,IntType const &actual
					,const std::true_type&,const std::true_type&){
			return expected== !(!actual); // warning from VS
		}
		template <typename IntType>
		bool do_equals(IntType const &expected
					,bool const &actual
					,const std::true_type&,const std::true_type&){
			return !(!expected)==actual; // warning from VS
		}
		// do not forget the inline on a non-template overload!
		// this overload is needed to actually avoid ambiguity for comparing bool==bool as a best match
		inline bool do_equals(bool const &expected
				      ,bool const &actual
				      , const std::true_type&,const std::true_type&){
			return expected==actual;
		}
		// overload for char const *, my test case failed because VC++ doesn't use string constant folding like g++/clang
		// a feature where we should do string comparison
		inline bool do_equals(char const *const &expected
				      ,char const *const &actual
				      , const std::false_type&,const std::false_type&){
			return std::string(expected) == actual;
		}
		template <typename IntegralType>
		size_t nof_bits(IntegralType const &){
			return std::numeric_limits<IntegralType>::digits;
		}
		template <typename ExpectedValue, typename ActualValue>
		bool do_equals_integral(ExpectedValue const &expected
				,ActualValue const &actual
				,const std::true_type&,const std::true_type&){
			if (nof_bits(expected) < nof_bits(actual))
						return static_cast<ActualValue>(expected) == actual;
			else
						return expected == static_cast<ExpectedValue>(actual);
			return false;
		}
		template <typename ExpectedValue, typename ActualValue>
		bool do_equals_integral(ExpectedValue const &expected
				,ActualValue const &actual
				,const std::false_type&,const std::true_type&){
//TODO complicated case, one signed one unsigned type. since it is about equality casting to the longer should work?
			if (sizeof(ExpectedValue) >	sizeof(ActualValue))
				return expected==static_cast<ExpectedValue>(actual);
			else
				return static_cast<ActualValue>(expected) == actual;
		}
		template <typename ExpectedValue, typename ActualValue>
		bool do_equals_integral(ExpectedValue const &expected
				,ActualValue const &actual
				,const std::true_type&,const std::false_type&){
//TODO
			if (sizeof(ExpectedValue) < sizeof(ActualValue))
				return static_cast<ActualValue>(expected)==	actual;
			else
				return expected == static_cast<ExpectedValue>(actual);
		}
		template <typename ExpectedValue, typename ActualValue>
		bool do_equals_integral(ExpectedValue const &expected
				,ActualValue const &actual
				,const std::false_type&,const std::false_type&){
			if (nof_bits(expected) < nof_bits(actual))
						return static_cast<ActualValue>(expected) == actual;
			else
						return expected == static_cast<ExpectedValue>(actual);
			return false;
		}
		// will not work if either type is bool, therefore the overloads above.
		template <typename ExpectedValue, typename ActualValue>
		bool do_equals(ExpectedValue const &expected
					,ActualValue const &actual
					,const std::true_type&,const std::true_type&){
			return do_equals_integral(expected,actual,
					std::is_signed<ExpectedValue>()
					,std::is_signed<ActualValue>());
		}
	} // namespace equals_impl
	template <typename ExpectedValue, typename ActualValue>
	void assert_equal(ExpectedValue const &expected
				,ActualValue const &actual
				,std::string const &msg
				,char const *file
				,int line) {
		// unfortunately there is no is_integral_but_not_bool_or_enum
		typedef typename std::is_integral<ExpectedValue> exp_integral;
		typedef typename std::is_integral<ActualValue> act_integral;
		if (cute_do_equals::do_equals(expected,actual,exp_integral(),act_integral()))
			return;
		throw test_failure(msg + diff_values(expected,actual),file,line);
	}

	template <typename ExpectedValue, typename ActualValue, typename DeltaValue>
	void assert_equal_delta(ExpectedValue const &expected
				,ActualValue const &actual
				,DeltaValue const &delta
				,std::string const &msg
				,char const *file
				,int line) {
		if (cute_do_equals::do_equals_floating_with_delta(expected,actual,delta)) return;
		throw test_failure(msg + diff_values(expected,actual),file,line);
	}

}

#define ASSERT_EQUALM(msg,expected,actual) cute::assert_equal((expected),(actual),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg),__FILE__,__LINE__)
#define ASSERT_EQUAL(expected,actual) ASSERT_EQUALM(#expected " == " #actual, (expected),(actual))
#define ASSERT_EQUAL_DELTAM(msg,expected,actual,delta) cute::assert_equal_delta((expected),(actual),(delta),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg),__FILE__,__LINE__)
#define ASSERT_EQUAL_DELTA(expected,actual,delta) ASSERT_EQUAL_DELTAM(#expected " == " #actual " with error " #delta  ,(expected),(actual),(delta))

//DEPRECATE(ASSERT_EQUAL_RANGES_M, ASSERT_EQUAL_RANGESM)
#define ASSERT_EQUAL_RANGES_M(msg,expbeg,expend,actbeg,actend) DEPRECATED(ASSERT_EQUAL_RANGES_M), ASSERT_EQUALM(msg,cute::make_range(expbeg,expend),cute::make_range(actbeg,actend))
#define ASSERT_EQUAL_RANGESM(msg,expbeg,expend,actbeg,actend) ASSERT_EQUALM(msg,cute::make_range(expbeg,expend),cute::make_range(actbeg,actend))
#define ASSERT_EQUAL_RANGES(expbeg,expend,actbeg,actend) ASSERT_EQUAL_RANGESM("range{" #expbeg "," #expend "} == range{" #actbeg "," #actend "}",expbeg,expend,actbeg,actend)
//#endif /*CUTE_EQUALS_H_*/



namespace cute {

namespace cute_relops_detail{
	template <typename LeftValue, typename RightValue>
	std::string compare_values(LeftValue const &left
						, RightValue const & right){
		return cute::diff_values(left,right,"left","right");
	}
}
template <template<typename> class RELOP,typename TL, typename TR>
void assert_relop(TL const &left
		, TR const &right
		,std::string const &msg
		,char const *file
		,int line) {
	if (RELOP<TL>()(left,right)) return;
	throw test_failure(msg + cute_relops_detail::compare_values(left,right),file,line);
}

}
#define ASSERT_LESSM(msg,left,right) cute::assert_relop<std::less>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg),__FILE__,__LINE__)
#define ASSERT_LESS(left,right) ASSERT_LESSM(#left " < " #right, (left),(right))
#define ASSERT_LESS_EQUALM(msg,left,right) cute::assert_relop<std::less_equal>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg),__FILE__,__LINE__)
#define ASSERT_LESS_EQUAL(left,right) ASSERT_LESS_EQUALM(#left " <= " #right, (left),(right))
#define ASSERT_GREATERM(msg,left,right) cute::assert_relop<std::greater>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg),__FILE__,__LINE__)
#define ASSERT_GREATER(left,right) ASSERT_GREATERM(#left " > " #right, (left),(right))
#define ASSERT_GREATER_EQUALM(msg,left,right) cute::assert_relop<std::greater_equal>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg),__FILE__,__LINE__)
#define ASSERT_GREATER_EQUAL(left,right) ASSERT_GREATER_EQUALM(#left " >= " #right, (left),(right))
#define ASSERT_NOT_EQUAL_TOM(msg,left,right) cute::assert_relop<std::not_equal_to>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg),__FILE__,__LINE__)
#define ASSERT_NOT_EQUAL_TO(left,right) ASSERT_NOT_EQUAL_TOM(#left " != " #right, (left),(right))


#define DDTM(msg) (cute::test_failure(cute::cute_to_string::backslashQuoteTabNewline(msg),__FILE__,__LINE__))
#define DDT() DDTM("")

#define ASSERT_DDTM(cond,msg,failure) do{ \
	if (!(cond)) \
		throw cute::test_failure(CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg)+(failure).reason,\
				(failure).filename.c_str(),(failure).lineno);\
} while (false)
#define ASSERT_DDT(cond,failure) ASSERT_DDTM(cond,#cond,(failure))

#define ASSERT_EQUAL_DDTM(msg,expected,actual,failure) cute::assert_equal((expected),(actual),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg)\
		+(failure).reason,(failure).filename.c_str(),(failure).lineno)
#define ASSERT_EQUAL_DDT(expected,actual,failure) ASSERT_EQUAL_DDTM(#expected " == " #actual, (expected),(actual),(failure))
#define ASSERT_EQUAL_DELTA_DDTM(msg,expected,actual,delta,failure) cute::assert_equal_delta((expected),(actual),(delta),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg)\
		+(failure).reason,(failure).filename.c_str(),(failure).lineno)
#define ASSERT_EQUAL_DELTA_DDT(expected,actual,delta,failure) ASSERT_EQUAL_DELTA_DDTM(#expected " == " #actual " with error " #delta  ,(expected),(actual),(delta),(failure))

#define ASSERT_LESS_DDTM(msg,left,right,failure) cute::assert_relop<std::less>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg)\
		+(failure).reason,(failure).filename.c_str(),(failure).lineno)
#define ASSERT_LESS_DDT(left,right,failure) ASSERT_LESS_DDTM(#left " < " #right, (left),(right),(failure))

#define ASSERT_LESS_EQUAL_DDTM(msg,left,right,failure) cute::assert_relop<std::less_equal>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg)\
		+(failure).reason,(failure).filename.c_str(),(failure).lineno)
#define ASSERT_LESS_EQUAL_DDT(left,right,failure) ASSERT_LESS_EQUAL_DDTM(#left " <= " #right, (left),(right),(failure))

#define ASSERT_GREATER_DDTM(msg,left,right,failure) cute::assert_relop<std::greater>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg)\
		+(failure).reason,(failure).filename.c_str(),(failure).lineno)
#define ASSERT_GREATER_DDT(left,right,failure) ASSERT_GREATER_DDTM(#left " > " #right, (left),(right),(failure))

#define ASSERT_GREATER_EQUAL_DDTM(msg,left,right,failure) cute::assert_relop<std::greater_equal>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg)\
		+(failure).reason,(failure).filename.c_str(),(failure).lineno)
#define ASSERT_GREATER_EQUAL_DDT(left,right,failure) ASSERT_GREATER_EQUAL_DDTM(#left " >= " #right, (left),(right),(failure))

#define ASSERT_NOT_EQUAL_TO_DDTM(msg,left,right,failure) cute::assert_relop<std::not_equal_to>((left),(right),\
		CUTE_FUNCNAME_PREFIX+cute::cute_to_string::backslashQuoteTabNewline(msg)\
		+(failure).reason,(failure).filename.c_str(),(failure).lineno)
#define ASSERT_NOT_EQUAL_TO_DDT(left,right,failure) ASSERT_NOT_EQUAL_TO_DDTM(#left " != " #right, (left),(right),(failure))




// should we allow arbitrary code and remove the parentheses around the macro expansion?
// not now, strange compilation side-effects might result.
namespace cute {
	namespace do_not_use_this_namespace {
		struct assert_throws_failure_exception {
			struct cute::test_failure original;
			assert_throws_failure_exception(std::string const &r,char const *f, int line):
					original(r,f, line){}
		};
	}
}

#define ASSERT_THROWSM(anuncommonmessagetextparametername,code,exc) \
	do { \
		try { \
			{ code ; } \
			throw cute::do_not_use_this_namespace::assert_throws_failure_exception((anuncommonmessagetextparametername),__FILE__,__LINE__); \
		} catch(exc const &){ \
		} catch(cute::do_not_use_this_namespace::assert_throws_failure_exception const &atf){throw atf.original;} \
	} while(0)
#define ASSERT_THROWS(code,exc) ASSERT_THROWSM(" expecting " #code " to throw " #exc,code,exc)



// make plain functions as tests more 'cute':
namespace cute {

	struct test{
		void operator()()const{ theTest(); }
		std::string name()const{ return name_;}


		// (real) functor types can (almost) spell their name
		// but a name can also be given explicitely, e.g. for CUTE() macro
		// for simple test functions
		template <typename VoidFunctor>
		test(VoidFunctor const &t, std::string sname = demangle(typeid(VoidFunctor).name()))
		:name_{std::move(sname)},theTest(t){}
		// separate overload to allow nicer C++11 initializers with {"name",lambda}
		template <typename VoidFunctor>
		test(std::string sname,VoidFunctor const &t)
		:name_{std::move(sname)},theTest{t}{}

	private:
		std::string name_;
		std::function<void()> theTest;
	};

}

#define CUTE(name) cute::test((&name),(#name))


//#define CUTE_SUITE_H_
namespace cute {
	typedef std::vector<test> suite;
	// convenience operator for appending to suites, might not be right
	// deprecated, not supported by plug-in, not needed with Eclipse plug-in
	inline
	suite &operator+=(suite &left, suite const &right){
		left.insert(left.end(),right.begin(),right.end());
		return left;
	}
	inline
	suite &operator+=(suite &left, test const &right){
		left.push_back(right);
		return left;
	}
}


namespace cute{
	struct repeated_test {
		repeated_test(test const &t,unsigned int n):theTest(t),repetitions(n){}
		void operator()(){
			for (unsigned int i=0;i<repetitions;++i){
				theTest();
			}
		}
		test theTest;
		const unsigned int repetitions;
	};
}

#define CUTE_REPEAT(aTestFunction,n) cute::test(cute::repeated_test((aTestFunction),(n)),#aTestFunction " " #n " times repeated")
#define CUTE_REPEAT_TEST(aTestObject,n) cute::test(cute::repeated_test((aTestObject),(n)),aTestObject.name()+" " #n " times repeated")


namespace cute{
	// make a whole suite a test, failure stops the suite's execution
	struct suite_test {
		suite theSuite;
		suite_test(suite s):theSuite{std::move(s)}{}
		void operator()(){
			std::for_each(theSuite.begin(),theSuite.end(),[](auto &test){ test();});
		}
	};
}
#define CUTE_SUITE_TEST(s) cute::test(cute::suite_test((s)),#s)
#define CUTE_SUITE_TEST_NAME(s, name) cute::test(cute::suite_test((s)),name)
#define CUTE_SUITE_TEST_H_





// idea blatantly stolen from Aeryn
namespace cute {
	template <typename TestFunctor>
	struct test_incarnate {
		void operator()(){
			TestFunctor()();
		}
	};
	// TODO: check if there are problems with references.
	template <typename TestFunctor, typename ContextObject>
	struct test_incarnate_with_context {
		test_incarnate_with_context(ContextObject context):theContext(context)
		{}
		test_incarnate_with_context(test_incarnate_with_context const &other):theContext(other.theContext){} // provide copy-ctor in case compiler will define it deleted
		void operator()(){
			TestFunctor t(theContext);// wouldn't create temporary to call with ()()
			t();
		}
		ContextObject theContext;
	};
	template <typename TestFunctor,typename ContextObject>
	test make_incarnate_with_context(ContextObject obj){
		return test(test_incarnate_with_context<TestFunctor,ContextObject>(obj),demangle(typeid(TestFunctor).name()));
	}
}

#define CUTE_INCARNATE(TestFunctor) cute::test(cute::test_incarnate<TestFunctor>(),cute::demangle(typeid(TestFunctor).name()))
#define CUTE_INCARNATE_WITH_CONTEXT(TestFunctor,contextObject) cute::make_incarnate_with_context<TestFunctor>(contextObject)



namespace cute {
// TODO: make this work form ref-qualified member functions and perfect forwarding, caution about dangling
	template <typename TestClass>
	test makeMemberFunctionTest(TestClass &t,void (TestClass::*fun)(),char const *name){
		return test{[tptr=&t,fun]{std::invoke(fun,tptr);},demangle(typeid(TestClass).name())+"::"+name};
		//return test(std::bind(fun,std::ref(t)),demangle(typeid(TestClass).name())+"::"+name);
	}
	template <typename TestClass>
	test makeMemberFunctionTest(TestClass const &t,void (TestClass::*fun)()const,char const *name){
		return test{[tptr=&t,fun]{std::invoke(fun,tptr);},demangle(typeid(TestClass).name())+"::"+name};
//		return test(std::bind(fun,std::cref(t)),demangle(typeid(TestClass).name())+"::"+name);
	}
	template <typename TestClass,typename MemFun>
	struct incarnate_for_member_function {
		MemFun memfun;
		incarnate_for_member_function(MemFun f):memfun(f){}
		void operator()(){
			TestClass t;
			(t.*memfun)();
		}
	};
	template <typename TestClass, typename MemFun>
	test makeSimpleMemberFunctionTest(MemFun fun,char const *name){
		return test(incarnate_for_member_function<TestClass,MemFun>(fun),demangle(typeid(TestClass).name())+"::"+name);
	}
	template <typename TestClass,typename MemFun, typename Context>
	struct incarnate_for_member_function_with_context_object {
		MemFun memfun;
		Context context;
		incarnate_for_member_function_with_context_object(MemFun f,Context c)
		:memfun(f),context(c){}
		incarnate_for_member_function_with_context_object(incarnate_for_member_function_with_context_object const &other)
		:memfun(other.memfun),context(other.context){} // provide copy-ctor for std::function ctor requirement should be =default on C++11

		void operator()(){
			TestClass t(context);
			(t.*memfun)();
		}
	};
	template <typename TestClass, typename MemFun, typename Context>
	test makeMemberFunctionTestWithContext(Context c,MemFun fun,char const *name){
		return test(incarnate_for_member_function_with_context_object<TestClass,MemFun,Context>(fun,c),demangle(typeid(TestClass).name())+"::"+name);
	}
}

#define CUTE_MEMFUN(testobject,TestClass,MemberFunctionName) \
	cute::makeMemberFunctionTest(testobject,\
		&TestClass::MemberFunctionName,\
		#MemberFunctionName)
#define CUTE_SMEMFUN(TestClass,MemberFunctionName) \
	cute::makeSimpleMemberFunctionTest<TestClass>(\
		&TestClass::MemberFunctionName,\
		#MemberFunctionName)
#define CUTE_CONTEXT_MEMFUN(context_object,TestClass,MemberFunctionName) \
	cute::makeMemberFunctionTestWithContext<TestClass>(\
		context_object,\
		&TestClass::MemberFunctionName,\
		#MemberFunctionName)

#define CUTE_LIB_VERSION "2.2.6"


namespace cute {
	struct null_listener{ // defines Contract of runner parameter
		void begin(suite const &, char const * /*info*/, size_t /*n_of_tests*/){}
		void end(suite const &, char const * /*info*/){}
		void start(test const &){}
		void success(test const &,char const * /*msg*/){}
		void failure(test const &,test_failure const &){}
		void error(test const &,char const * /*what*/){}
	};
}



namespace cute {
	namespace runner_aux {
	struct prefixMatcher
	{
		prefixMatcher(std::string const &prefix):prefix(prefix){}
		bool operator()(std::string const &s) const {
			size_t found=s.find(prefix);
			return found==0 && (s.size()==prefix.size() || s[prefix.size()]=='#');
		}
	private:
		std::string const prefix;
	};
	struct prefixCutter
	{
		prefixCutter(std::string const &prefix):prefix(prefix){}
		std::string operator()(std::string s) const {
			size_t found=s.find(prefix);
			if ( found==0 && s.size()>prefix.size() && s[prefix.size()]=='#'){
				s = s.substr(prefix.size()+1);
			} else {
				s.clear(); // either no match, or no individual test
			}
			return s;
		}
	private:
		std::string const prefix;
	};
	class ArgvTestFilter
	{
	    std::set<std::string> match;
	    bool shouldRunSuite(std::string const &info, std::vector<std::string> const &args)
	    {
	        if(!args.size() || !info.size())
	            return true;
	        if(args.end() != find_if(args.begin(), args.end(), prefixMatcher(info))){
	           std::transform(args.begin(), args.end(), std::inserter(match,match.begin()),prefixCutter(info));
	           match.erase(std::string()); // get rid of empty string
	           return true;
	        }
	        return false;
	    }
	public:
	    bool const shouldrunsuite;
		ArgvTestFilter(std::string const &info, std::vector<std::string> const &args)
		:shouldrunsuite(shouldRunSuite(info,args)){}
	    bool shouldRun(const std::string & name) const
	    {
	        return match.empty() || match.count(name);
	    }
	};
	} // namespace runner_aux
	template <typename Listener=null_listener>
	struct runner{
		Listener &listener;
	    std::vector<std::string> args;
		runner(Listener &l, int argc = 0, const char *const *argv = 0):listener(l){
	        if(needsFiltering(argc,argv)){
	            std::remove_copy_if(argv + 1, argv + argc,back_inserter(args),std::logical_not<char const *>());
	        }
		}
		bool operator()(const test & t) const
	    {
	        return runit(t);
	    }

	    bool operator ()(suite const &s, const char *info = "") const
	    {
	    	runner_aux::ArgvTestFilter filter(info,args);

	        bool result = true;
	        if(filter.shouldrunsuite){
	            listener.begin(s, info, (size_t)
	            		count_if(s.begin(),s.end(),std::bind(&runner_aux::ArgvTestFilter::shouldRun,filter,std::bind(&test::name,std::placeholders::_1))));
	            for(suite::const_iterator it = s.begin();it != s.end();++it){
	                if (filter.shouldRun(it->name())) result = this->runit(*it) && result;
	            }
	            listener.end(s, info);
	        }

	        return result;
	    }
	private:
	    bool needsFiltering(int argc, const char *const *argv) const
	    {
	        return argc > 1 && argv ;
	    }


	    bool runit(const test & t) const
	    {
	        try {
	            listener.start(t);
	            t();
	            listener.success(t, "OK");
	            return true;
	        } catch(const cute::test_failure & e){
	            listener.failure(t, e);
	        } catch(const std::exception & exc){
	            listener.error(t, demangle(exc.what()).c_str());
	        } catch(std::string & s){
	            listener.error(t, s.c_str());
	        } catch(const char *&cs) {
				listener.error(t,cs);
			} catch(...) {
				listener.error(t,"unknown exception thrown");
			}
			return false;
		}
	};
	template <typename Listener>
	runner<Listener> makeRunner(Listener &s, int argc = 0, const char *const *argv = 0){
		return runner<Listener>(s,argc,argv);
	}
}

namespace cute{
	template <typename Listener=null_listener>
	struct counting_listener:Listener{
		counting_listener()
		:Listener()
		,numberOfTests(0),successfulTests(0),failedTests(0),errors(0),numberOfSuites(0),numberOfTestsInSuites(0){}

		counting_listener(Listener const &s)
		:Listener(s)
		,numberOfTests(0),successfulTests(0),failedTests(0),errors(0),numberOfSuites(0),numberOfTestsInSuites(0){}

		void begin(suite const &s,char const *info, size_t n_of_tests){
			++numberOfSuites;
			numberOfTestsInSuites+=n_of_tests;
			Listener::begin(s,info, n_of_tests);
		}
		void start(test const &t){
			++numberOfTests;
			Listener::start(t);
		}
		void success(test const &t,char const *msg){
			++successfulTests;
			Listener::success(t,msg);
		}
		void failure(test const &t,test_failure const &e){
			++failedTests;
			Listener::failure(t,e);
		}
		void error(test const &t,char const *what){
			++errors;
			Listener::error(t,what);
		}
		size_t numberOfTests;
		size_t successfulTests;
		size_t failedTests;
		size_t errors;
		size_t numberOfSuites;
		size_t numberOfTestsInSuites;

	};
}

#ifndef DONT_USE_IOSTREAM

namespace cute {
	template <typename Listener=null_listener>
	struct ide_listener: Listener
	{
		ide_listener(std::ostream &os=std::cout):out(os) {}
		void begin(suite const &t,char const *info, size_t n_of_tests){
			out << std::dec << "\n#beginning " << info << " " << n_of_tests << '\n' << std::flush;
			Listener::begin(t,info,n_of_tests);
		}
		void end(suite const &t, char const *info){
			out << "\n#ending " << info << '\n' << std::flush;
			Listener::end(t,info);
		}
		void start(test const &t){
			out << "\n#starting " << t.name() << '\n' << std::flush;
			Listener::start(t);
		}
		void success(test const &t, char const *msg){
			out << "\n#success " <<  maskBlanks(t.name()) << " " << msg << '\n' << std::flush;
			Listener::success(t,msg);
		}
		void failure(test const &t,test_failure const &e){
			out << std::dec <<  "\n#failure " << maskBlanks(t.name()) << " " << e.filename << ":" << e.lineno << " " << (e.reason) << '\n' << std::flush;
			Listener::failure(t,e);
#ifdef _MSC_VER
			std::ostringstream os;
			os << std::dec << e.filename << "(" << e.lineno << ") : failure: " <<e.reason << " in " << t.name() << '\n' << std::flush;
			OutputDebugString(os.str().c_str());
#endif
		}
		void error(test const &t, char const *what){
			out << "\n#error " << maskBlanks(t.name()) << " " << what << '\n' << std::flush;
			Listener::error(t,what);
#ifdef _MSC_VER
			std::ostringstream os;
			os << what << " error in " << t.name() << '\n' << std::flush;
			OutputDebugString(os.str().c_str());
#endif
		}
		static std::string maskBlanks(std::string in) {
			std::transform(in.begin(),in.end(),in.begin(),blankToUnderscore());
			return in;
		}
	protected:
		struct blankToUnderscore{
            char operator()(char in) const {
			if (in == ' ') return '_';
			return in;
		}
        };
		std::ostream &out;
	};
}



namespace cute {
struct xml_file_opener {
	std::string filename;
	std::ofstream out;
	xml_file_opener(int argc, char const *const* argv)
	:filename(argc>0&&argv[0]?basename(argv[0]):"testresult.xml")
	,out(filename.c_str()){}
	std::string basename(std::string path){
#if defined( _MSC_VER ) || defined(__MINGW32__)
		char const sep='\\';
#else
		char const sep='/';
#endif
		std::string::size_type pos=path.find_last_of(sep,path.size()-1);
		if (pos != std::string::npos) path.erase(0,pos+1);
		path+=".xml";
		return path;
	}
};
}

namespace cute {
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
					std::string replacement = "0x" + cute_to_string::hexit(std::uint8_t(c));
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
			out << "<testsuites>\n";
		}
		~xml_listener(){
			out << "</testsuites>\n"<< std::flush;
		}

		void begin(suite const &t,char const *info, size_t n_of_tests){
			current_suite=mask_xml_chars(info);
			out << std::dec << "\t<testsuite name=\"" << current_suite << "\" tests=\"" << n_of_tests << "\">\n";
			Listener::begin(t,info, n_of_tests);
		}
		void end(suite const &t, char const *info){
			out << "\t</testsuite>\n";
			current_suite.clear();
			Listener::end(t,info);
		}
		void start(test const &t){
			out << "\t\t<testcase classname=\""<<current_suite <<"\" name=\""<< mask_xml_chars(t.name())<<"\"";
			Listener::start(t);
		}
		void success(test const &t, char const *msg){
			out << "/>\n";
			Listener::success(t,msg);
		}
		void failure(test const &t,test_failure const &e){
			out << std::dec <<  ">\n\t\t\t<failure message=\"" << mask_xml_chars(e.filename) << ":" << e.lineno << " "
				<< mask_xml_chars(e.reason) << "\">\n"<<mask_xml_chars(e.reason)<<"\n\t\t\t</failure>\n\t\t</testcase>\n";
			Listener::failure(t,e);
		}
		void error(test const &t, char const *what){
			out << ">\n\t\t\t<error message=\"" << mask_xml_chars(t.name()) << " " << mask_xml_chars(what)
				<< "\" type=\"unexpected exception\">\n"<<mask_xml_chars(what)
				<<"\n\t\t\t</error>\n\t\t</testcase>\n";
			Listener::error(t,what);
		}
	};
}




namespace cute {

	template<typename Listener=null_listener>
	struct tap_listener : Listener {
		tap_listener(std::ostream & out = std::cout) : out(out),nofTests(0) { }

		~tap_listener() {
			if (nofTests) {
				out << "1.." << nofTests << '\n';
			}
		}

		void begin(suite const & suite, char const * info, size_t n_of_tests) {
			out << "# Starting suite '" << info << "' containing " << n_of_tests << " tests\n";
			Listener::begin(suite, info, n_of_tests);
		}

		void end(suite const & suite, char const * info) {
			out << "# Ending suite '" << info << "'\n";
			Listener::end(suite, info);
		}

		void start(test const & test) {
			++nofTests;
			Listener::start(test);
		}

		void success(test const & test, char const * msg) {
			out << "ok " << nofTests << ' ' << test.name() << '\n';
			Listener::success(test, msg);
		}

		void failure(test const & test, test_failure const & reason) {
			out << "not ok " << nofTests << ' ' << test.name() << '\n';
			out << "# Assertion failed: " << reason.what() << '\n';
			Listener::failure(test, reason);
		}

		void error(test const & test, char const * what) {
			out << "not ok " << nofTests << ' ' << test.name() << '\n';
			out << "# Unexpected exception: " << what << '\n';
			Listener::error(test, what);
		}

	private:
		std::ostream & out;
		std::size_t nofTests;
	};

}
#endif



#endif /* CUTE_SH_H_ */
