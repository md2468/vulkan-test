#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>

namespace A::Graphics {
    class Command_buffer: public Unique_handle_interface<VkCommandBuffer> {
    public:
	// Command_buffer must not outlive the VkCommandPool object of command_pool
	Command_buffer(class Logical_device& logical_device, Handle command_buffer, class Command_pool& command_pool);

	void begin_recording();
	void end_recording();
    };
}
