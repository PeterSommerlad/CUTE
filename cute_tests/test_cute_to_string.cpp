/*********************************************************************************
 * This file is part of CUTE.
 *
 * Copyright (c) 2013-2018 Peter Sommerlad, IFS
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

#include "test_cute_to_string.h"

#include "cute_determine_version.h"
#include "cute_to_string.h"
#include "cute.h"
#include <set>
#include <map>
using cute::cute_to_string::to_string;
void test_simple_to_string(){
	std::string res=cute::cute_to_string::to_string(42);
	ASSERT_EQUAL("42",res);
}
void test_simple_string_to_string(){
	ASSERT_EQUAL("hallo Peter\n",to_string("hallo Peter\n"));
}
void test_simple_char_array_to_string(){
	char a[]="Hello\tWorld\n";
	ASSERT_EQUAL(a,to_string(a));
}

void test_int_array_to_string(){
	int a[3];
	a[0]=1;
	a[1]=2;
	a[2]=3;
	ASSERT_EQUAL("int[3]{\n1,\n2,\n3}",to_string(a));
}

void test_pair_to_string(){
	ASSERT_EQUAL("[1 -> hallo]",to_string(std::make_pair(1,std::string("hallo"))));
}
#ifdef USE_STD11
void test_tuple_to_string(){
	ASSERT_EQUAL("std::tuple<int, int>{\n7,\n42}",to_string(std::tuple<int,int>(7,42)));
}
#endif
void test_vector_pair_to_string(){
	typedef std::vector<std::pair<int,int> > Vec;
	Vec v;
	v.push_back(std::make_pair(42,4));
	ASSERT_EQUAL("std::vector<std::pair<int, int>, std::allocator<std::pair<int, int> > >{\n[42 -> 4]}",to_string(v));
}

void test_set_to_string(){
	std::set<int> s;
	s.insert(42); s.insert(1); s.insert(100);
	std::string res=to_string(s);
	ASSERT_EQUAL("std::set<int, std::less<int>, std::allocator<int> >{\n1,\n42,\n100}",res);
// MS VC++ will not detect std::map/std::set as a container, because their begin/end member functions are defined in their superclass... :-(
}
void test_multiset_to_string(){
	std::multiset<int> s;
	s.insert(42); s.insert(42); s.insert(100);
	std::string res=to_string(s);
	ASSERT_EQUAL("std::multiset<int, std::less<int>, std::allocator<int> >{\n42,\n42,\n100}",res);
// MS VC++ will not detect std::map/std::set as a container, because their begin/end member functions are defined in their superclass... :-(
}

void test_map_to_string(){
	std::map<std::string,std::string> m;
	m["one"]="two";
	m["three"]="four";
	m["five"]="six";
	std::string exp="std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >{"
"\n[five -> six],\n[one -> two],\n[three -> four]}";
	std::string res=to_string(m);
	ASSERT_EQUAL(exp,res);
// MS VC++ will not detect std::map/std::set as a container, because their begin/end member functions are defined in their superclass... :-(
}
void test_multimap_to_string(){
	std::multimap<std::string,std::string> m;
	m.insert(std::pair<std::string,std::string>("one","two"));
	m.insert(std::pair<std::string,std::string>("one","three"));
	m.insert(std::pair<std::string,std::string>("three","four"));
	std::string exp="std::multimap<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >{"
"\n[one -> two],\n[one -> three],\n[three -> four]}"; // fragile!
	std::string res=to_string(m);
	ASSERT_EQUAL(exp,res);
// MS VC++ will not detect std::map/std::set as a container, because their begin/end member functions are defined in their superclass... :-(
}
#if defined(USE_STD17)
void test_byte_to_string(){
	std::byte b{42};
	std::string exp="0x2A";
	std::string res=to_string(b);
	ASSERT_EQUAL(exp,res);
}
void test_vector_of_byte_to_string(){
	std::vector<std::byte> v{static_cast<std::byte>(21), static_cast<std::byte>(42)};
	std::string exp="std::vector<std::byte, std::allocator<std::byte> >{\n0x15,\n0x2A}";
	std::string res=to_string(v);
	ASSERT_EQUAL(exp,res);
}
#endif








cute::suite test_cute_to_string(){
	cute::suite s;
	s.push_back(CUTE(test_simple_to_string));
	s.push_back(CUTE(test_simple_string_to_string));
	s.push_back(CUTE(test_simple_char_array_to_string));
	s.push_back(CUTE(test_int_array_to_string));
	s.push_back(CUTE(test_pair_to_string));
#ifdef USE_STD11
	s.push_back(CUTE(test_tuple_to_string));
#endif
	s.push_back(CUTE(test_vector_pair_to_string));
	s.push_back(CUTE(test_set_to_string));
	s.push_back(CUTE(test_map_to_string));
	s.push_back(CUTE(test_multimap_to_string));
	s.push_back(CUTE(test_multiset_to_string));
#if defined(USE_STD17)
	s.push_back(CUTE(test_byte_to_string));
	s.push_back(CUTE(test_vector_of_byte_to_string));
#endif
	return s;

}




