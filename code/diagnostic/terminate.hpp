#pragma once

#include <exception>
#include "diagnostic/critical_error.hpp"


namespace A {
    namespace Terminate {
	inline void set_handler(void (*handler)()) {
	    std::set_terminate(handler);
	}

	namespace Handlers {
	    [[noreturn]] inline void die() {
		critical_error("std::terminate() has been called");
	    }
	}
    }
}
