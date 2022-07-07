#include "graphics/shader/module.hpp"
#include "graphics/shader/binary.hpp"
#include "graphics/device/logical.hpp"
#include "diagnostic/assert.hpp"

using namespace A::Graphics;


Shader_module::Shader_module(Logical_device& logical_device, Shader_binary const& binary):
    kind{binary.get_kind()}
{
    VkShaderModuleCreateInfo create_info{
	.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.codeSize = binary.get_data().size(),
	.pCode = reinterpret_cast<std::uint32_t const*>(binary.get_data().data())
    };
    
    Handle shader_module;
    A_ASSERT(!vkCreateShaderModule(logical_device, &create_info, nullptr, &shader_module)); 
    Unique_handle::reset(shader_module, [&logical_device](Handle shader_module) { vkDestroyShaderModule(logical_device, shader_module, nullptr); });

    shader_stage_creation_info = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.stage = kind,
	.module = get_handle(),
	.pName = "main",
	.pSpecializationInfo = nullptr
    };
}

Shader_kind const& Shader_module::get_kind() const noexcept {
    return kind;
}

VkPipelineShaderStageCreateInfo const& Shader_module::get_shader_stage_creation_info() const noexcept {
    return shader_stage_creation_info;
}
