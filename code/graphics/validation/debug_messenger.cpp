#include "graphics/validation/debug_messenger.hpp"
#include "graphics/validation/message_dispatcher.hpp"
#include "graphics/state/instance.hpp"

using namespace A::Graphics;


Debug_messenger::Debug_messenger(Instance& instance, Validation_message_dispatcher& message_dispatcher) {
    Handle debug_messenger;
    auto ptr_vkCreateDebugUtilsMessengerEXT = instance.get_function_address<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");
    A_ASSERT(!ptr_vkCreateDebugUtilsMessengerEXT( instance, &message_dispatcher.get_debug_messenger_creation_info(), nullptr, &debug_messenger));

    Unique_handle::reset(
	debug_messenger,
	[&instance](Handle debug_messenger) { 
	    auto ptr_vkDestroyDebugUtilsMessengerEXT = instance.get_function_address<PFN_vkDestroyDebugUtilsMessengerEXT>("vkDestroyDebugUtilsMessengerEXT");
	    ptr_vkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
	}
    );

}
