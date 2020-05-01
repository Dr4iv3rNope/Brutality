#pragma once
#include "../util/xorstr.hpp"
#include "../util/strings.hpp"

#include "imgui.h"
#include "imgui_stdlib.h"

#include <memory>
#include <cstdarg>
#include <optional>

namespace ImGuiCustom
{
	namespace
	{
		static ImVec4& CopiedColor() noexcept
		{
			static ImVec4 color(1.f, 1.f, 1.f, 1.f);

			return color;
		}
	}

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

	static bool ColorPicker(const char* label, ImVec4& color,
							const std::optional<ImVec4>& default_color = {}) noexcept
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

	static bool CopiableText(const char* format, ...) noexcept
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

	static std::string ToTextBoolean(bool condition) noexcept
	{
		return condition ? UTIL_SXOR("true") : UTIL_SXOR("false");
	}

	inline std::string FormatSysString(std::string str) noexcept
	{
		for (auto i = str.find('\\'); i != -1; i = str.find('\\', i + 1))
		{
			auto replaceToSysChar = [&str, i] (char sysch) noexcept
			{
				str.erase(i, 2);
				str.insert(str.begin() + i, sysch);
			};

			switch (str[i + 1])
			{
				case 'n': replaceToSysChar('\n'); break;
				case 'r': replaceToSysChar('\r'); break;
				case 't': replaceToSysChar('\t'); break;
				case 'b': replaceToSysChar('\b'); break;	// backspace
				case 'e': replaceToSysChar('\x1B'); break;	// escape
				case 'd': replaceToSysChar('\x7F'); break;	// delete
				case 'a': replaceToSysChar('\a'); break;	// bell
				case 'v': replaceToSysChar('\v'); break;	// vertical tabulation
				case 'f': replaceToSysChar('\f'); break;	// form feed

				case '1': replaceToSysChar('\x01'); break;
				case '2': replaceToSysChar('\x02'); break;
				case '3': replaceToSysChar('\x03'); break;
				case '4': replaceToSysChar('\x04'); break;
				case '5': replaceToSysChar('\x05'); break;
				case '6': replaceToSysChar('\x06'); break;
			}
		}

		return str;
	}

	inline bool InputSysText(const char* label,
							 std::string& non_formated,
							 std::string* formated = nullptr,
							 ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue) noexcept
	{
		bool is_changed = ImGui::InputText(label, &non_formated, flags);

		if (is_changed && formated)
			*formated = FormatSysString(non_formated);

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(UTIL_CXOR(
				"Text entry supports special symbols\n"
				"Example: \\t\\t\\tMagic\\nString\n"
				"\nSymbol List:\n"
				"\\n - new line\n"
				"\\r - carriage return\n"
				"\\t - tabulation\n"
				"\\b - backspace\n"
				"\\e - escape\n"
				"\\d - delete (0x7F)\n"
				"\\a - bell\n"
				"\\v - vertical tabulation\n"
				"\\f - form feed\n"
				"\nExperemental:\n"
				"\\1 - start of heading\n"
				"\\2 - start of text\n"
				"\\3 - end of text\n"
				"\\4 - end of transmission\n"
				"\\5 - enquiry\n"
				"\\6 - acknowledge"
			));
			ImGui::EndTooltip();
		}

		return is_changed;
	}
}
