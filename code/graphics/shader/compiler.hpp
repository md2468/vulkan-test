#pragma once

#include <shaderc/shaderc.h>
#include <string_view>
#include "graphics/shader/binary.hpp"
#include "graphics/shader/kind.hpp"
#include "utility/unique_handle.hpp"

namespace A::Graphics {
    class Shader_compiler {
    public:
	Shader_compiler();
	
	Shader_binary compile(Shader_kind const& kind, std::string_view source);

    private:
	Unique_handle<shaderc_compiler_t> compiler;
	Unique_handle<shaderc_compile_options_t> compile_options;

    };
}
