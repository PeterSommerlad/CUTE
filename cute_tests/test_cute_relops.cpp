#include "cute_relops.h"
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_cute_relops.h"

void test_cute_assert_less_success() {
	ASSERT_LESS(4,5);
}
void test_cute_assert_less_fail(){
	ASSERT_THROWS(ASSERT_LESS(4,2),cute::test_failure);
}
void test_cute_assert_greater_success(){
	ASSERT_GREATER(10.0,9.9);
}
void test_cute_assert_greater_fail(){
	ASSERT_THROWS(ASSERT_GREATER(2,3),cute::test_failure);
}
void test_cute_assert_less_equal_success(){
	ASSERT_LESS_EQUAL(4,4);
}
void test_cute_assert_less_equal_fail(){
	ASSERT_THROWS(ASSERT_LESS_EQUAL(4,3),cute::test_failure);
}
void test_cute_assert_greater_equal_success(){
	int const x=4;
	int const y=4;
	ASSERT_GREATER_EQUAL(x,y);
}
void test_cute_assert_greater_equal_fail(){
	ASSERT_THROWS(ASSERT_GREATER_EQUAL(4,5),cute::test_failure);
}
void test_cute_assert_not_euqal_to_success(){
	ASSERT_NOT_EQUAL_TO(1,2);
}
void test_cute_assert_not_equal_to_fail(){
	ASSERT_THROWS(ASSERT_NOT_EQUAL_TO(4,4),cute::test_failure);
}

cute::suite make_suite_test_cute_relops(){
	cute::suite s;
	s.push_back(CUTE(test_cute_assert_less_success));
	s.push_back(CUTE(test_cute_assert_less_fail));
	s.push_back(CUTE(test_cute_assert_greater_success));
	s.push_back(CUTE(test_cute_assert_greater_fail));
	s.push_back(CUTE(test_cute_assert_less_equal_success));
	s.push_back(CUTE(test_cute_assert_less_equal_fail));
	s.push_back(CUTE(test_cute_assert_greater_equal_fail));
	s.push_back(CUTE(test_cute_assert_greater_equal_success));
	s.push_back(CUTE(test_cute_assert_not_euqal_to_success));
	s.push_back(CUTE(test_cute_assert_not_equal_to_fail));
	return s;
}



