#pragma once

#include <iostream>
#include <syncstream>


namespace A {
    namespace Io {
	struct Console {
	    static void write(auto const&... args) {
		// When running the executable from Vim, the output isn't printed without std::flush
		// if the program terminates abnormally
		(std::osyncstream{std::cout} << ... << args) << std::flush;
	    }

	    static void write_line(auto const&... args) {
		write(args..., '\n');
	    }
	};
    }
}
