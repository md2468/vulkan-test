#include "graphics/device/logical.hpp"
#include "graphics/device/physical.hpp"
#include "graphics/wsi/surface.hpp"
#include <set>
#include <array>
#include <ranges>
#include <algorithm>

using namespace A::Graphics;


Logical_device::Logical_device(Physical_device& physical_device_, Surface& surface):
    physical_device{&physical_device_} 
{
    std::vector<VkDeviceQueueCreateInfo> queue_creation_info;
    std::uint32_t graphics_queue_family_index, presentation_queue_family_index;

    {
	auto has_graphics_support = [&](std::uint32_t index) { return physical_device->get_queue_family_properties(index).queueFlags & VK_QUEUE_GRAPHICS_BIT; };
	auto has_presentation_support = [&](std::uint32_t index) { return physical_device->get_queue_family_presentation_support(surface, index); };
	auto queue_family_indices = std::ranges::iota_view<std::uint32_t, std::uint32_t>(0, physical_device->get_queue_family_count());

	auto with_both_support = queue_family_indices | std::views::filter(has_graphics_support) | std::views::filter(has_presentation_support);

	if(!with_both_support.empty()) {
	    queue_creation_info.emplace_back(Queue::make_creation_info(*std::ranges::begin(with_both_support), 1));
	    graphics_queue_family_index = *std::ranges::begin(with_both_support);
	    presentation_queue_family_index = *std::ranges::begin(with_both_support);
	} else {
	    auto with_graphics_support = queue_family_indices | std::views::filter(has_graphics_support);
	    auto with_presentation_support = queue_family_indices | std::views::filter(has_presentation_support);
	    A_ASSERT(!with_graphics_support.empty() && !with_presentation_support.empty());

	    queue_creation_info.emplace_back(Queue::make_creation_info(*std::ranges::begin(with_graphics_support ), 1));
	    queue_creation_info.emplace_back(Queue::make_creation_info(*std::ranges::begin(with_presentation_support ), 1));
	    graphics_queue_family_index = *std::ranges::begin(with_graphics_support);
	    presentation_queue_family_index = *std::ranges::begin(with_presentation_support);
	}
    }


    std::array extensions{
	+VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo create_info{
	.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.queueCreateInfoCount = (std::uint32_t)queue_creation_info.size(),
	.pQueueCreateInfos = queue_creation_info.data(),
	.enabledLayerCount = 0,
	.ppEnabledLayerNames = nullptr,
	.enabledExtensionCount = extensions.size(),
	.ppEnabledExtensionNames = extensions.data(),
	.pEnabledFeatures = nullptr
    };
    
    Handle logical_device;
    A_ASSERT(!vkCreateDevice(*physical_device, &create_info, nullptr, &logical_device)); 
    Unique_handle::operator=({logical_device, [](Handle logical_device) { vkDestroyDevice(logical_device, nullptr); }});
    
    graphics_queue = {*this, graphics_queue_family_index, 0};
    presentation_queue = {*this, presentation_queue_family_index, 0};
}

Physical_device& Logical_device::get_physical_device() noexcept {
    return *physical_device;
}

Queue const& Logical_device::get_graphics_queue() const noexcept {
    return graphics_queue;
}

Queue const& Logical_device::get_presentation_queue() const noexcept {
    return presentation_queue;
}

void Logical_device::submit_drawing_commands(Command_buffer& command_buffer, Semaphore& wait_sem, Semaphore& signal_sem) {
    graphics_queue.submit(command_buffer, wait_sem, signal_sem, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
}

void Logical_device::present(Swap_chain& swap_chain, std::uint32_t swap_chain_image_index, Semaphore& wait_sem) {
    presentation_queue.present(swap_chain, swap_chain_image_index, wait_sem);
}

void Logical_device::wait_idle() {
    vkDeviceWaitIdle(*this);
}


