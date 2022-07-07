#pragma once

#include "utility/unique_handle.hpp"


namespace A {
    template<typename Handle_t>
    class Unique_handle_interface: protected Unique_handle<Handle_t> {
    public:
	using Unique_handle<Handle_t>::Unique_handle;
	using Unique_handle<Handle_t>::Handle;
	using Unique_handle<Handle_t>::operator Handle_t;
    };

    template<typename Handle_t>
    class Unique_handle_interface<Unique_handle<Handle_t>>;
}
