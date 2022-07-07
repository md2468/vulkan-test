#pragma once

#include <csignal>
#include "diagnostic/critical_error.hpp"

namespace A {
    namespace Signal {
	inline void set_handler(int signal, void (*handler)(int)) {
	    if(std::signal(signal, handler) == SIG_ERR) critical_error("std::signal() error");
	}

	namespace Handlers {
	    [[noreturn]] inline void die(int signal) {
		char const* signal_name = 
		    signal == SIGTERM ? "SIGTERM" :
		    signal == SIGSEGV ? "SIGSEGV" :
		    signal == SIGINT  ? "SIGINT" :
		    signal == SIGILL  ? "SIGILL" :
		    signal == SIGABRT ? "SIGABRT" :
		    signal == SIGFPE  ? "SIGFPE" :
					"UNKNOWN";
			              
		// This is UB, but at this point the program is going to terminate anyway
		critical_error("received signal ", signal_name);
	    }
	}
    }
}
