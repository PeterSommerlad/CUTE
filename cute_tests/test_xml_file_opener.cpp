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

#include "cute_xml_file.h"
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_xml_file_opener.h"
#include <cstdio> //for remove
#if defined( _MSC_VER ) || defined(__MINGW32__)
#define SEP "\\"
#else
#define SEP "/"
#endif
void thisIsAtest_xml_file_openerTest() {
	char const *argv[2]={SEP "root" SEP "test",0};
	cute::xml_file_opener xmlfile(1,argv);
	ASSERT_EQUAL("test.xml",xmlfile.filename);
	ASSERT(xmlfile.out.is_open());
	xmlfile.out.close();
	ASSERT_EQUAL(0,remove(xmlfile.filename.c_str()));
}
void test_xml_file_with_no_name(){
	char const *argv[1]={0};
	cute::xml_file_opener xmlfile(0,argv);
	ASSERT_EQUAL("testresult.xml",xmlfile.filename);
	ASSERT(xmlfile.out.is_open());
	xmlfile.out.close();
	ASSERT_EQUAL(0,remove(xmlfile.filename.c_str()));
}
void test_xml_file_with_path(){
	char const *argv[2]={"." SEP,0};
	cute::xml_file_opener xmlfile(1,argv);
	ASSERT_EQUAL(".xml",xmlfile.filename);
	ASSERT(xmlfile.out.is_open());
	xmlfile.out.close();
	ASSERT_EQUAL(0,remove(xmlfile.filename.c_str()));
}



cute::suite make_suite_test_xml_file_opener(){
	cute::suite s;
	s.push_back(CUTE(thisIsAtest_xml_file_openerTest));
	s.push_back(CUTE(test_xml_file_with_no_name));
	s.push_back(CUTE(test_xml_file_with_path));
	return s;
}



