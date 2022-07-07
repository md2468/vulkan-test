#pragma once

#include "utility/unique_handle_interface.hpp"
#include <vulkan/vulkan.h>


namespace A::Graphics {
    class Debug_messenger: public Unique_handle_interface<VkDebugUtilsMessengerEXT> {
    public:
	Debug_messenger() = default;
	Debug_messenger(class Instance& instance, class Validation_message_dispatcher& message_dispatcher);
	
    };
}
