#include "graphics/shader/compiler.hpp"
#include "graphics/shader/compilation_status.hpp"
#include "diagnostic/assert.hpp"
#include "diagnostic/critical_error.hpp"

using namespace A::Graphics;


Shader_compiler::Shader_compiler() {
    shaderc_compiler_t compiler_ = shaderc_compiler_initialize();
    shaderc_compile_options_t compile_options_ = shaderc_compile_options_initialize();

    A_ASSERT(compiler_);
    A_ASSERT(compile_options_);

    compiler = {compiler_, [](decltype(compiler)::Handle compiler) { shaderc_compiler_release(compiler); }};
    compile_options = {compile_options_, [](decltype(compile_options)::Handle compile_options) { shaderc_compile_options_release(compile_options); }};
    
    shaderc_compile_options_set_source_language(compile_options, shaderc_source_language_glsl);
    shaderc_compile_options_set_optimization_level(compile_options, shaderc_optimization_level_performance);
    shaderc_compile_options_set_target_env(compile_options, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
}

Shader_binary Shader_compiler::compile(Shader_kind const& kind, std::string_view source) {
    // The input_file_name argument used as a tag to identify the source string in cases like emitting error messages. 
    // It doesn't have to be a 'file name'.
    Unique_handle<shaderc_compilation_result_t> compilation_result{
	shaderc_compile_into_spv(
	    compiler, source.data(), source.length(), kind, kind, "main", compile_options
	),
	[](decltype(compilation_result)::Handle compilation_result) { shaderc_result_release(compilation_result); }
    };
    
    // Some internal error, not a compilation error
    A_ASSERT(compilation_result);

    shaderc_compilation_status compilation_status = shaderc_result_get_compilation_status(compilation_result);
    std::size_t error_count = shaderc_result_get_num_errors(compilation_result);
    std::size_t warning_count = shaderc_result_get_num_warnings(compilation_result);

    if(compilation_status != shaderc_compilation_status_success || warning_count || error_count) {
	critical_error(
	    "Shader compilation failed (", shader_compilation_status_string(compilation_status),
	    ") with ", warning_count, " warning(s) and ", error_count, " error(s):\n", 
	    shaderc_result_get_error_message(compilation_result) // Error and warning messages
	);
    } else {
	return {kind, std::move(compilation_result)};
    }
}
