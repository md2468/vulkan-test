#include "graphics/rendering/renderer.hpp"
#include "graphics/device/logical.hpp"
#include "graphics/shader/module.hpp"
#include "graphics/wsi/swap_chain.hpp"

using namespace A::Graphics;

Renderer::Renderer(
	Logical_device& _logical_device, Swap_chain& _swap_chain,
	Shader_module& _vertex_shader, Shader_module& _fragment_shader
):
    logical_device(&_logical_device),
    swap_chain(&_swap_chain),
    image_available_sem(*logical_device),
    rendering_finished_sem(*logical_device),
    graphics_command_pool(*logical_device, logical_device->get_graphics_queue().get_family_index())
{
    VkAttachmentDescription attachments[] = {{
	.flags = 0,
	.format = swap_chain->get_image_format(),
	.samples = VK_SAMPLE_COUNT_1_BIT,
	.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
	.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
	.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    }};

    VkAttachmentReference subpass_attachment_references[] = {{
	.attachment = 0, // index in attachments[]
	.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    }};

    VkSubpassDescription subpasses[] = {{
	.flags = 0,
	.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
	.inputAttachmentCount = 0,
	.pInputAttachments = nullptr,
	.colorAttachmentCount = 1,
	.pColorAttachments = subpass_attachment_references,
	.pResolveAttachments = 0,
	.pDepthStencilAttachment = nullptr,
	.preserveAttachmentCount = 0,
	.pPreserveAttachments= nullptr
    }};
    
    VkSubpassDependency subpass_dependencies[] = {{
	.srcSubpass = VK_SUBPASS_EXTERNAL,
	.dstSubpass = 0,
	.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	.srcAccessMask = 0,
	.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	.dependencyFlags = 0
    }};

    render_pass = {*logical_device, attachments, subpasses, subpass_dependencies};

    pipeline = {
	*logical_device, 
	_vertex_shader, _fragment_shader, 
	swap_chain->get_image_extent().width, swap_chain->get_image_extent().height,
	swap_chain->get_image_format(),
	render_pass,
	0
    };
    
    framebuffers.reserve(swap_chain->get_image_count());
    graphics_command_buffers.reserve(swap_chain->get_image_count());
    for(unsigned i = 0; i < swap_chain->get_image_count(); ++i) {
	framebuffers.emplace_back(*logical_device, render_pass, swap_chain->get_image_view(i));
	graphics_command_buffers.emplace_back(graphics_command_pool.allocate_command_buffer());
    }

    record_command_buffers();

}

void Renderer::draw_frame() {
    std::uint32_t acquired_image_index = swap_chain->acquire_next_image(image_available_sem);
    logical_device->submit_drawing_commands(graphics_command_buffers[acquired_image_index], image_available_sem, rendering_finished_sem);
    logical_device->present(*swap_chain, acquired_image_index, rendering_finished_sem);
}


void Renderer::record_command_buffers() {
    for(unsigned i = 0; i< graphics_command_buffers.size(); ++i) {
	graphics_command_buffers[i].begin_recording();
	
	// See: VK_ATTACHMENT_LOAD_OP_CLEAR
	VkClearValue clear_values[] = {
	    { .color = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } } }
	};

	VkRenderPassBeginInfo render_pass_begin_info{
	    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
	    .pNext = nullptr,
	    .renderPass = render_pass,
	    .framebuffer = framebuffers[i],
	    .renderArea = {
		.offset = { .x = 0, .y = 0 },
		.extent = swap_chain->get_image_extent()
	    },
	    .clearValueCount = 1,
	    .pClearValues = clear_values
	};

	vkCmdBeginRenderPass(graphics_command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdDraw(graphics_command_buffers[i], 3, 1, 0, 0);

	vkCmdEndRenderPass(graphics_command_buffers[i]);

	graphics_command_buffers[i].end_recording();

    }
}
