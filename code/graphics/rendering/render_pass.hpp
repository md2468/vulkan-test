#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>
#include <span>

namespace A::Graphics {
    class Render_pass: public Unique_handle_interface<VkRenderPass> {
    public:
	Render_pass() = default;

	Render_pass(
	    class Logical_device& logical_device, 
	    std::span<VkAttachmentDescription const> attachments, 
	    std::span<VkSubpassDescription const> subpasses,
	    std::span<VkSubpassDependency const> subpass_dependencies
	);
    };
}
