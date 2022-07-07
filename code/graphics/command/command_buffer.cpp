#include "graphics/command/command_buffer.hpp"
#include "graphics/command/command_pool.hpp"
#include "graphics/device/logical.hpp"
#include <array>

using namespace A::Graphics;


Command_buffer::Command_buffer(Logical_device& logical_device, Handle command_buffer, Command_pool& command_pool) { 
    Unique_handle::reset(command_buffer, 
	[&logical_device, &command_pool](Handle command_buffer) { 
	    vkFreeCommandBuffers(logical_device, command_pool, 1, std::array{command_buffer}.data());
	}
    );
}


void Command_buffer::begin_recording() {
    VkCommandBufferBeginInfo begin_info{
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	.pNext = nullptr,
	.flags = 0,
	.pInheritanceInfo = nullptr
    };

    A_ASSERT(!vkBeginCommandBuffer(*this, &begin_info)); 
}

void Command_buffer::end_recording() {
    A_ASSERT(!vkEndCommandBuffer(*this)); 
}
