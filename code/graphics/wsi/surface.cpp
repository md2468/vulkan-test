#include "graphics/wsi/surface.hpp"
#include "graphics/wsi/window.hpp"
#include "graphics/state/instance.hpp"

using namespace A::Graphics;


Surface::Surface(Handle handle, Instance& instance): 
    Unique_handle_interface{handle, [&instance](Handle surface) { vkDestroySurfaceKHR(instance, surface, nullptr); }}
{ }
