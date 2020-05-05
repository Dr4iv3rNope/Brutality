#include "errors.hpp"

#include "../imgui.h"

#include "../../util/xorstr.hpp"

#include "../../util/debug/assert.hpp"

void ImGui::Custom::StatusError(int& current_error, const ErrorList& errors)
{
	if (current_error >= 0 && current_error + 1 <= int(errors.size()))
	{
		ImVec4 color(1.f, 1.f, 1.f, 1.f);

		auto error = errors.cbegin();
		std::advance(error, current_error);

		ImGui::TextColored(error->Color(), error->What().c_str());

		if (ImGui::IsItemClicked())
			current_error = -1;

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
				
			if (error->Description())
				ImGui::TextUnformatted(error->Description()->c_str());

			ImGui::TextUnformatted(UTIL_CXOR("Click to reset the error"));
			ImGui::EndTooltip();
		}
	}
}

ImGui::Custom::Error::Error(const std::string& what, const ImVec4& color) noexcept
	: _what { what }, _color { color }, _description { std::nullopt }
{}

ImGui::Custom::Error::Error(const std::string& what, const ImVec4& color, const std::string& desc) noexcept
	: _what { what }, _color { color }, _description { desc }
{}
