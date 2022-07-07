#include "graphics/image/image.hpp"
#include "graphics/wsi/swap_chain.hpp"
#include "graphics/device/logical.hpp"

using namespace A::Graphics;


std::vector<Image> Image::get_swap_chain_images(Logical_device& logical_device, Swap_chain& swap_chain) {
    // Application must not destroy these images
    
    std::uint32_t actual_image_count;
    A_ASSERT(!vkGetSwapchainImagesKHR(logical_device, swap_chain, &actual_image_count, nullptr)); 
    std::vector<VkImage> created_images(actual_image_count);
    A_ASSERT(!vkGetSwapchainImagesKHR(logical_device, swap_chain, &actual_image_count, created_images.data())); 

    std::vector<Image> result;
    result.reserve(actual_image_count);
    for(auto image : created_images) result.push_back(Image(image, swap_chain)); // push_back() because ctor is private

    return result;
}

Image::Image(VkImage image, Swap_chain& swap_chain): 
    Unique_handle_interface{image, Unique_handle::noop_deleter},
    format{swap_chain.get_image_format()},
    extent{swap_chain.get_image_extent()}
{ }

VkFormat Image::get_format() const noexcept {
    return format;
}

VkExtent2D Image::get_extent() const noexcept {
    return extent;
}
