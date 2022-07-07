#include "graphics/command/queue.hpp"
#include "graphics/command/command_buffer.hpp"
#include "graphics/device/logical.hpp"
#include "graphics/wsi/swap_chain.hpp"
#include "graphics/synchronisation/semaphore.hpp"

using namespace A::Graphics;


Queue::Queue(Logical_device& logical_device, std::uint32_t family_index_, std::uint32_t index_):
    family_index{family_index_}, index{index_}
{
    Handle queue;
    vkGetDeviceQueue(logical_device, family_index, index, &queue);
    Unique_handle::reset(queue, Unique_handle::noop_deleter);
}

std::uint32_t Queue::get_index() const noexcept {
    A_ASSERT(Unique_handle::has_value());
    return index;
}

std::uint32_t Queue::get_family_index() const noexcept {
    A_ASSERT(Unique_handle::has_value());
    return family_index;
}

void Queue::submit(Command_buffer& command_buffer_, Semaphore& wait_sem_, Semaphore& signal_sem_, VkPipelineStageFlags wait_stages_) {
    VkCommandBuffer command_buffers[] = { command_buffer_ };
    VkPipelineStageFlags wait_stages[] = { wait_stages_ };
    VkSemaphore wait_semaphores[] = { wait_sem_ };
    VkSemaphore signal_semaphores[] = { signal_sem_ };

    VkSubmitInfo submit_info[] = {{
	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	.pNext = nullptr,
	.waitSemaphoreCount = 1,
	.pWaitSemaphores = wait_semaphores,
	.pWaitDstStageMask = wait_stages,
	.commandBufferCount = 1,
	.pCommandBuffers = command_buffers,
	.signalSemaphoreCount = 1,
	.pSignalSemaphores = signal_semaphores
    }};

    A_ASSERT(!vkQueueSubmit(*this, 1, submit_info, VK_NULL_HANDLE)); 
}


void Queue::present(Swap_chain& swap_chain_, std::uint32_t swap_chain_image_index_, Semaphore& wait_sem_) {
    VkSwapchainKHR swap_chains[] = { swap_chain_ };
    std::uint32_t image_indices[] = { swap_chain_image_index_ };
    VkSemaphore wait_semaphores[] = { wait_sem_ };

    VkPresentInfoKHR present_info{
	.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
	.pNext = nullptr,
	.waitSemaphoreCount = 1,
	.pWaitSemaphores = wait_semaphores,
	.swapchainCount = 1,
	.pSwapchains = swap_chains,
	.pImageIndices = image_indices,
	.pResults = nullptr
    };

    A_ASSERT(!vkQueuePresentKHR(*this, &present_info)); 
}


VkDeviceQueueCreateInfo Queue::make_creation_info(std::uint32_t family_index, std::uint32_t count) {
    // The lifetime of this array must continue after exiting this function, 
    // because it's referenced in VkDeviceQueueCreateInfo
    static std::vector<float> const priorities(64, 1.0f);

    return {
	.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.queueFamilyIndex = family_index,
	.queueCount = count,
	.pQueuePriorities = priorities.data()
    };
}
