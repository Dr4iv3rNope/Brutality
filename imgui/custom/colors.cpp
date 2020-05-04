#include "colors.hpp"

#include "../../util/xorstr.hpp"
#include "../../util/strings.hpp"

static ImVec4& CopiedColor() noexcept
{
	static ImVec4 color(1.f, 1.f, 1.f, 1.f);

	return color;
}

bool ImGui::Custom::ColorPicker(const char* label, ImVec4& color, const std::optional<ImVec4>& default_color) noexcept
{
	auto popup_name = UTIL_FORMAT('#' << label << UTIL_XOR("##COLOR_PICKER"));
	auto is_edited { false };

	ImGui::ColorButton(label, color, ImGuiColorEditFlags_AlphaPreview);

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(UTIL_CXOR("Click to edit color"));
		ImGui::EndTooltip();
	}

	if (ImGui::IsItemClicked())
		ImGui::OpenPopup(popup_name.c_str());

	if (ImGui::BeginPopup(popup_name.c_str()))
	{
		if (ImGui::ColorPicker4(
			label,
			(float*)&color,
			ImGuiColorEditFlags_DisplayHex |
			ImGuiColorEditFlags_DisplayRGB |
			ImGuiColorEditFlags_DisplayHSV |
			ImGuiColorEditFlags_InputRGB |
			ImGuiColorEditFlags_AlphaBar |
			ImGuiColorEditFlags_AlphaPreview
		))
			is_edited = true;

		ImGui::NewLine();

		if (ImGui::Button(UTIL_CXOR("Copy Color")))
			CopiedColor() = color;

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();

			ImGui::TextUnformatted(UTIL_CXOR("Before: "));
			ImGui::SameLine();
			ImGui::ColorButton(label, CopiedColor(), ImGuiColorEditFlags_AlphaPreview);

			ImGui::TextUnformatted(UTIL_CXOR("After: "));
			ImGui::SameLine();
			ImGui::ColorButton(label, color, ImGuiColorEditFlags_AlphaPreview);

			ImGui::EndTooltip();
		}

		ImGui::SameLine();

		if (ImGui::Button(UTIL_CXOR("Paste Color")))
		{
			color = CopiedColor();
			is_edited = true;
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();

			ImGui::TextUnformatted(UTIL_CXOR("Copied color: "));
			ImGui::SameLine();
			ImGui::ColorButton(label, CopiedColor(), ImGuiColorEditFlags_AlphaPreview);

			ImGui::EndTooltip();
		}

		if (default_color)
		{
			ImGui::SameLine();

			if (ImGui::Button(UTIL_CXOR("Reset Color")))
			{
				color = *default_color;
				is_edited = true;
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();

				ImGui::TextUnformatted(UTIL_CXOR("Default color: "));
				ImGui::SameLine();
				ImGui::ColorButton(label, *default_color, ImGuiColorEditFlags_AlphaPreview);

				ImGui::EndTooltip();
			}
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();
	ImGui::TextUnformatted(label);

	return is_edited;
}
