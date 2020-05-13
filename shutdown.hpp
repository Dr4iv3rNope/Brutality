#pragma once
#include "main.hpp"

#include <atomic>
#include <functional>

#include "util/debug/assert.hpp"
#include "util/debug/errors.hpp"

namespace Shutdown
{	
	using ActionFn = std::function<void()>;

	struct Element final
	{
		std::atomic_bool busy { false };
		ActionFn action;
		std::wstring name;

		inline Element(const std::wstring& name, ActionFn action = nullptr) noexcept
			: action { action }, name { name }
		{
			Main::AddToShutdown(this);
		}
	};

	class Guard final
	{
	private:
		Element* _element;

	public:
		inline Guard(Element*& element) noexcept
		{
			UTIL_DEBUG_ASSERT(element);

			_element = element;
		}

		inline ~Guard() noexcept
		{
			_element->busy = false;
		}
	};
}

#define SHUTDOWN_HOOK_GUARD_CALLBACK(name, ...) \
	static Shutdown::Element* __shutdown_element; \
	if (!__shutdown_element) { \
		UTIL_CHECK_ALLOC(__shutdown_element = new Shutdown::Element(UTIL_XOR(name), [] () \
			{ __VA_ARGS__ })) }

#define SHUTDOWN_HOOK_GUARD(name) \
	static Shutdown::Element* __shutdown_element; \
	if (!__shutdown_element) { \
		UTIL_CHECK_ALLOC(__shutdown_element = new Shutdown::Element(UTIL_XOR(name))) }

#define MAKE_BUSY_SHUTDOWN_GUARD \
	auto __shutdown_guard = Shutdown::Guard(__shutdown_element);
