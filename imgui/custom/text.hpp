#pragma once
#include <string>

namespace ImGui
{
	namespace Custom
	{
		// return true if text has been copied
		extern bool CopiableText(const char* format, ...) noexcept;

		extern std::string ToTextBoolean(bool condition) noexcept;
	}
}
