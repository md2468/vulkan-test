#include <iostream>
#include <typeinfo>
#include <boost/core/demangle.hpp>
#include "diagnostic/critical_error.hpp"
#include "diagnostic/signal.hpp"
#include "diagnostic/terminate.hpp"
#include "concurrency/main_thread.hpp"
#include "main/a.hpp"


int main(int argc, char** argv) {
    using namespace A;

    try {
	main_thread_id = std::this_thread::get_id();
	Signal::set_handler(SIGSEGV, Signal::Handlers::die);
	Terminate::set_handler(Terminate::Handlers::die);

	a({const_cast<char const* const*>(argv), static_cast<std::size_t>(argc)});
	
	return 0;
    } catch(std::exception const& exception) {
	critical_error("unhandled exception (", boost::core::demangle(typeid(exception).name()), "): ", exception.what());
    } catch(...) {
	critical_error("unhandled exception");
    }

}
