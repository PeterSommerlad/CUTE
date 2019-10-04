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

#ifndef MOCK_LISTENER_H_
#define MOCK_LISTENER_H_
#include "cute.h"
namespace cute_test {
using namespace cute;
struct mock_listener {
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
}
#endif /* MOCK_LISTENER_H_ */
