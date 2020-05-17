#include "keys.hpp"

#include "../imgui.h"

#include "../../util/xorstr.hpp"
#include "../../util/strings.hpp"

#include <deque>
#include <Windows.h>

static inline const std::deque<std::string>& GetKeyStringList() noexcept
{
	static std::deque keyStringList =
	{
		// Function keys (0-23)

		UTIL_SXOR("F1"),		// 0
		UTIL_SXOR("F2"),		// 1
		UTIL_SXOR("F3"),		// 2
		UTIL_SXOR("F4"),		// 3
		UTIL_SXOR("F5"),		// 4
		UTIL_SXOR("F6"),		// 5
		UTIL_SXOR("F7"),		// 6
		UTIL_SXOR("F8"),		// 7
		UTIL_SXOR("F9"),		// 8
		UTIL_SXOR("F10"),		// 9
		UTIL_SXOR("F11"),		// 10
		UTIL_SXOR("F12"),		// 11
		UTIL_SXOR("F13"),		// 12
		UTIL_SXOR("F14"),		// 13
		UTIL_SXOR("F15"),		// 14
		UTIL_SXOR("F16"),		// 15
		UTIL_SXOR("F17"),		// 16
		UTIL_SXOR("F18"),		// 17
		UTIL_SXOR("F19"),		// 18
		UTIL_SXOR("F20"),		// 19
		UTIL_SXOR("F21"),		// 20
		UTIL_SXOR("F22"),		// 21
		UTIL_SXOR("F23"),		// 22
		UTIL_SXOR("F24"),		// 23

		// Number keys (24-33)

		UTIL_SXOR("1"),			// 24
		UTIL_SXOR("2"),			// 25
		UTIL_SXOR("3"),			// 26
		UTIL_SXOR("4"),			// 27
		UTIL_SXOR("5"),			// 28
		UTIL_SXOR("6"),			// 29
		UTIL_SXOR("7"),			// 30
		UTIL_SXOR("8"),			// 31
		UTIL_SXOR("9"),			// 32
		UTIL_SXOR("0"),			// 33

		// Letters (34-59)

		UTIL_SXOR("Q"),			// 34
		UTIL_SXOR("W"),			// 35
		UTIL_SXOR("E"),			// 36
		UTIL_SXOR("R"),			// 37
		UTIL_SXOR("T"),			// 38
		UTIL_SXOR("Y"),			// 39
		UTIL_SXOR("U"),			// 40
		UTIL_SXOR("I"),			// 41
		UTIL_SXOR("O"),			// 42
		UTIL_SXOR("P"),			// 43
		UTIL_SXOR("A"),			// 44
		UTIL_SXOR("S"),			// 45
		UTIL_SXOR("D"),			// 46
		UTIL_SXOR("F"),			// 47
		UTIL_SXOR("G"),			// 48
		UTIL_SXOR("H"),			// 49
		UTIL_SXOR("J"),			// 50
		UTIL_SXOR("K"),			// 51
		UTIL_SXOR("L"),			// 52
		UTIL_SXOR("Z"),			// 53
		UTIL_SXOR("X"),			// 54
		UTIL_SXOR("C"),			// 55
		UTIL_SXOR("V"),			// 56
		UTIL_SXOR("B"),			// 57
		UTIL_SXOR("N"),			// 58
		UTIL_SXOR("M"),			// 59

		// Symbols (60-)

		UTIL_SXOR("`"),			// 60
		UTIL_SXOR("["),			// 61
		UTIL_SXOR("]"),			// 62
		UTIL_SXOR(";"),			// 63
		UTIL_SXOR("'"),			// 64
		UTIL_SXOR("\\"),		// 65
		UTIL_SXOR(","),			// 66
		UTIL_SXOR("."),			// 67
		UTIL_SXOR("/"),			// 68
		UTIL_SXOR("-"),			// 69 :joy: :ok_hand: :100:
		UTIL_SXOR("="),			// 70

		// Control keys (71-92)

		UTIL_SXOR("Tab"),			// 71
		UTIL_SXOR("Caps Lock"),		// 72
		UTIL_SXOR("Shift"),			// 73
		UTIL_SXOR("Ctrl"),			// 74
		UTIL_SXOR("Alt"),			// 75
		UTIL_SXOR("Space"),			// 76
		UTIL_SXOR("Enter"),			// 77
		UTIL_SXOR("Backspace"),		// 78
		UTIL_SXOR("Insert"),		// 79
		UTIL_SXOR("Home"),			// 80
		UTIL_SXOR("Page Up"),		// 81
		UTIL_SXOR("Delete"),		// 82
		UTIL_SXOR("End"),			// 83
		UTIL_SXOR("Page Down"),		// 84
		UTIL_SXOR("Scroll Lock"),	// 85
		UTIL_SXOR("Up Arrow"),		// 86
		UTIL_SXOR("Down Arrow"),	// 87
		UTIL_SXOR("Left Arrow"),	// 88
		UTIL_SXOR("Right Arrow"),	// 89

		// Numpad (90-104)

		UTIL_SXOR("Numpad /"),		// 90
		UTIL_SXOR("Numpad *"),		// 91
		UTIL_SXOR("Numpad -"),		// 92
		UTIL_SXOR("Numpad +"),		// 93
		UTIL_SXOR("Numpad ."),		// 94
		UTIL_SXOR("Numpad 1"),		// 95
		UTIL_SXOR("Numpad 2"),		// 96
		UTIL_SXOR("Numpad 3"),		// 97
		UTIL_SXOR("Numpad 4"),		// 98
		UTIL_SXOR("Numpad 5"),		// 99
		UTIL_SXOR("Numpad 6"),		// 100
		UTIL_SXOR("Numpad 7"),		// 101
		UTIL_SXOR("Numpad 8"),		// 102
		UTIL_SXOR("Numpad 9"),		// 103
		UTIL_SXOR("Numpad 0"),		// 104

		// Mouse (105-112)

		UTIL_SXOR("LMB"),		// 105
		UTIL_SXOR("RMB"),		// 106
		UTIL_SXOR("MMB"),		// 107
		UTIL_SXOR("Mouse 4"),	// 108
		UTIL_SXOR("Mouse 5"),	// 109
	};

	return keyStringList;
}

static inline const std::deque<ImGui::Custom::VirtualKey>& GetVirtualKeyList() noexcept
{
	static std::deque<ImGui::Custom::VirtualKey> vkKeyList =
	{
		// Function keys (0-23)

		VK_F1,		// 0
		VK_F2,		// 1
		VK_F3,		// 2
		VK_F4,		// 3
		VK_F5,		// 4
		VK_F6,		// 5
		VK_F7,		// 6
		VK_F8,		// 7
		VK_F9,		// 8
		VK_F10,		// 9
		VK_F11,		// 10
		VK_F12,		// 11
		VK_F13,		// 12
		VK_F14,		// 13
		VK_F15,		// 14
		VK_F16,		// 15
		VK_F17,		// 16
		VK_F18,		// 17
		VK_F19,		// 18
		VK_F20,		// 19
		VK_F21,		// 20
		VK_F22,		// 21
		VK_F23,		// 22
		VK_F24,		// 23

		// Number keys (24-33)

		'1',		// 24
		'2',		// 25
		'3',		// 26
		'4',		// 27
		'5',		// 28
		'6',		// 29
		'7',		// 30
		'8',		// 31
		'9',		// 32
		'0',		// 33

		// Letters (34-59)

		'Q',		// 34
		'W',		// 35
		'E',		// 36
		'R',		// 37
		'T',		// 38
		'Y',		// 39
		'U',		// 40
		'I',		// 41
		'O',		// 42
		'P',		// 43
		'A',		// 44
		'S',		// 45
		'D',		// 46
		'F',		// 47
		'G',		// 48
		'H',		// 49
		'J',		// 50
		'K',		// 51
		'L',		// 52
		'Z',		// 53
		'X',		// 54
		'C',		// 55
		'V',		// 56
		'B',		// 57
		'N',		// 58
		'M',		// 59

		// Symbols (60-)

		VK_OEM_3,		// 60
		VK_OEM_4,		// 61
		VK_OEM_6,		// 62
		VK_OEM_1,		// 63
		VK_OEM_7,		// 64
		VK_OEM_5,		// 65
		VK_OEM_COMMA,	// 66
		VK_OEM_PERIOD,	// 67
		VK_OEM_2,		// 68
		VK_OEM_PERIOD,	// 69 :joy: :ok_hand: :100:
		VK_OEM_PLUS,	// 70

		// Control keys (71-92)

		VK_TAB,			// 71
		VK_CAPITAL,		// 72
		VK_SHIFT,		// 73
		VK_CONTROL,		// 74
		VK_MENU,		// 75
		VK_SPACE,		// 76
		VK_RETURN,		// 77
		VK_BACK,		// 78
		VK_INSERT,		// 79
		VK_HOME,		// 80
		VK_PRIOR,		// 81
		VK_DELETE,		// 82
		VK_END,			// 83
		VK_NEXT,		// 84
		VK_SCROLL,		// 85
		VK_UP,			// 86
		VK_DOWN,		// 87
		VK_LEFT,		// 88
		VK_RIGHT,		// 89

		// Numpad (90-104)

		0x6F,			// 90
		0x6A,			// 91
		0x6D,			// 92
		0x6B,			// 93
		0x6E,			// 94
		VK_NUMPAD1,		// 95
		VK_NUMPAD2,		// 96
		VK_NUMPAD3,		// 97
		VK_NUMPAD4,		// 98
		VK_NUMPAD5,		// 99
		VK_NUMPAD6,		// 100
		VK_NUMPAD7,		// 101
		VK_NUMPAD8,		// 102
		VK_NUMPAD9,		// 103
		VK_NUMPAD0,		// 104

		// Mouse (105-112)

		VK_LBUTTON,		// 105
		VK_RBUTTON,		// 106
		VK_MBUTTON,		// 107
		VK_XBUTTON1,	// 108
		VK_XBUTTON2,	// 109
	};

	return vkKeyList;
}

bool ImGui::Custom::KeyToString(Key key, const std::string** out) noexcept
{
	if (key < GetKeyStringList().size())
	{
		if (out)
			*out = &GetKeyStringList()[key];

		return true;
	}
	else
		return false;
}

bool ImGui::Custom::KeyToVirtualKey(Key key, VirtualKey* out) noexcept
{
	if (key < GetVirtualKeyList().size())
	{
		if (out)
			*out = GetVirtualKeyList()[key];

		return true;
	}
	else
		return false;
}

bool ImGui::Custom::IsKeyPressed(Key key, bool repeat) noexcept
{
	if (VirtualKey vk; KeyToVirtualKey(key, &vk))
		return ImGui::IsKeyPressed(vk, repeat);

	return false;
}

bool ImGui::Custom::IsKeyDown(Key key) noexcept
{
	if (VirtualKey vk; KeyToVirtualKey(key, &vk))
		return ImGui::IsKeyDown(vk);

	return false;
}

bool ImGui::Custom::IsKeyReleased(Key key) noexcept
{
	if (VirtualKey vk; KeyToVirtualKey(key, &vk))
		return ImGui::IsKeyReleased(vk);

	return false;
}

short ImGui::Custom::GetAsyncKeyState(Key key) noexcept
{
	if (VirtualKey vk; KeyToVirtualKey(key, &vk))
		return ::GetAsyncKeyState(vk);

	return 0;
}

bool ImGui::Custom::InputKey(const char* label, Key& key) noexcept
{
	bool is_changed { false };

	static auto showKey = [] (const char* what, Key key)
	{
		if (const std::string* name; KeyToString(key, &name))
			ImGui::Text(UTIL_CXOR("%s: { %s }"), what, name->c_str());
		else
			ImGui::Text(UTIL_CXOR("%s: None"), what);
	};

	if (ImGui::Button(UTIL_FORMAT(UTIL_XOR("Change ") << label).c_str()))
		ImGui::OpenPopup(UTIL_CXOR("CUSTOM##INPUTKEY"));

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		showKey(UTIL_CXOR("Current Key"), key);
		ImGui::EndTooltip();
	}

	if (ImGui::BeginPopup(UTIL_CXOR("CUSTOM##INPUTKEY")))
	{
		static Key newKey { Keys::INVALID };
		static Key copiedKey { Keys::INVALID };

		showKey(UTIL_CXOR("Current Key"), key);

		ImGui::Separator();

		showKey(UTIL_CXOR("Hover to change key\nNew Key"), newKey);

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(UTIL_CXOR("Press any key"));
			ImGui::EndTooltip();

			for (Key i = 0; i < Keys::KEY_COUNT; i++)
				if (Custom::GetAsyncKeyState(i))
					newKey = i;
		}


		ImGui::NewLine();

		if (ImGui::Button(UTIL_CXOR("Set")))
		{
			key = newKey;
			is_changed = true;
		}

		ImGui::SameLine();

		if (ImGui::Button(UTIL_CXOR("Reset")))
		{
			key = newKey = Keys::INVALID;
			is_changed = true;
		}

		ImGui::SameLine();

		if (ImGui::Button(UTIL_CXOR("Copy")))
		{
			copiedKey = key;
			is_changed = true;
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();

			showKey(UTIL_CXOR("Before"), copiedKey);
			showKey(UTIL_CXOR("After"), key);

			ImGui::EndTooltip();
		}

		ImGui::SameLine();

		if (ImGui::Button(UTIL_CXOR("Paste")))
		{
			key = newKey = copiedKey;
			is_changed = true;
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			showKey(UTIL_CXOR("Copied key"), copiedKey);
			ImGui::EndTooltip();
		}

		ImGui::EndPopup();
	}

	return is_changed;
}
