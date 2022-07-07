#include "graphics/validation/message_dispatcher.hpp"

using namespace A::Graphics;
namespace Events = A::Events::Graphics::Validation_message_dispatcher;


VKAPI_ATTR VkBool32 VKAPI_CALL Validation_message_dispatcher::forward(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_types,
	VkDebugUtilsMessengerCallbackDataEXT const* callback_data,
	void* user_data
) {
    // This function must not make calls to any Vulkan commands (VUID-PFN_vkDebugUtilsMessengerCallbackEXT-None-04769)

    Validation_message_dispatcher* message_dispatcher_object = static_cast<Validation_message_dispatcher*>(user_data);

    ::Events::Message message{
	.severity = message_severity,
	.type_flags = message_types,
	.message_id_string = callback_data->pMessageIdName,
	.message_id = callback_data->messageIdNumber,
	.message = callback_data->pMessage
    };

    message_dispatcher_object->post_event(std::as_const(message));

    // Must always return false
    return VK_FALSE;
}


Validation_message_dispatcher::Validation_message_dispatcher():
    debug_messenger_creation_info{
	.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
	.pNext = nullptr,
	.flags = 0,
	.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    | 
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
	.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  | 
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
	.pfnUserCallback = &Validation_message_dispatcher::forward,
	.pUserData = static_cast<void*>(this)
    }
{ }


VkDebugUtilsMessengerCreateInfoEXT const& Validation_message_dispatcher::get_debug_messenger_creation_info() const noexcept {
    return debug_messenger_creation_info;
}
