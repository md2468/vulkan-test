#include "graphics/image/view.hpp"
#include "graphics/image/image.hpp"
#include "graphics/device/logical.hpp"

using namespace A::Graphics;


Image_view::Image_view(Logical_device& logical_device, Image& image):
    image_format{image.get_format()},
    image_extent{image.get_extent()}
{
    VkImageViewCreateInfo create_info{
	.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.image = image,
	.viewType = VK_IMAGE_VIEW_TYPE_2D,
	.format = image.get_format(),
	.components = {
	    VK_COMPONENT_SWIZZLE_IDENTITY,
	    VK_COMPONENT_SWIZZLE_IDENTITY,
	    VK_COMPONENT_SWIZZLE_IDENTITY,
	    VK_COMPONENT_SWIZZLE_IDENTITY
	},
	.subresourceRange = {
	    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
	    .baseMipLevel = 0,
	    .levelCount = 1,
	    .baseArrayLayer = 0,
	    .layerCount = 1
	}
    };

    Handle image_view;
    A_ASSERT(!vkCreateImageView(logical_device, &create_info, nullptr, &image_view)); 
    Unique_handle::reset(image_view, [&logical_device](Handle image_view) { vkDestroyImageView(logical_device, image_view, nullptr); });
}

VkFormat Image_view::get_image_format() const noexcept { 
    return image_format;
}

VkExtent2D Image_view::get_image_extent() const noexcept {
    return image_extent;
}

