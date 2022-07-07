#include "graphics/rendering/render_pass.hpp"
#include "graphics/device/logical.hpp"

using namespace A::Graphics;

Render_pass::Render_pass(
	Logical_device& logical_device, 
	std::span<VkAttachmentDescription const> attachments, 
	std::span<VkSubpassDescription const> subpasses,
	std::span<VkSubpassDependency const> subpass_dependencies
) {
    VkRenderPassCreateInfo create_info{
	.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.attachmentCount = (std::uint32_t)attachments.size(),
	.pAttachments = attachments.data(),
	.subpassCount = (std::uint32_t)subpasses.size(),
	.pSubpasses = subpasses.data(),
	.dependencyCount = (std::uint32_t)subpass_dependencies.size(),
	.pDependencies = subpass_dependencies.data()
    };

    Handle render_pass;
    A_ASSERT(!vkCreateRenderPass(logical_device, &create_info, nullptr, &render_pass));
    Unique_handle::operator=({render_pass, [&logical_device](Handle render_pass) { vkDestroyRenderPass(logical_device, render_pass, nullptr); }});
}
