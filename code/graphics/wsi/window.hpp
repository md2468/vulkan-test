#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string_view>
#include <memory>
#include <span>
#include "diagnostic/exceptions.hpp"
#include "events/emitter.hpp"

namespace A::Exceptions::Graphics::Window {
    struct Library_error: public Exception {
	using Exception::Exception;
    };
}

namespace A::Events::Graphics::Window {
    struct Close { };
    struct Resize { int framebuffer_width_px, framebuffer_height_px; };
}

namespace A::Graphics {
    class Window:
	public Event_emitter<
	    Events::Graphics::Window::Close,
	    Events::Graphics::Window::Resize
	>
    {
    private:
	struct Framebuffer_size;

    public:
	using Options_t = unsigned;
	struct Options { enum Options_e: Options_t; };

	Window(int width, int height, Options_t = 0);
	~Window();

	class Surface create_surface(class Instance& instance);

	// The size is in pixels
	Framebuffer_size const& get_framebuffer_size() const noexcept;
	void swap_buffers();


	class Library {
	    friend Window;

	public:
	    static std::string_view get_version_string();
	    static std::span<char const* const> get_required_instance_extensions();
	    static void poll_events();
	    static void await_events();

	private:
	    static void initialise();
	    static void terminate();

	    inline static bool is_initialised = false;
	};

    
    private:
	inline static unsigned total_window_count = 0;

	using Window_ptr = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>;

	// The default ctor is disabled if deleter is a pointer
	// The deleter is not called for nullptr (unlike std::shared_ptr)
	Window_ptr window = { nullptr, nullptr };
	struct Framebuffer_size { int width, height; } framebuffer_size = {};

    };


    enum Window::Options::Options_e: Window::Options_t {
	none      = 0,
	resizable = 1 << 0,
	maximized = 1 << 1,
	decorated = 1 << 2,
    };

}
