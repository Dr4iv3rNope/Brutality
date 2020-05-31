#pragma once
#include <cstdint>

namespace UI
{	
	extern std::uint32_t GetPreviewColor(std::uint32_t div_power = 1) noexcept;

	extern void Initialize();
	extern void Shutdown() noexcept;
	
	extern void Draw();
	extern void Reset();
}
