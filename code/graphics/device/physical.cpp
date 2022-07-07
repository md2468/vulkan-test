#include "graphics/device/physical.hpp"
#include "graphics/state/instance.hpp"
#include "graphics/wsi/surface.hpp"
#include "diagnostic/assert.hpp"
#include <cstdint>
#include <ranges>

using namespace A::Graphics;


std::span<Physical_device> Physical_device::enumerate_all(class Instance& instance) {
    static std::vector<Physical_device> devices = [&]() -> std::vector<Physical_device> {
	std::uint32_t device_count = 0;
	A_ASSERT(!vkEnumeratePhysicalDevices(instance, &device_count, nullptr));
	std::vector<VkPhysicalDevice> device_handles{device_count};
	A_ASSERT(!vkEnumeratePhysicalDevices(instance, &device_count, device_handles.data()));

	std::vector<Physical_device> result;
	result.reserve(device_count);

	std::ranges::transform(
	    device_handles, 
	    std::back_insert_iterator{result}, 
	    [](VkPhysicalDevice device_handle) -> auto { return Physical_device{device_handle}; }
	);

	return result;
    }();

    return {devices};
}


Physical_device::Physical_device(Handle physical_device) {
    Unique_handle::reset(physical_device, +[](Handle) -> void { });

    vkGetPhysicalDeviceProperties(*this, &properties);

    std::uint32_t queue_family_properties_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*this, &queue_family_properties_count, nullptr);
    queue_family_properties.resize(queue_family_properties_count);
    vkGetPhysicalDeviceQueueFamilyProperties(*this, &queue_family_properties_count, queue_family_properties.data());
}


VkPhysicalDeviceProperties const& Physical_device::get_properties() const noexcept {
    return properties;
}

std::uint32_t Physical_device::get_queue_family_count() const noexcept {
    return queue_family_properties.size();
}

VkQueueFamilyProperties const& Physical_device::get_queue_family_properties(std::uint32_t queue_index) const {
    return queue_family_properties[queue_index];
}

bool Physical_device::get_queue_family_presentation_support(Surface& surface, std::uint32_t queue_family_index) {
    VkBool32 result;
    A_ASSERT(!vkGetPhysicalDeviceSurfaceSupportKHR(*this, queue_family_index, surface, &result));
    return result;
}

VkSurfaceCapabilitiesKHR Physical_device::get_surface_capabilities(Surface& surface) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    A_ASSERT(!vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*this, surface, &surface_capabilities));
    return surface_capabilities;
}

std::vector<VkSurfaceFormatKHR> Physical_device::get_surface_formats(Surface& surface) {
    std::uint32_t surface_format_count;
    A_ASSERT(!vkGetPhysicalDeviceSurfaceFormatsKHR(*this, surface, &surface_format_count, nullptr));
    std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
    A_ASSERT(!vkGetPhysicalDeviceSurfaceFormatsKHR(*this, surface, &surface_format_count, surface_formats.data()));
    return surface_formats;
}

std::vector<VkPresentModeKHR> Physical_device::get_surface_presentation_modes(Surface& surface) {
    std::uint32_t surface_presentation_mode_count;
    A_ASSERT(!vkGetPhysicalDeviceSurfacePresentModesKHR(*this, surface, &surface_presentation_mode_count, nullptr));
    std::vector<VkPresentModeKHR> surface_presentation_modes(surface_presentation_mode_count);
    A_ASSERT(!vkGetPhysicalDeviceSurfacePresentModesKHR(*this, surface, &surface_presentation_mode_count, surface_presentation_modes.data()));
    return surface_presentation_modes;
}
