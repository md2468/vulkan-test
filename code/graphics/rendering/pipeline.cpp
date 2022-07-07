#include "graphics/rendering/pipeline.hpp"
#include "graphics/device/logical.hpp"
#include "graphics/shader/module.hpp"
#include "graphics/rendering/render_pass.hpp"

using namespace A::Graphics;

Pipeline::Pipeline(
	Logical_device& logical_device, 
	Shader_module& vertex_shader, Shader_module& fragment_shader, 
	std::uint32_t viewport_width, std::uint32_t viewport_height,
	VkFormat colour_attachment_format,
	Render_pass& render_pass,
	std::uint32_t subpass_index
) {
    (void)colour_attachment_format;

    VkPipelineShaderStageCreateInfo shader_stages_create_info[] = {
	vertex_shader.get_shader_stage_creation_info(),
	fragment_shader.get_shader_stage_creation_info()
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{
	.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.vertexBindingDescriptionCount = 0,
	.pVertexBindingDescriptions = nullptr,
	.vertexAttributeDescriptionCount = 0,
	.pVertexAttributeDescriptions = nullptr
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{
	.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	.primitiveRestartEnable = VK_FALSE
    };

    VkViewport viewports[] = {{
	.x = 0,
	.y = 0,
	.width = (float)viewport_width,
	.height = (float)viewport_height,
	.minDepth = 0.0f,
	.maxDepth = 1.0f
    }};

    VkRect2D scissors[] = {{
	.offset = {.x = 0, .y = 0},
	.extent = {.width = viewport_width, .height = viewport_height}
    }};

    VkPipelineViewportStateCreateInfo viewport_state_create_info{
	.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.viewportCount = 1,
	.pViewports = viewports,
	.scissorCount = 1,
	.pScissors = scissors
    };
    
    VkPipelineRasterizationStateCreateInfo rasterisation_state_create_info{
	.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.depthClampEnable = VK_FALSE,
	.rasterizerDiscardEnable = VK_FALSE,
	.polygonMode = VK_POLYGON_MODE_FILL,
	.cullMode = VK_CULL_MODE_NONE,
	.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
	.depthBiasEnable = VK_FALSE,
	.depthBiasConstantFactor = {},
	.depthBiasClamp = {},
	.depthBiasSlopeFactor = {},
	.lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info{
	.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	.sampleShadingEnable = VK_FALSE,
	.minSampleShading = {},
	.pSampleMask = {},
	.alphaToCoverageEnable = {},
	.alphaToOneEnable = {}
    };

    // Per-target blending state for each individual color attachment (docs)
    VkPipelineColorBlendAttachmentState colour_blend_attachment_states[] = {{
	.blendEnable = VK_FALSE,
	.srcColorBlendFactor = {},
	.dstColorBlendFactor = {},
	.colorBlendOp = {},
	.srcAlphaBlendFactor = {},
	.dstAlphaBlendFactor = {},
	.alphaBlendOp = {},
	.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    }};

    VkPipelineColorBlendStateCreateInfo colour_blend_state_create_info{
	.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.logicOpEnable = VK_FALSE,
	.logicOp = {},
	.attachmentCount = 1,
	.pAttachments = colour_blend_attachment_states,
	.blendConstants = {}
    };

    layout = {logical_device};

    // Framebuffers and graphics pipelines are created based on a specific render pass object. 
    // They must only be used with that render pass object, or one compatible with it. (docs)
    VkGraphicsPipelineCreateInfo create_info[] = {{
	.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.stageCount = 2,
	.pStages = shader_stages_create_info,
	.pVertexInputState = &vertex_input_state_create_info,
	.pInputAssemblyState = &input_assembly_state_create_info,
	.pTessellationState = nullptr,
	.pViewportState = &viewport_state_create_info,
	.pRasterizationState = &rasterisation_state_create_info,
	.pMultisampleState = &multisample_state_create_info,
	.pDepthStencilState = nullptr,
	.pColorBlendState = &colour_blend_state_create_info,
	.pDynamicState = nullptr,
	.layout = layout,
	.renderPass = render_pass, // render pass object describing the environment in which the pipeline will be used (docs)
	.subpass = subpass_index, // index of the subpass in the render pass where this pipeline will be used (docs)
	.basePipelineHandle = VK_NULL_HANDLE,
	.basePipelineIndex = -1
    }};
    
    Handle pipelines[1];
    A_ASSERT(!vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, create_info, nullptr, pipelines));
    Unique_handle::operator=({pipelines[0], [&logical_device](Handle pipeline) { vkDestroyPipeline(logical_device, pipeline, nullptr); }});
}


Pipeline::~Pipeline() {
    Unique_handle::operator=({}); // Destroy the pipeline before destroying the layout
}

Pipeline::Layout::Layout(Logical_device& logical_device) {
    VkPipelineLayoutCreateInfo create_info{
	.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.setLayoutCount = 0,
	.pSetLayouts = nullptr,
	.pushConstantRangeCount = 0,
	.pPushConstantRanges = nullptr
    };

    Handle layout;
    A_ASSERT(!vkCreatePipelineLayout(logical_device, &create_info, nullptr, &layout)); 
    Unique_handle::operator=({layout, [&logical_device](Handle layout) { vkDestroyPipelineLayout(logical_device, layout, nullptr); }});
}
