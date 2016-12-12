/*
 * mock_listener.h
 *
 *  Created on: Jul 1, 2013
 *      Author: hsr
 */

#ifndef MOCK_LISTENER_H_
#define MOCK_LISTENER_H_
#include "cute.h"
namespace cute_test {
using namespace cute;
struct mock_listener {
	unsigned int begincount;
	unsigned int endcount;
	unsigned int startcount;
	unsigned int successcount;
	unsigned int failurecount;
	unsigned int errorcount;
	unsigned int suitetestcount;
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
}
#endif /* MOCK_LISTENER_H_ */
