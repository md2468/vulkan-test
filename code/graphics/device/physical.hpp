#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>
#include <span>

namespace A::Graphics {
    class Physical_device: public Unique_handle_interface<VkPhysicalDevice> {
    public:
	static std::span<Physical_device> enumerate_all(class Instance& instance);

	VkPhysicalDeviceProperties const& get_properties() const noexcept;
	std::uint32_t get_queue_family_count() const noexcept;
	VkQueueFamilyProperties const& get_queue_family_properties(std::uint32_t queue_index) const;
	bool get_queue_family_presentation_support(class Surface& surface, std::uint32_t queue_family_index);
	VkSurfaceCapabilitiesKHR get_surface_capabilities(class Surface& surface);
	std::vector<VkSurfaceFormatKHR> get_surface_formats(class Surface& surface);
	std::vector<VkPresentModeKHR> get_surface_presentation_modes(class Surface& surface);

    private:
	Physical_device(Handle physical_device);

	VkPhysicalDeviceProperties properties;
	std::vector<VkQueueFamilyProperties> queue_family_properties;

    };
}
