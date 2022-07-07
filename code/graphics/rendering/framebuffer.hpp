#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>

namespace A::Graphics {
    class Framebuffer: public Unique_handle_interface<VkFramebuffer> {
    public:
	Framebuffer(class Logical_device& logical_device, class Render_pass& render_pass, class Image_view& image_view);
    };
}
