#include "graphics/rendering/framebuffer.hpp"
#include "graphics/device/logical.hpp"
#include "graphics/rendering/render_pass.hpp"
#include "graphics/image/view.hpp"

using namespace A::Graphics;

Framebuffer::Framebuffer(Logical_device& logical_device, Render_pass& render_pass, Image_view& image_view) {
    VkImageView attachments[] = {image_view};

    VkFramebufferCreateInfo create_info{
	.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.renderPass = render_pass,
	.attachmentCount = 1,
	.pAttachments = attachments,
	.width = image_view.get_image_extent().width,
	.height = image_view.get_image_extent().height,
	.layers = 1
    };

    Handle framebuffer;
    A_ASSERT(!vkCreateFramebuffer(logical_device, &create_info, nullptr, &framebuffer));
    Unique_handle::operator=({framebuffer, [&logical_device](Handle framebuffer) { vkDestroyFramebuffer(logical_device, framebuffer, nullptr); }});
}
