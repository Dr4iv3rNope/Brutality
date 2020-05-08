#pragma once
#include <string>

namespace Shutdown
{
	struct Element;
}

namespace Main
{
	extern void Initialize() noexcept;
	extern void Shutdown() noexcept;

	extern bool IsLocalPathInitialized();
	extern const std::wstring& GetLocalPath();

	extern void AddToShutdown(Shutdown::Element* element) noexcept;
	extern void Shutdown() noexcept;
	extern bool IsInShutdown() noexcept;
}
