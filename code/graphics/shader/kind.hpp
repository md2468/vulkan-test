#pragma once

#include <shaderc/shaderc.h>
#include <vulkan/vulkan.h>

namespace A::Graphics {
    struct Shader_kind {
	shaderc_shader_kind kind;
	VkShaderStageFlagBits stage;
	char const* name;
	inline constexpr operator shaderc_shader_kind() const noexcept { return kind; }
	inline constexpr operator VkShaderStageFlagBits() const noexcept { return stage; }
	inline constexpr operator char const*() const noexcept { return name; }
    };

    namespace Shader_kinds {
	inline constexpr Shader_kind vertex = {shaderc_vertex_shader, VK_SHADER_STAGE_VERTEX_BIT, "vertex"};
	inline constexpr Shader_kind fragment = {shaderc_fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT, "fragment"};
	inline constexpr Shader_kind compute = {shaderc_compute_shader, VK_SHADER_STAGE_COMPUTE_BIT, "compute"};
	inline constexpr Shader_kind geometry = {shaderc_geometry_shader, VK_SHADER_STAGE_GEOMETRY_BIT, "geometry"};
	inline constexpr Shader_kind tessellation_control = {shaderc_tess_control_shader, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "tessellation-control"};
	inline constexpr Shader_kind tessellation_evaluation = {shaderc_tess_evaluation_shader, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "tessellation-evaluation"};
    }
}
