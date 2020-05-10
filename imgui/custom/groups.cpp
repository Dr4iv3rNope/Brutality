#include "groups.hpp"

#include "../imgui.h"

#include "../../util/xorstr.hpp"
#include "../../util/strings.hpp"

bool ImGui::Custom::BeginGroup(const std::string& label, bool& is_open, GroupFlags flags) noexcept
{
	const bool returnOpened = flags.HasFlag(GroupFlags_ReturnIsOpened);

	bool changed = false;

	if (is_open)
	{
		ImGui::TextUnformatted(UTIL_CXOR("Hide "));
		ImGui::SameLine();

		if (ImGui::Selectable(label.c_str(), true))
		{
			is_open = false;			
			if (!returnOpened)
				changed = true;
		}

		ImGui::BeginChild(UTIL_FORMAT(UTIL_XOR("CUSTOM##GROUP_") << label).c_str());

		if (returnOpened)
			return true;
	}
	else
	{
		ImGui::TextUnformatted(UTIL_CXOR("Show "));
		ImGui::SameLine();

		if (ImGui::Selectable(label.c_str(), true))
		{
			is_open = true;
			changed = !returnOpened;
		}

		if (returnOpened)
			return false;
	}

	return changed;
}

void ImGui::Custom::EndGroup() noexcept
{
	ImGui::EndChild();
}
