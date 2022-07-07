#pragma once

#include <type_traits>
#include <functional>
#include <memory>
#include <list>
#include <ranges>

/* This file should be mostly compatible with C++17 */

namespace A {
    namespace Internal::Event_emitter_impl {
	namespace Constraints {
	    template<typename T>
	    struct No_cvref: 
		std::conditional<
		    std::is_same<
			T,
			typename std::remove_cv<typename std::remove_reference<T>::type>::type
		    >::value,
		    std::true_type,
		    std::false_type
		>::type
	    { };

	    template<typename T>
	    struct Decayed:
		std::conditional<
		    std::is_same<
			T,
			typename std::decay<T>::type
		    >::value,
		    std::true_type,
		    std::false_type
		>::type
	    { };

	    template<typename Void, typename Callable, typename... Args>
	    struct Invocable_impl: std::false_type { };

	    template<typename Callable, typename... Args>
	    struct Invocable_impl<
		std::void_t<decltype(
		    std::invoke(std::declval<Callable&&>(), std::declval<Args&&>()...)
		)>, 
		Callable, 
		Args...
	    >: std::true_type { };

	    template<typename Callable, typename... Args>
	    using Invocable = Invocable_impl<void, Callable, Args...>;


	} // namespace Constraints


	template<
	    typename Argument, 
	    typename std::enable_if<
		Constraints::No_cvref<Argument>::value, 
	    int>::type = 0
	>
	class Callable_base {
	public:
	    constexpr virtual void invoke(Argument& arg) = 0;
	    constexpr virtual void invoke(Argument const& arg) = 0;
	    constexpr virtual void invoke(Argument&& arg) = 0;
	    constexpr virtual void invoke(Argument const&& arg) = 0;

	    virtual ~Callable_base() noexcept = default;
	};


	template<
	    typename Argument, 
	    typename Stored_callable,
	    typename std::enable_if<
		std::conjunction<
		    Constraints::No_cvref<Argument>,
		    Constraints::Decayed<Stored_callable>
		>::value,
	    int>::type = 0
	>
	class Callable: public Callable_base<Argument> {
	public:
	    constexpr virtual void invoke(Argument& arg) override {
		invoke_impl(static_cast<decltype(arg)&&>(arg));
	    }

	    constexpr virtual void invoke(Argument const& arg) override {
		invoke_impl(static_cast<decltype(arg)&&>(arg));
	    }

	    constexpr virtual void invoke(Argument&& arg) override {
		invoke_impl(static_cast<decltype(arg)&&>(arg));
	    }

	    constexpr virtual void invoke(Argument const&& arg) override {
		invoke_impl(static_cast<decltype(arg)&&>(arg));
	    }

	    constexpr virtual ~Callable() noexcept override = default;

	private:
	    template<typename Argument_, typename T>
	    friend constexpr auto make_callable(T&& callable);

	    template<typename T>
	    constexpr explicit Callable(T&& callable_):
		callable{static_cast<T&&>(callable_)}
	    { }


	    template<typename T, typename std::enable_if<Constraints::Invocable<Stored_callable&, T&&>::value, int>::type = 0>
	    constexpr void invoke_impl(T&& arg) {
		std::invoke(callable, static_cast<T&&>(arg));
	    }

	    template<typename T, typename std::enable_if<!Constraints::Invocable<Stored_callable&, T&&>::value, int>::type = 0>
	    [[noreturn]] constexpr void invoke_impl(T&& arg) {
		std::abort();
	    }


	    Stored_callable callable;
	};


	template<typename Argument, typename T>
	constexpr auto make_callable(T&& callable) {
	    return Callable<Argument, typename std::decay<T>::type>{static_cast<T&&>(callable)};
	}


	template<
	    typename Argument, 
	    typename std::enable_if<
		Constraints::No_cvref<Argument>::value,
	    int>::type = 0
	    >
	class Callback {
	public:
	    template<typename T>
	    constexpr explicit Callback(T&& callable):
		// new must be used here to help with deduction
		callable{new Callable{make_callable<Argument>(static_cast<T&&>(callable))}}
	    { }

	    template<typename T>
	    constexpr void invoke(T&& argument) {
		callable->invoke(static_cast<T&&>(argument));
	    }

	    Callback(Callback&&) = default;
	    Callback& operator=(Callback&&) = default;

	private:
	    std::unique_ptr<Callable_base<Argument>> callable;

	};


	template<typename T>
	using Remove_cvref = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

	// Function type consists of: https://en.cppreference.com/w/cpp/language/functions
	// &&-qualified operator()s need not be considered, because the invoked callable will be an l-value
	// Exception specification is part of the function type
	template<typename...> struct Callback_ctad_helper;
	template<typename A, typename R> struct Callback_ctad_helper<R(*)(A) > { using Arg = Remove_cvref<A>; };
	template<typename A, typename R> struct Callback_ctad_helper<R(*)(A) noexcept > { using Arg = Remove_cvref<A>; };
	template<typename C, typename A, typename R> struct Callback_ctad_helper<R(C::*)(A) > { using Arg = Remove_cvref<A>; };
	template<typename C, typename A, typename R> struct Callback_ctad_helper<R(C::*)(A) & > { using Arg = Remove_cvref<A>; };
	template<typename C, typename A, typename R> struct Callback_ctad_helper<R(C::*)(A) const > { using Arg = Remove_cvref<A>; };
	template<typename C, typename A, typename R> struct Callback_ctad_helper<R(C::*)(A) const & > { using Arg = Remove_cvref<A>; };
	template<typename C, typename A, typename R> struct Callback_ctad_helper<R(C::*)(A) noexcept> { using Arg = Remove_cvref<A>; };
	template<typename C, typename A, typename R> struct Callback_ctad_helper<R(C::*)(A) & noexcept> { using Arg = Remove_cvref<A>; };
	template<typename C, typename A, typename R> struct Callback_ctad_helper<R(C::*)(A) const noexcept> { using Arg = Remove_cvref<A>; };
	template<typename C, typename A, typename R> struct Callback_ctad_helper<R(C::*)(A) const & noexcept> { using Arg = Remove_cvref<A>; };
	template<typename C> struct Callback_ctad_helper<C> { using Arg = Callback_ctad_helper<decltype(&C::operator())>::Arg; };

	// Deduction guide:
	template<typename T>
	Callback(T) -> Callback<typename Callback_ctad_helper<T>::Arg>;


	template<
	    typename Event, 
	    typename std::enable_if<
		Constraints::No_cvref<Event>::value, 
	    int>::type = 0
	    >
	class Event_emitter_base {
	protected:
	    using Callback_handle = std::list<Callback<Event>>::const_iterator;

	private:
	    // std::list never invalidates iterators
	    std::list<Callback<Event>> callbacks;
	    std::vector<Callback_handle> callbacks_for_removal;

	protected:
	    Event_emitter_base() = default;

	    // This could be enabled in the future if needed
	    Event_emitter_base(Event_emitter_base const&) = delete;
	    Event_emitter_base& operator= (Event_emitter_base const&) = delete;
	    
	    // After a move, the iterators of std::list and std::vector remain valid, but refer to elements that are now in *this
	    Event_emitter_base(Event_emitter_base&&) = default;
	    Event_emitter_base& operator= (Event_emitter_base&&) = default;


	    // Taking Callback<Event> allows for overload resolution to choose the right base in Event_emitter
	    constexpr Callback_handle add_callback(Callback<Event>&& callback) {
		return callbacks.emplace(std::cend(callbacks), std::move(callback));
	    }

	    constexpr void remove_callback(Callback_handle handle) {
		callbacks_for_removal.emplace_back(handle);
	    }

	    // Accept only non-const or const l-values
	    template<typename T>
	    constexpr void post_event(T& event) {
		// This allows callbacks to register and deregister other callbacks or themselves
		// If that is done, changes are only visible in future calls to post_event()

		for(auto callback : callbacks_for_removal) callbacks.erase(callback);
		callbacks_for_removal.clear();

		// This will only iterate over the original range of callbacks, whether or not this range changes during iteration
		for(auto& callback : callbacks | std::views::take(std::ssize(callbacks))) callback.invoke(event);
	    }

	};


	// Pack must be specified last, but constraints depend on the pack
	template<bool constraints, typename... Events>
	class Event_emitter;

	template<typename... Events>
	class Event_emitter<true, Events...>: private Event_emitter_base<Events>... {
	public:
	    // Event type deduced
	    template<typename T>
	    decltype(auto) add_event_callback(T&& callback) {
		return add_callback(Callback{static_cast<T&&>(callback)});
	    }

	    // Event type provided explicitly
	    template<typename Event, typename T>
	    decltype(auto) add_event_callback(T&& callback) {
		return add_callback(Callback<Event>{static_cast<T&&>(callback)});
	    }

	    template<typename T>
	    void remove_event_callback(T callback_handle) {
		remove_callback(callback_handle);
	    }


	protected:
	    Event_emitter() = default;

	    Event_emitter(Event_emitter const&) = delete;
	    Event_emitter& operator=(Event_emitter const&) = delete;
	    Event_emitter(Event_emitter&&) = default;
	    Event_emitter& operator=(Event_emitter&&) = default;

	    template<typename T>
	    void post_event(T&& event) {
		// Forward event as l-value
		Event_emitter_base<Remove_cvref<T>>::post_event(event);
	    }


	private:
	    using Event_emitter_base<Events>::add_callback...;
	    using Event_emitter_base<Events>::remove_callback...;

	};
	

	template<typename... Events>
	using Make_event_emitter = Event_emitter<
	    std::conjunction<
		Constraints::No_cvref<Events>...
	    >::value,
	    Events...
	>;


    } // namespace Internal::Event_emitter_impl

    template<typename... Events>
    using Event_emitter = Internal::Event_emitter_impl::Make_event_emitter<Events...>;


}
