#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>


namespace A::Graphics {
    class Semaphore: public Unique_handle_interface<VkSemaphore> {
    public:
	Semaphore(class Logical_device&	logical_device);
    };
}
