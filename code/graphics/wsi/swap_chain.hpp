#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>
#include "graphics/image/image.hpp"
#include "graphics/image/view.hpp"


namespace A::Graphics {
    class Swap_chain: public Unique_handle_interface<VkSwapchainKHR> {
    public:
	Swap_chain() = default;
	Swap_chain(class Logical_device& logical_device, class Surface& surface, class Window& window);

	std::size_t get_image_count() const noexcept;
	VkExtent2D get_image_extent() const noexcept;
	VkFormat get_image_format() const noexcept;
	class Image_view& get_image_view(std::size_t index) noexcept;
	
	// Returns the index of the acquired image, as obtained from vkGetSwapchainImagesKHR()
	std::uint32_t acquire_next_image(class Semaphore& semaphore);

    private:
	class Logical_device* logical_device;
	VkFormat image_format;
	VkExtent2D image_extent;
	std::vector<Image> images;
	std::vector<Image_view> image_views;

    };
}
