#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>
#include <cstdint>


namespace A::Graphics {
    class Image_view: public Unique_handle_interface<VkImageView> {
    public:
	Image_view(class Logical_device& logical_device, class Image& image);

	VkFormat get_image_format() const noexcept;
	VkExtent2D get_image_extent() const noexcept;

    private:
	VkFormat image_format;
	VkExtent2D image_extent;
    };
}


