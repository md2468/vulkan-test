#include "graphics/command/command_pool.hpp"
#include "graphics/device/logical.hpp"

using namespace A::Graphics;


Command_pool::Command_pool(Logical_device& logical_device, std::uint32_t queue_family_index):
    logical_device{&logical_device}
{
    VkCommandPoolCreateInfo create_info{
	.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.queueFamilyIndex = queue_family_index
    };

    Handle command_pool;
    A_ASSERT(!vkCreateCommandPool(logical_device, &create_info, nullptr, &command_pool)); 
    Unique_handle::reset(command_pool, [&logical_device](Handle command_pool) { vkDestroyCommandPool(logical_device, command_pool, nullptr); });
}


Command_buffer Command_pool::allocate_command_buffer() {
    VkCommandBufferAllocateInfo allocate_info{
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	.pNext = nullptr,
	.commandPool = *this,
	.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	.commandBufferCount = 1
    };
    
    VkCommandBuffer command_buffers[1];
    // If the allocation of any of these command buffers fails, the implementation must free all successfully allocated command buffer objects
    A_ASSERT(!vkAllocateCommandBuffers(*logical_device, &allocate_info, command_buffers));
    
    return {*logical_device, command_buffers[0], *this};
}
