#include "specialchars.hpp"

#include "../imgui.h"

#include "../../util/xorstr.hpp"

std::string ImGui::Custom::FormatSpecialChars(std::string str) noexcept
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

void ImGui::Custom::ShowSpecialChars() noexcept
{
	ImGui::BeginTooltip();
	ImGui::TextUnformatted(UTIL_CXOR(
		"Text entry supports special chars\n"
		"Example: \\t\\t\\tMagic\\nString\n"
		"\nCharacter List:\n"
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
