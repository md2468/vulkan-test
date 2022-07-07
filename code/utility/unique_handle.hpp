#pragma once

#include <optional>
#include <functional>
#include "diagnostic/assert.hpp"

// RAII wrapper for an owning, non-nullable handle type using pass-by-value semantics

namespace A {
    template<typename Handle_t>
    class Unique_handle {
    public:
	using Handle = Handle_t;
	inline static constexpr auto noop_deleter = [](auto) -> void { };

	Unique_handle() = default;

	constexpr Unique_handle(Handle new_handle, std::function<void(Handle)> new_deleter) noexcept:
	    handle{new_handle},
	    deleter{std::move(new_deleter)}
	{ }

	constexpr ~Unique_handle() noexcept {
	    if(has_value()) deleter(*handle);
	}

	Unique_handle(Unique_handle const&) = delete;
	Unique_handle& operator=(Unique_handle const&) = delete;

	constexpr Unique_handle(Unique_handle&& other) noexcept:
	    handle{std::exchange(other.handle, std::nullopt)},
	    deleter{std::exchange(other.deleter, nullptr)}
	{ }

	constexpr Unique_handle& operator=(Unique_handle&& other) noexcept {
	    if(this == &other) return *this;
	    if(has_value()) deleter(*handle);
	    handle = std::exchange(other.handle, std::nullopt);
	    deleter = std::exchange(other.deleter, nullptr);
	    return *this;
	}

	constexpr void reset(Handle new_handle, std::function<void(Handle)> new_deleter) noexcept {
	    if(has_value()) deleter(*handle);
	    handle = new_handle;
	    deleter = std::move(new_deleter);
	}

	constexpr void reset() noexcept {
	    if(has_value()) deleter(*handle);
	    handle = std::nullopt;
	    deleter = nullptr;
	}

	// Constness is shallow - it is possible to modify the referenced object through
	// a const Unique_handle object
	constexpr Handle get_handle() const noexcept {
	    A_ASSERT(has_value());
	    return *handle;
	}

	constexpr operator Handle() const noexcept {
	    return get_handle();
	}
	
	constexpr bool has_value() const noexcept {
	    return handle.has_value();
	}


    private:
	std::optional<Handle> handle;
	std::function<void(Handle)> deleter;

    };
}
