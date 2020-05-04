#pragma once
#include "../imgui.h"

#include <optional>

namespace ImGui
{
	namespace Custom
	{
		template <typename T, ImGuiDataType DataType>
		inline bool InputAny(const char* label, T& value,
							 const std::optional<T>& min = {},
							 const std::optional<T>& max = {}) noexcept
		{
			if (min.has_value() && max.has_value())
				return ImGui::SliderScalar(label, DataType, &value, &*min, &*max);
			else
				return ImGui::InputScalar(label, DataType, &value);
		}
	}
}
