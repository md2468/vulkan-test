#pragma once

#include <thread>


namespace A {
    // Initialized in main()
    inline std::thread::id main_thread_id{};

    inline bool is_main_thread() noexcept {
	return std::this_thread::get_id() == main_thread_id;
    }
}
