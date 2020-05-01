#pragma once
#include <atomic>
#include <functional>

#include "util/debug/errors.hpp"

namespace Main
{	
	using ShutdownActionFn = std::function<void()>;

	struct ShutdownElement;
	extern void AddToShutdown(ShutdownElement* element) noexcept;

	struct ShutdownElement final
	{
		std::atomic_bool busy { false };
		ShutdownActionFn action;
		std::wstring name;

		inline ShutdownElement(const std::wstring& name,
							   ShutdownActionFn action) noexcept
			: action { action }, name { name }
		{
			AddToShutdown(this);
		}
	};

	class ShutdownGuard final
	{
	private:
		ShutdownElement* _element;

	public:
		ShutdownGuard(ShutdownElement*& element) noexcept;

		inline ~ShutdownGuard() noexcept
		{
			_element->busy = false;
		}
	};

	extern void Shutdown() noexcept;
	extern bool IsInShutdown() noexcept;

	extern void DrawMenu() noexcept;
}

#define CREATE_SHUTDOWN_HOOK_GUARD(name, hookPtr) \
	static Main::ShutdownElement* __shutdown_element; \
	if (!__shutdown_element) { \
		UTIL_CHECK_ALLOC(__shutdown_element = new Main::ShutdownElement(UTIL_XOR(name), [] () \
			{ delete hookPtr; })) }

#define MAKE_BUSY_SHUTDOWN_GUARD \
	auto __shutdown_guard = Main::ShutdownGuard(__shutdown_element);
