#pragma once
#include <functional>

namespace ImGui
{
	namespace Custom
	{
		extern bool FlagInput(
			const char* label,
			unsigned int& flags,
			std::function<bool(unsigned int bit, std::string& desc)>,
			int bit_count
		) noexcept;
	}
}
