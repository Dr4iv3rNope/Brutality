#pragma once
#include "../imgui.h"

#include <optional>

namespace ImGui
{
	namespace Custom
	{
		extern bool ColorPicker(const char* label, ImVec4& color,
								const std::optional<ImVec4>& default_color = {}) noexcept;
	}
}
