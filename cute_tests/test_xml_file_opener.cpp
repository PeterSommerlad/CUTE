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



