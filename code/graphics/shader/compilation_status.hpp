#pragma once

#include <shaderc/shaderc.h>

namespace A::Graphics {
    inline constexpr char const* shader_compilation_status_string(shaderc_compilation_status status) noexcept {
	switch(status) {
	    case shaderc_compilation_status_success: return "success";
	    case shaderc_compilation_status_invalid_stage: return "invalid stage";
	    case shaderc_compilation_status_compilation_error: return "compilation error";
	    case shaderc_compilation_status_internal_error: return "internal error";
	    case shaderc_compilation_status_null_result_object: return "null result object";
	    case shaderc_compilation_status_invalid_assembly: return "invalid assembly";
	    case shaderc_compilation_status_validation_error: return "validation error";
	    case shaderc_compilation_status_transformation_error: return "transformation error";
	    case shaderc_compilation_status_configuration_error: return "configuration error";
	    default: return "?";
	}
    }
}
