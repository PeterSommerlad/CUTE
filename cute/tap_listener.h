/*********************************************************************************
 * This file is part of CUTE.
 *
 * CUTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CUTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with CUTE.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2017 Felix Morgner <felix.morgner@gmail.com>
 *
 *********************************************************************************/

#ifndef TAP_LISTENER_H_
#define TAP_LISTENER_H_

#include <cstdint>
#include <iostream>

#include "cute_listener.h"

namespace cute {

	template<typename Listener=null_listener>
	struct tap_listener : Listener {
		tap_listener(std::ostream & out = std::cout) : out{out} { }

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
		std::size_t nofTests{};
	};

}

#endif /* TAP_LISTENER_H_ */

