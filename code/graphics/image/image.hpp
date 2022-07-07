#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>
#include <cstdint>


namespace A::Graphics {
    class Image: public Unique_handle_interface<VkImage> {
    public:
	static std::vector<Image> get_swap_chain_images(class Logical_device& logical_device, class Swap_chain& swap_chain);

	VkFormat get_format() const noexcept;
	VkExtent2D get_extent() const noexcept;

    private:
	Image(VkImage image, class Swap_chain& swap_chain);

	VkFormat format;
	VkExtent2D extent;
    };
}
