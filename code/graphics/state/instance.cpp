#include "graphics/state/instance.hpp"
#include "graphics/wsi/window.hpp"
#include <array>

using namespace A::Graphics;


Instance::Instance(
	Validation validation, 
	std::function<void(Events::Graphics::Validation_message_dispatcher::Message const&)> validation_message_callback
): 
    validation_enabled{validation == Validation::enabled}
{
    VkApplicationInfo application_info{
	.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
	.pNext = nullptr,
	.pApplicationName = nullptr,
	.applicationVersion = 0,
	.pEngineName = nullptr,
	.engineVersion = 0,
	.apiVersion = VK_API_VERSION_1_2
    };

    std::array validation_layers{
	+"VK_LAYER_KHRONOS_validation"
    };
    
    std::vector<char const*> all_extensions;
    std::span<char const* const> glfw_extensions = Window::Library::get_required_instance_extensions();
    for(char const* e : glfw_extensions) all_extensions.emplace_back(e);
    if(validation_enabled) all_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    validation_message_dispatcher.add_event_callback(std::move(validation_message_callback));

    // To capture events that occur while creating or destroying an instance an application 
    // can link a VkDebugUtilsMessengerCreateInfoEXT structure to the pNext element of the 
    // VkInstanceCreateInfo structure given to vkCreateInstance.
    // https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/chap50.html#_examples_9

    VkInstanceCreateInfo create_info{
	.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	.pNext = validation_enabled ? &validation_message_dispatcher.get_debug_messenger_creation_info() : nullptr,
	.flags = 0,
	.pApplicationInfo = &application_info,
	.enabledLayerCount = std::uint32_t(validation_enabled ? validation_layers.size() : 0),
	.ppEnabledLayerNames = validation_enabled ? validation_layers.data() : nullptr,
	.enabledExtensionCount = (std::uint32_t)all_extensions.size(),
	.ppEnabledExtensionNames = all_extensions.data()
    };

    Handle instance{};
    A_ASSERT(!vkCreateInstance(&create_info, nullptr, &instance));
    Unique_handle::reset(instance, [](Handle instance) -> void { vkDestroyInstance(instance, nullptr); });

    if(validation_enabled) debug_messenger = {*this, validation_message_dispatcher};
}

Instance::~Instance() {
    // Instance must be alive during the destruction of debug_messenger
    if(validation_enabled) debug_messenger = {};
    Unique_handle::reset();
    // validation_message_dispatcher must be destroyed after VkInstance, because destroying VkInstance may emit debug messages
}
