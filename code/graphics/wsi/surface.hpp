#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>


namespace A::Graphics {
    class Surface: public Unique_handle_interface<VkSurfaceKHR> {
    friend class Window;

    private:
	Surface(Handle handle, class Instance& instance);
    };
}

