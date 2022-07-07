#include "graphics/shader/binary.hpp"

using namespace A::Graphics;

Shader_binary::Shader_binary(Shader_kind const& kind, Unique_handle<shaderc_compilation_result_t> compilation_result_):
    kind{kind},
    compilation_result{std::move(compilation_result_)},
    data{reinterpret_cast<std::byte const*>(shaderc_result_get_bytes(compilation_result)), shaderc_result_get_length(compilation_result)}
{ }

Shader_kind const& Shader_binary::get_kind() const noexcept {
    return kind;
}

std::span<std::byte const> const& Shader_binary::get_data() const noexcept {
    return data;
}

