#pragma once

#include <source_location>
#include <boost/stacktrace.hpp>
#include "io/console.hpp"


namespace A {
    // This allows to use default arguments with a parameter pack
    template<typename... Args>
    struct critical_error {
	[[noreturn]] critical_error(Args... args, std::source_location source_location = std::source_location::current()) noexcept {
	    try {
		Io::Console::write_line(
		    "Internal error:  ", args...,
		    "\nLocation:        ", source_location.file_name(), ":", source_location.line(),
		    "\nStack:           ", source_location.function_name(),
		    "\n", boost::stacktrace::stacktrace(),
		    "\nTerminating"
		);
	    } catch(...) {
		// There's nothing that can be done with an exception at this point
		void();
	    }

	    // std::abort() is noexcept
	    std::abort();
	}

    };

    template<typename... Args>
    critical_error(Args const&...) -> critical_error<Args const&...>;
}

