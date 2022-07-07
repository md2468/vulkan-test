#pragma once

#include <stdexcept>

namespace A {
    struct Exception: public std::runtime_error {
	using std::runtime_error::runtime_error;
    };

    namespace Exceptions {
	struct Internal_error: public Exception {
	    using Exception::Exception;
	};
    }
}

