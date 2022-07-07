#pragma once

#include "events/emitter.hpp"
#include <vulkan/vulkan.h>
#include <type_traits>


namespace A::Events::Graphics::Validation_message_dispatcher {
    struct Message {
	using Severity_t = typename std::underlying_type<VkDebugUtilsMessageSeverityFlagBitsEXT>::type;
	using Type_t = typename std::underlying_type<VkDebugUtilsMessageTypeFlagBitsEXT>::type;

	struct Severity { enum Severity_e: Severity_t; };
	struct Type { enum Type_e: Type_t; };


	Severity_t severity;
	Type_t type_flags;
	char const* message_id_string; // Contains the portion of the Vulkan specification that has been violated
	std::int32_t message_id; // Internal number associated with the message being triggered
	char const* message; // Detailed info
    };


    // The values of VkDebugUtilsMessageSeverityFlagBitsEXT are sorted based on severity
    enum Message::Severity::Severity_e: Message::Severity_t {
	verbose = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
	info = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
	warning = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
	error = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
    };

    enum Message::Type::Type_e: Message::Type_t {
	general = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
	validation = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
	performance = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
    };

}

namespace A::Graphics {
    class Validation_message_dispatcher:
	public Event_emitter<
	    Events::Graphics::Validation_message_dispatcher::Message
	>
    {
    public:
	// Called by Vulkan, forwards the messages (as events) to associated Validation_message_dispatcher objects
	static VKAPI_ATTR VkBool32 VKAPI_CALL forward(
	    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	    VkDebugUtilsMessageTypeFlagsEXT message_types,
	    VkDebugUtilsMessengerCallbackDataEXT const* callback_data,
	    void* user_data
	);

	Validation_message_dispatcher();

	// Needed for constructing VkInstance and Debug_messenger objects associated with this Validation_message_dispatcher object
	VkDebugUtilsMessengerCreateInfoEXT const& get_debug_messenger_creation_info() const noexcept;

    private:
	VkDebugUtilsMessengerCreateInfoEXT debug_messenger_creation_info;

    };

}


