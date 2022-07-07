#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>
#include <cstdint>


namespace A::Graphics {
    class Queue: public Unique_handle_interface<VkQueue> {
    public:
	static VkDeviceQueueCreateInfo make_creation_info(std::uint32_t family_index, std::uint32_t count);

	Queue() = default;
	Queue(class Logical_device& logical_device, std::uint32_t family_index, std::uint32_t index);

	std::uint32_t get_family_index() const noexcept;
	std::uint32_t get_index() const noexcept;

	void submit(class Command_buffer& command_buffer, class Semaphore& wait_sem, class Semaphore& signal_sem, VkPipelineStageFlags wait_stages);
	void present(class Swap_chain& swap_chain, std::uint32_t swap_chain_image_index, class Semaphore& wait_sem);

    private:
	// family_index corresponds to the index of an element of the pQueueFamilyProperties array 
	// that was returned by vkGetPhysicalDeviceQueueFamilyProperties
	std::uint32_t family_index;
	std::uint32_t index;

    };
}
