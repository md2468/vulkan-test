#include "graphics/wsi/window.hpp"
#include "graphics/wsi/surface.hpp"
#include "graphics/state/instance.hpp"
#include "diagnostic/assert.hpp"
#include "diagnostic/exceptions.hpp"
#include "concurrency/main_thread.hpp"

using namespace A::Graphics;
namespace Exceptions = A::Exceptions::Graphics::Window;
namespace Events = A::Events::Graphics::Window;


std::string_view Window::Library::get_version_string() {
    // glfwGetVersionString() may be called before glfwInit(), but only from the main thread.
    // After glfwInit(), it is thread-safe.

    // Because the Library object is static, it may be initialised from any thread.
    // Thus, this string cannot be initialised in the constructor.

    A_ASSERT(is_initialised || std::this_thread::get_id() == main_thread_id);
    
    return {glfwGetVersionString()};
}

std::span<char const* const> Window::Library::get_required_instance_extensions() {
    static std::span<char const* const> result = []() -> std::span<char const* const> {
	initialise();
	std::uint32_t count = 0;
	char const* const* extensions = glfwGetRequiredInstanceExtensions(&count);
	return {extensions, count};
    }();

    return result;
}

void Window::Library::initialise() {
    // The library cannot be initialised in the constructor, because the library object
    // is static, so it is unspecified which thread would initialise it
    A_ASSERT(std::this_thread::get_id() == main_thread_id);
    if(is_initialised) return;

    // May be called before glfwInit(), necessary to catch glfwInit() errors
    glfwSetErrorCallback(
	+[](int error_code, char const* description) -> void {
	    (void)error_code;
	    critical_error("GLFW error: ", description);
	}
    );

    // If glfwInit() fails, it calls glfwTerminate() automatically
    is_initialised = (glfwInit() == GLFW_TRUE);
}

void Window::Library::poll_events() {
    // Must not be called from a callback
    // Should be called each frame after buffer swapping, even when there are no windows
    // ( https://www.glfw.org/docs/latest/input_guide.html#events )
    A_ASSERT(std::this_thread::get_id() == main_thread_id);
    A_ASSERT(is_initialised);

    glfwPollEvents();
}

void Window::Library::await_events() {
    glfwWaitEvents();
}

void Window::Library::terminate() {
    A_ASSERT(std::this_thread::get_id() == main_thread_id);
    if(!is_initialised) return;

    // Must not be called from a callback
    is_initialised = false;
    glfwTerminate();
}


Window::Window(int width, int height, Options_t options) {
    A_ASSERT(std::this_thread::get_id() == main_thread_id);
    Library::initialise();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if(auto resizable = options & Options::resizable ? GLFW_TRUE : GLFW_FALSE) glfwWindowHint(GLFW_RESIZABLE, resizable);
    if(auto maximized = options & Options::maximized ? GLFW_TRUE : GLFW_FALSE) glfwWindowHint(GLFW_MAXIMIZED, maximized);
    if(auto decorated = options & Options::decorated ? GLFW_TRUE : GLFW_FALSE) glfwWindowHint(GLFW_DECORATED, decorated);

    window = { 
	glfwCreateWindow(width, height, "A", nullptr, nullptr),
	+[](GLFWwindow* window) -> void { 
	    // The deleter is not called for nullptr
	    glfwDestroyWindow(window); 
	    if(--total_window_count == 0) Library::terminate();
	}
    };
    
    if(!window) {
	if(total_window_count == 0) Library::terminate();
	throw ::Exceptions::Library_error{"Window creation failed"};
    }

    ++total_window_count;

    // The size may be different than requested
    glfwGetFramebufferSize(window.get(), &framebuffer_size.width, &framebuffer_size.height);

    // The this pointer must not be const!
    glfwSetWindowUserPointer(window.get(), static_cast<void*>(this));

    glfwSetWindowCloseCallback(window.get(), +[](GLFWwindow* window) -> void { 
	Window* window_object = static_cast<Window*>(glfwGetWindowUserPointer(window));
	window_object->post_event(::Events::Close{});
    });
    glfwSetFramebufferSizeCallback(window.get(), +[](GLFWwindow* window, int width, int height) -> void {
	Window* window_object = static_cast<Window*>(glfwGetWindowUserPointer(window));
	window_object->framebuffer_size = { width, height };
	window_object->post_event(::Events::Resize{width, height});
    });

}

Window::~Window() {
    A_ASSERT(std::this_thread::get_id() == main_thread_id);
}

Surface Window::create_surface(Instance& instance) {
    Surface::Handle surface;
    A_ASSERT(!glfwCreateWindowSurface(instance, window.get(), nullptr, &surface)); 
    return Surface{surface, instance};
}

Window::Framebuffer_size const& Window::get_framebuffer_size() const noexcept {
    return framebuffer_size;
}

void Window::swap_buffers() {
    // https://www.glfw.org/docs/latest/input_guide.html#events
    // Event processing is done each frame after buffer swapping
    glfwSwapBuffers(window.get());
}

