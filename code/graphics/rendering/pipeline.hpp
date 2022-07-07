#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>

namespace A::Graphics {
    class Pipeline: public Unique_handle_interface<VkPipeline> {
    public:
	Pipeline() = default;

	Pipeline(
	    class Logical_device& logical_device, 
	    class Shader_module& vertex_shader, class Shader_module& fragment_shader, 
	    std::uint32_t viewport_width, std::uint32_t viewport_height,
	    VkFormat colour_attachment_format,
	    class Render_pass& render_pass,
	    std::uint32_t subpass_index
	);

	~Pipeline();

	Pipeline(Pipeline&&) = default;
	Pipeline& operator=(Pipeline&&) = default;

    private:
	class Layout: public Unique_handle_interface<VkPipelineLayout> {
	public:
	    Layout() = default;
	    Layout(class Logical_device& logical_device);
	} layout;

    };
}

