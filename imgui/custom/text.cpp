#include "text.hpp"

#include "../imgui.h"

#include "../../util/xorstr.hpp"

#include <cstdarg>

bool ImGui::Custom::CopiableText(const char* format, ...) noexcept
{
	static constexpr int BUFFER_SIZE { 1 << 13 };
	char buffer[BUFFER_SIZE];

	{
		std::va_list args;
		va_start(args, format);
		vsprintf_s(buffer, format, args);
		va_end(args);
	}

	ImGui::TextUnformatted(buffer);

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(UTIL_CXOR("Click to copy text"));
		ImGui::EndTooltip();
	}

	if (ImGui::IsItemClicked())
	{
		ImGui::SetClipboardText(buffer);

		return true;
	}

	return false;
}

std::string ImGui::Custom::ToTextBoolean(bool condition) noexcept
{
	return condition ? UTIL_SXOR("true") : UTIL_SXOR("false");
}
