/*********************************************************************************
 * This file is part of CUTE.
 *
 * Copyright (c) 2007-2018 Peter Sommerlad, IFS
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

#ifndef IDE_LISTENER_H_
#define IDE_LISTENER_H_

#include "cute_listener.h"

#include <iostream>
#include <algorithm>
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sstream>
#endif

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
#ifdef _MSC_VER
			HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
			if (hStdOut != NULL && hStdOut != INVALID_HANDLE_VALUE) {
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				if (GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
					WORD consoleAttributes = csbi.wAttributes & ~(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					SetConsoleTextAttribute(hStdOut, consoleAttributes | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					out << "\n#success " << maskBlanks(t.name()) << " " << msg << '\n' << std::flush;
					SetConsoleTextAttribute(hStdOut, csbi.wAttributes);
				} else {
					out << "\n#success " << maskBlanks(t.name()) << " " << msg << '\n' << std::flush;
				}
			}
#else
			out << "\n#success " << maskBlanks(t.name()) << " " << msg << '\n' << std::flush;
#endif
			Listener::success(t,msg);
		}
		void failure(test const &t,test_failure const &e){
#ifdef _MSC_VER
			HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
			if (hStdOut != NULL && hStdOut != INVALID_HANDLE_VALUE) {
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				if (GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
					WORD consoleAttributes = csbi.wAttributes & ~(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					SetConsoleTextAttribute(hStdOut, consoleAttributes | FOREGROUND_RED | FOREGROUND_INTENSITY);
					out << std::dec << "\n#failure " << maskBlanks(t.name()) << " " << e.filename << ":" << e.lineno << " " << (e.reason) << '\n' << std::flush;
					SetConsoleTextAttribute(hStdOut, csbi.wAttributes);
				} else {
					out << std::dec << "\n#failure " << maskBlanks(t.name()) << " " << e.filename << ":" << e.lineno << " " << (e.reason) << '\n' << std::flush;
				}
			}

			std::ostringstream os;
			os << std::dec << e.filename << "(" << e.lineno << ") : failure: " << e.reason << " in " << t.name() << '\n' << std::flush;
			OutputDebugString(os.str().c_str());
#else
			out << std::dec << "\n#failure " << maskBlanks(t.name()) << " " << e.filename << ":" << e.lineno << " " << (e.reason) << '\n' << std::flush;
#endif
			Listener::failure(t,e);
		}
		void error(test const &t, char const *what){
#ifdef _MSC_VER
			HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
			if (hStdOut != NULL && hStdOut != INVALID_HANDLE_VALUE) {
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				if (GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
					WORD consoleAttributes = csbi.wAttributes & ~(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					SetConsoleTextAttribute(hStdOut, consoleAttributes | FOREGROUND_RED | FOREGROUND_INTENSITY);
					out << "\n#error " << maskBlanks(t.name()) << " " << what << '\n' << std::flush;
					SetConsoleTextAttribute(hStdOut, csbi.wAttributes);
				} else {
					out << "\n#error " << maskBlanks(t.name()) << " " << what << '\n' << std::flush;
				}
			}

			std::ostringstream os;
			os << what << " error in " << t.name() << '\n' << std::flush;
			OutputDebugString(os.str().c_str());
#else
			out << "\n#error " << maskBlanks(t.name()) << " " << what << '\n' << std::flush;
#endif
			Listener::error(t, what);
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

#endif /*IDE_LISTENER_H_*/
