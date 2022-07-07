#pragma once

#include "utility/unique_handle_interface.hpp"
#include "graphics/command/queue.hpp"
#include <vulkan/vulkan.h>
#include <cstdint>


namespace A::Graphics {
    class Logical_device: public Unique_handle_interface<VkDevice> {
    public:
	Logical_device(class Physical_device& physical_device, class Surface& surface);
	
	class Physical_device& get_physical_device() noexcept;

	Queue const& get_graphics_queue() const noexcept;
	Queue const& get_presentation_queue() const noexcept;

	void submit_drawing_commands(class Command_buffer& command_buffer, class Semaphore& wait_sem, class Semaphore& signal_sem);
	void present(class Swap_chain& swap_chain, std::uint32_t swap_chain_image_index, class Semaphore& wait_sem);

	void wait_idle();

    private:
	class Physical_device* physical_device;

	// These might be the same queue
	Queue graphics_queue;
	Queue presentation_queue;

    };
}
