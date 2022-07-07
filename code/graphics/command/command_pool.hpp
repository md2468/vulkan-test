#pragma once

#include "utility/unique_handle_interface.hpp"
#include "graphics/command/command_buffer.hpp"
#include <vulkan/vulkan.h>


namespace A::Graphics {
    class Command_pool: public Unique_handle_interface<VkCommandPool> {
    public:
	Command_pool(class Logical_device& logical_device, std::uint32_t queue_family_index);
	
	Command_buffer allocate_command_buffer();

    private:
	class Logical_device* logical_device;

    };
}

