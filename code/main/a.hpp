#pragma once

#include "io/console.hpp"
#include "graphics/wsi/window.hpp"
#include "graphics/state/instance.hpp"
#include "graphics/device/physical.hpp"
#include "graphics/device/logical.hpp"
#include "graphics/wsi/surface.hpp"
#include "graphics/wsi/swap_chain.hpp"
#include "graphics/shader/compiler.hpp"
#include "graphics/rendering/renderer.hpp"
#include "graphics/shader/module.hpp"


namespace A {

    inline void a(std::span<char const* const> args) {
	Io::Console::write_line("Called with ", args.size(), " argument(s)");
	Io::Console::write_line("GLFW version: ", Graphics::Window::Library::get_version_string());


	Graphics::Window window(800, 600);
	
	Graphics::Instance instance = {
	    Graphics::Instance::Validation::enabled, 
	    [](Events::Graphics::Validation_message_dispatcher::Message const& m) {
		if(m.severity == Events::Graphics::Validation_message_dispatcher::Message::Severity::error) critical_error(m.message);
		else if(m.severity >= Events::Graphics::Validation_message_dispatcher::Message::Severity::info) Io::Console::write_line(m.message);
		// else if(m.severity >= Events::Graphics::Validation_message_dispatcher::Message::Severity::verbose) Io::Console::write_line(m.message);
	    }
	};

	Graphics::Surface surface = window.create_surface(instance);
	auto all_physical_devices = Graphics::Physical_device::enumerate_all(instance);
	Graphics::Logical_device logical_device = {all_physical_devices[0], surface};
	Graphics::Swap_chain swap_chain(logical_device, surface, window);
	Graphics::Shader_compiler shader_compiler;
	
	auto vertex_shader_binary = shader_compiler.compile(
	    Graphics::Shader_kinds::vertex,
	    R"(
		#version 450
		layout(location = 0) out vec3 fragment_colour;
		vec2 positions[3] = vec2[](
		    vec2(0.0, -0.5),
		    vec2(0.5, 0.5),
		    vec2(-0.5, 0.5)
		);
		vec3 colours[3] = vec3[](
		    vec3(1.0, 0.0, 0.0),
		    vec3(0.0, 1.0, 0.0),
		    vec3(0.0, 0.0, 1.0)
		);
		void main() {
		    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
		    fragment_colour = colours[gl_VertexIndex];
		}
	    )"
	);

	auto fragment_shader_binary = shader_compiler.compile(
	    Graphics::Shader_kinds::fragment,
	    R"(
		#version 450
	
		layout(location = 0) in vec3 fragment_colour;
		layout(location = 0) out vec4 out_colour;
		void main() {
		    out_colour = vec4(fragment_colour, 1.0);
		}
	    )"
	);

	Graphics::Shader_module vertex_shader_module(logical_device, vertex_shader_binary);
	Graphics::Shader_module fragment_shader_module(logical_device, fragment_shader_binary);

	Graphics::Renderer renderer(logical_device, swap_chain, vertex_shader_module, fragment_shader_module);

	renderer.draw_frame();

	bool exit = false;

	window.add_event_callback([&](Events::Graphics::Window::Close const& event) {
	    (void)event;
	    exit = true;
	});

	while(!exit) {
	    Graphics::Window::Library::await_events();
	}

	logical_device.wait_idle();

	    
	}
}
