#include "graphics/synchronisation/semaphore.hpp"
#include "graphics/device/logical.hpp"

using namespace A::Graphics;


Semaphore::Semaphore(Logical_device& logical_device) {
    VkSemaphoreCreateInfo create_info{
	.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
    };

    Handle semaphore;
    A_ASSERT(!vkCreateSemaphore(logical_device, &create_info, nullptr, &semaphore));
    Unique_handle::reset(semaphore, [&logical_device](Handle semaphore) { vkDestroySemaphore(logical_device, semaphore, nullptr); });
}
