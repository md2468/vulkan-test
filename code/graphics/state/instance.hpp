#pragma once

#include "utility/unique_handle_interface.hpp"
#include "graphics/validation/message_dispatcher.hpp"
#include "graphics/validation/debug_messenger.hpp"
#include <vulkan/vulkan.h>
#include <functional>

namespace A::Graphics {
    class Instance: public Unique_handle_interface<VkInstance> {
    public:
	enum class Validation { enabled, disabled };

	// Validation callback must be registered in the constructor in order to catch messages emitted during the construction of VkInstance
	Instance(
	    Validation validation, 
	    std::function<void(Events::Graphics::Validation_message_dispatcher::Message const&)> validation_message_callback = 
		+[](Events::Graphics::Validation_message_dispatcher::Message const&) { }
	);

	~Instance();

	template<
	    typename Function_ptr, 
	    typename std::enable_if<
		std::is_function<typename std::remove_pointer<Function_ptr>::type>::value,
		int>::type = 0
	    >
	Function_ptr get_function_address(char const* function_name) {
	    Function_ptr pointer = reinterpret_cast<Function_ptr>(vkGetInstanceProcAddr(*this, function_name));
	    A_ASSERT(pointer);
	    return pointer;
	}


    private:
	bool validation_enabled;
	Validation_message_dispatcher validation_message_dispatcher;
	Debug_messenger debug_messenger;

    };
}
