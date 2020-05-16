#pragma once
#include "gmod/gmod.hpp"

#include <string>

namespace Shutdown
{
	struct Element;
}

namespace Main
{
	extern void Initialize() noexcept;

	extern void Shutdown() noexcept;
	extern bool IsInShutdown() noexcept;

	extern bool IsLocalPathInitialized();
	extern const std::wstring& GetLocalPath();
}

extern GameInterfaces* interfaces;
extern GameHooks* hooks;
