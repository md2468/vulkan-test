#pragma once

#include <vulkan/vulkan.h>
#include "graphics/command/command_buffer.hpp"
#include "graphics/command/command_pool.hpp"
#include "graphics/rendering/framebuffer.hpp"
#include "graphics/rendering/render_pass.hpp"
#include "graphics/rendering/pipeline.hpp"
#include "graphics/synchronisation/semaphore.hpp"

namespace A::Graphics {
    class Renderer {
    public:
	Renderer(
	    class Logical_device& logical_device, class Swap_chain& swap_chain,
	    class Shader_module& vertex_shader, class Shader_module& fragment_shader
	);

	void draw_frame();

    private:
	class Logical_device* logical_device;
	class Swap_chain* swap_chain;

	Semaphore image_available_sem;
	Semaphore rendering_finished_sem;

	Command_pool graphics_command_pool;
	std::vector<Command_buffer> graphics_command_buffers; // One command buffer for each image in the swap chain

	Render_pass render_pass;
	Pipeline pipeline;
	std::vector<Framebuffer> framebuffers;

	void record_command_buffers();

    };
}
