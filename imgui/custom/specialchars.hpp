#pragma once
#include <string>

namespace ImGui
{
	namespace Custom
	{
		extern std::string FormatSpecialChars(std::string str) noexcept;

		// shows tooltip
		extern void ShowSpecialChars() noexcept;
	}
}
