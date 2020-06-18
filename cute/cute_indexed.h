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

#ifndef CUTE_INDEXED_H_
#define CUTE_INDEXED_H_

#include "cute_base.h"
#include "cute_equals.h"
#include "cute_relops.h"
#include "cute_throws.h"

#define CUTE_INDEXMSG(msg, index) (std::string(msg) + " (" + #index + "=" + cute::cute_to_string::to_string(index) + ")")

#define ASSERTMI(cond, msg, index) ASSERTM(cond, CUTE_INDEXMSG((msg), index))
#define ASSERTI(cond, index) ASSERT_MI(cond, #cond, index)

#define ASSERT_EQUALMI(msg, index, expected, actual) ASSERT_EQUALM(CUTE_INDEXMSG((msg), index), (expected), (actual))
#define ASSERT_EQUALI(index, expected, actual) ASSERT_EQUALMI(#expected " == " #actual, index, (expected), (actual))
#define ASSERT_EQUAL_DELTAMI(msg, index, expected, actual, delta) ASSERT_EQUAL_DELTAM(CUTE_INDEXMSG((msg), index), (expected), (actual), (delta))
#define ASSERT_EQUAL_DELTAI(index, expected, actual, delta) ASSERT_EQUAL_DELTAMI(#expected " == " #actual " with error " #delta, index, (expected), (actual), (delta))

#define ASSERT_LESSMI(msg, index, left, right) ASSERT_LESSM(CUTE_INDEXMSG((msg), index), (left), (right))
#define ASSERT_LESSI(index, left, right, failure) ASSERT_LESSMI(#left " < " #right, index, (left), (right))

#define ASSERT_LESS_EQUALMI(msg, index, left, right) ASSERT_LESS_EQUALM(CUTE_INDEXMSG((msg), index), (left), (right))
#define ASSERT_LESS_EQUALI(index, left, right) ASSERT_LESS_EQUALMI(#left " <= " #right, index, (left), (right))

#define ASSERT_GREATERMI(msg, index, left, right) ASSERT_GREATERM(CUTE_INDEXMSG((msg), index), (left), (right))
#define ASSERT_GREATERI(index, left, right, failure) ASSERT_GREATERMI(#left " > " #right, index, (left), (right))

#define ASSERT_GREATER_EQUALMI(msg, index, left, right) ASSERT_GREATER_EQUALM(CUTE_INDEXMSG((msg), index), (left), (right))
#define ASSERT_GREATER_EQUALI(index, left, right) ASSERT_GREATER_EQUALMI(#left " >= " #right, index, (left), (right))

#define ASSERT_NOT_EQUAL_TOMI(msg, index, left, right) ASSERT_NOT_EQUAL_TOM(CUTE_INDEXMSG((msg), index), (left), (right))
#define ASSERT_NOT_EQUAL_TOI(index, left, right) ASSERT_NOT_EQUAL_TOMI(#left " != " #right, index, (left), (right))

#define ASSERT_THROWSMI(anuncommonmessagetextparametername, index, code, exc) ASSERT_THROWSM(CUTE_INDEXMSG(anuncommonmessagetextparametername, index), code, exc)
#define ASSERT_THROWSI(index, code, exc) ASSERT_THROWSMI(" expecting " #code " to throw " #exc, index, code, exc)

#define ASSERT_THROWS_CHECKMI(anuncommonmessagetextparametername, index, code, exc, check_code) ASSERT_THROWS_CHECKM(CUTE_INDEXMSG(anuncommonmessagetextparametername, index), code, exc, check_code)
#define ASSERT_THROWS_CHECKI(index, code, exc, check_code) ASSERT_THROWS_CHECKMI(" expecting " #code " to throw " #exc, index, code, exc, check_code)

//#undef INDEXMSG

#endif /* CUTE_DATA_DRIVEN_H_ */
