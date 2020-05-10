#include "flags.h"

#include "../imgui.h"

#include "../../util/xorstr.hpp"

bool ImGui::Custom::FlagInput(
	const char* label,
	unsigned int& flags,
	std::function<bool(unsigned int bit, std::string& desc)> callback,
	int bit_count
) noexcept
{
	bool changed = false;

	if (ImGui::Button(label, ImVec2(-1.f, 0.f)))
		ImGui::OpenPopup(UTIL_CXOR("CUSTOM##FLAGINPUT"));

	if (ImGui::BeginPopup(UTIL_CXOR("CUSTOM##FLAGINPUT")))
	{
		ImGui::TextUnformatted(label);
		ImGui::Separator();

		ImGui::BeginChild(UTIL_CXOR("CUSTOM##FLAGLIST"), ImVec2(200, 150), true);

		for (int bit = 0; bit < bit_count; bit++)
		{
			ImGui::PushID(bit);

			std::string desc = UTIL_SXOR("*NO DESCRIPTION*");

			if (callback(bit, desc))
				if (ImGui::Selectable(desc.c_str(), &flags))
				{
					if (flags & (1 << bit))
						flags |= (1 << bit);
					else
						flags &= ~(1 << bit);

					changed = true;
				}

			ImGui::PopID(); // bit
		}

		ImGui::EndChild(); // CUSTOM##FLAGLIST
		ImGui::EndPopup();
	}

	return changed;
}
