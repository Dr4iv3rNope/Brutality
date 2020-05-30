#include "keys.hpp"

#include "../imgui.h"

#include "../../util/xorstr.hpp"
#include "../../util/strings.hpp"

#include "../../util/debug/assert.hpp"

#include <deque>
#include <Windows.h>

static inline const std::deque<std::string>& GetKeyStringList() noexcept
{
	static std::deque<std::string> keyStringList =
	{
		// Function keys (0-23)

		UTIL_SXOR("F1"),
		UTIL_SXOR("F2"),
		UTIL_SXOR("F3"),
		UTIL_SXOR("F4"),
		UTIL_SXOR("F5"),
		UTIL_SXOR("F6"),
		UTIL_SXOR("F7"),
		UTIL_SXOR("F8"),
		UTIL_SXOR("F9"),
		UTIL_SXOR("F10"),
		UTIL_SXOR("F11"),
		UTIL_SXOR("F12"),
		UTIL_SXOR("F13"),
		UTIL_SXOR("F14"),
		UTIL_SXOR("F15"),
		UTIL_SXOR("F16"),
		UTIL_SXOR("F17"),
		UTIL_SXOR("F18"),
		UTIL_SXOR("F19"),
		UTIL_SXOR("F20"),
		UTIL_SXOR("F21"),
		UTIL_SXOR("F22"),
		UTIL_SXOR("F23"),
		UTIL_SXOR("F24"),

		// Number keys (24-33)

		UTIL_SXOR("1"),
		UTIL_SXOR("2"),
		UTIL_SXOR("3"),
		UTIL_SXOR("4"),
		UTIL_SXOR("5"),
		UTIL_SXOR("6"),
		UTIL_SXOR("7"),
		UTIL_SXOR("8"),
		UTIL_SXOR("9"),
		UTIL_SXOR("0"),

		// Letters (34-59)

		UTIL_SXOR("A"),
		UTIL_SXOR("B"),
		UTIL_SXOR("C"),
		UTIL_SXOR("D"),
		UTIL_SXOR("E"),
		UTIL_SXOR("F"),
		UTIL_SXOR("G"),
		UTIL_SXOR("H"),
		UTIL_SXOR("I"),
		UTIL_SXOR("J"),
		UTIL_SXOR("K"),
		UTIL_SXOR("L"),
		UTIL_SXOR("M"),
		UTIL_SXOR("N"),
		UTIL_SXOR("O"),
		UTIL_SXOR("P"),
		UTIL_SXOR("Q"),
		UTIL_SXOR("R"),
		UTIL_SXOR("S"),
		UTIL_SXOR("T"),
		UTIL_SXOR("U"),
		UTIL_SXOR("V"),
		UTIL_SXOR("W"),
		UTIL_SXOR("X"),
		UTIL_SXOR("Y"),
		UTIL_SXOR("Z"),

		// Symbols (60-)

		UTIL_SXOR("`"),
		UTIL_SXOR("["),
		UTIL_SXOR("]"),
		UTIL_SXOR(";"),
		UTIL_SXOR("'"),
		UTIL_SXOR("\\"),
		UTIL_SXOR(","),
		UTIL_SXOR("."),
		UTIL_SXOR("/"),
		UTIL_SXOR("-"),
		UTIL_SXOR("="),

		// Control keys (71-92)

		UTIL_SXOR("Tab"),
		UTIL_SXOR("Caps Lock"),
		UTIL_SXOR("Shift"),
		UTIL_SXOR("Ctrl"),
		UTIL_SXOR("Alt"),
		UTIL_SXOR("Space"),
		UTIL_SXOR("Enter"),
		UTIL_SXOR("Backspace"),
		UTIL_SXOR("Insert"),
		UTIL_SXOR("Home"),
		UTIL_SXOR("Page Up"),
		UTIL_SXOR("Delete"),
		UTIL_SXOR("End"),
		UTIL_SXOR("Page Down"),
		UTIL_SXOR("Scroll Lock"),
		UTIL_SXOR("Up Arrow"),
		UTIL_SXOR("Down Arrow"),
		UTIL_SXOR("Left Arrow"),
		UTIL_SXOR("Right Arrow"),

		// Numpad (90-104)

		UTIL_SXOR("Numpad /"),
		UTIL_SXOR("Numpad *"),
		UTIL_SXOR("Numpad -"),
		UTIL_SXOR("Numpad +"),
		UTIL_SXOR("Numpad ."),
		UTIL_SXOR("Numpad 1"),
		UTIL_SXOR("Numpad 2"),
		UTIL_SXOR("Numpad 3"),
		UTIL_SXOR("Numpad 4"),
		UTIL_SXOR("Numpad 5"),
		UTIL_SXOR("Numpad 6"),
		UTIL_SXOR("Numpad 7"),
		UTIL_SXOR("Numpad 8"),
		UTIL_SXOR("Numpad 9"),
		UTIL_SXOR("Numpad 0"),

		// Mouse (105-112)

		UTIL_SXOR("LMB"),
		UTIL_SXOR("RMB"),
		UTIL_SXOR("MMB"),
		UTIL_SXOR("Mouse 4"),
		UTIL_SXOR("Mouse 5"),
	};

	return keyStringList;
}

static inline const std::deque<ImGui::Custom::VirtualKey>& GetVirtualKeyList() noexcept
{
	static std::deque<ImGui::Custom::VirtualKey> vkKeyList =
	{
		// Function keys

		VK_F1,
		VK_F2,
		VK_F3,
		VK_F4,
		VK_F5,
		VK_F6,
		VK_F7,
		VK_F8,
		VK_F9,
		VK_F10,
		VK_F11,
		VK_F12,
		VK_F13,
		VK_F14,
		VK_F15,
		VK_F16,
		VK_F17,
		VK_F18,
		VK_F19,
		VK_F20,
		VK_F21,
		VK_F22,
		VK_F23,
		VK_F24,

		// Number keys

		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'0',

		// Letters

		'A',
		'B',
		'C',
		'D',
		'E',
		'F',
		'G',
		'H',
		'I',
		'J',
		'K',
		'L',
		'M',
		'N',
		'O',
		'P',
		'Q',
		'R',
		'S',
		'T',
		'U',
		'V',
		'W',
		'X',
		'Y',
		'Z',

		// Symbols

		VK_OEM_3,
		VK_OEM_4,
		VK_OEM_6,
		VK_OEM_1,
		VK_OEM_7,
		VK_OEM_5,
		VK_OEM_COMMA,
		VK_OEM_PERIOD,
		VK_OEM_2,
		VK_OEM_PERIOD,
		VK_OEM_PLUS,

		// Control keys

		VK_TAB,
		VK_CAPITAL,
		VK_SHIFT,
		VK_CONTROL,
		VK_MENU,
		VK_SPACE,
		VK_RETURN,
		VK_BACK,
		VK_INSERT,
		VK_HOME,
		VK_PRIOR,
		VK_DELETE,
		VK_END,
		VK_NEXT,
		VK_SCROLL,
		VK_UP,
		VK_DOWN,
		VK_LEFT,
		VK_RIGHT,

		// Numpad 

		0x6F,
		0x6A,
		0x6D,
		0x6B,
		0x6E,
		VK_NUMPAD1,
		VK_NUMPAD2,
		VK_NUMPAD3,
		VK_NUMPAD4,
		VK_NUMPAD5,
		VK_NUMPAD6,
		VK_NUMPAD7,
		VK_NUMPAD8,
		VK_NUMPAD9,
		VK_NUMPAD0,

		// Mouse

		VK_LBUTTON,
		VK_RBUTTON,
		VK_MBUTTON,
		VK_XBUTTON1,
		VK_XBUTTON2,
	};

	return vkKeyList;
}

static inline const std::deque<SourceSDK::ButtonCode>& GetButtonCodeList() noexcept
{
	static std::deque<SourceSDK::ButtonCode> keyStringList =
	{
		// Function keys (0-23)

		SourceSDK::ButtonCode::KeyF1,
		SourceSDK::ButtonCode::KeyF2,
		SourceSDK::ButtonCode::KeyF3,
		SourceSDK::ButtonCode::KeyF4,
		SourceSDK::ButtonCode::KeyF5,
		SourceSDK::ButtonCode::KeyF6,
		SourceSDK::ButtonCode::KeyF7,
		SourceSDK::ButtonCode::KeyF8,
		SourceSDK::ButtonCode::KeyF9,
		SourceSDK::ButtonCode::KeyF10,
		SourceSDK::ButtonCode::KeyF11,
		SourceSDK::ButtonCode::KeyF12,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::Invalid,

		// Number keys (24-33)

		SourceSDK::ButtonCode::Key1,
		SourceSDK::ButtonCode::Key2,
		SourceSDK::ButtonCode::Key3,
		SourceSDK::ButtonCode::Key4,
		SourceSDK::ButtonCode::Key5,
		SourceSDK::ButtonCode::Key6,
		SourceSDK::ButtonCode::Key7,
		SourceSDK::ButtonCode::Key8,
		SourceSDK::ButtonCode::Key9,
		SourceSDK::ButtonCode::Key0,

		// Letters (34-59)

		SourceSDK::ButtonCode::KeyA,
		SourceSDK::ButtonCode::KeyB,
		SourceSDK::ButtonCode::KeyC,
		SourceSDK::ButtonCode::KeyD,
		SourceSDK::ButtonCode::KeyE,
		SourceSDK::ButtonCode::KeyF,
		SourceSDK::ButtonCode::KeyG,
		SourceSDK::ButtonCode::KeyH,
		SourceSDK::ButtonCode::KeyI,
		SourceSDK::ButtonCode::KeyJ,
		SourceSDK::ButtonCode::KeyK,
		SourceSDK::ButtonCode::KeyL,
		SourceSDK::ButtonCode::KeyM,
		SourceSDK::ButtonCode::KeyN,
		SourceSDK::ButtonCode::KeyO,
		SourceSDK::ButtonCode::KeyP,
		SourceSDK::ButtonCode::KeyQ,
		SourceSDK::ButtonCode::KeyR,
		SourceSDK::ButtonCode::KeyS,
		SourceSDK::ButtonCode::KeyT,
		SourceSDK::ButtonCode::KeyU,
		SourceSDK::ButtonCode::KeyV,
		SourceSDK::ButtonCode::KeyW,
		SourceSDK::ButtonCode::KeyX,
		SourceSDK::ButtonCode::KeyY,
		SourceSDK::ButtonCode::KeyZ,

		// Symbols (60-)

		SourceSDK::ButtonCode::Invalid,
		SourceSDK::ButtonCode::KeyLBracket,
		SourceSDK::ButtonCode::KeyRBracket,
		SourceSDK::ButtonCode::KeySemicolon,
		SourceSDK::ButtonCode::KeyApostrophe,
		SourceSDK::ButtonCode::KeyBackslash,
		SourceSDK::ButtonCode::KeyComma,
		SourceSDK::ButtonCode::KeyPeriod,
		SourceSDK::ButtonCode::KeySlash,
		SourceSDK::ButtonCode::KeyMinus,
		SourceSDK::ButtonCode::KeyEqual,

		// Control keys (71-92)

		SourceSDK::ButtonCode::KeyTab,
		SourceSDK::ButtonCode::KeyCapslock,
		SourceSDK::ButtonCode::KeyLShift,
		SourceSDK::ButtonCode::KeyLControl,
		SourceSDK::ButtonCode::KeyLAlt,
		SourceSDK::ButtonCode::KeySpace,
		SourceSDK::ButtonCode::KeyEnter,
		SourceSDK::ButtonCode::KeyBackspace,
		SourceSDK::ButtonCode::KeyInsert,
		SourceSDK::ButtonCode::KeyHome,
		SourceSDK::ButtonCode::KeyPageUp,
		SourceSDK::ButtonCode::KeyDelete,
		SourceSDK::ButtonCode::KeyEnd,
		SourceSDK::ButtonCode::KeyPageDown,
		SourceSDK::ButtonCode::KeyScrollLock,
		SourceSDK::ButtonCode::KeyUp,
		SourceSDK::ButtonCode::KeyDown,
		SourceSDK::ButtonCode::KeyLeft,
		SourceSDK::ButtonCode::KeyRight,

		// Numpad (90-104)

		SourceSDK::ButtonCode::KeyPadDivide,
		SourceSDK::ButtonCode::KeyPadMultiply,
		SourceSDK::ButtonCode::KeyPadMinus,
		SourceSDK::ButtonCode::KeyPadPlus,
		SourceSDK::ButtonCode::KeyPadDecimal,
		SourceSDK::ButtonCode::KeyPad1,
		SourceSDK::ButtonCode::KeyPad2,
		SourceSDK::ButtonCode::KeyPad3,
		SourceSDK::ButtonCode::KeyPad4,
		SourceSDK::ButtonCode::KeyPad5,
		SourceSDK::ButtonCode::KeyPad6,
		SourceSDK::ButtonCode::KeyPad7,
		SourceSDK::ButtonCode::KeyPad8,
		SourceSDK::ButtonCode::KeyPad9,
		SourceSDK::ButtonCode::KeyPad0,

		// Mouse (105-112)

		SourceSDK::ButtonCode::MouseLeft,
		SourceSDK::ButtonCode::MouseRight,
		SourceSDK::ButtonCode::MouseMiddle,
		SourceSDK::ButtonCode::Mouse4,
		SourceSDK::ButtonCode::Mouse5,
	};

	return keyStringList;
}

bool ImGui::Custom::KeyToString(Key key, const std::string** out) noexcept
{
	UTIL_DEBUG_ASSERT(GetVirtualKeyList().size() == std::size_t(Key::_KeyCount));

	if (key != Key::_Invalid)
	{
		if (out)
			*out = &GetKeyStringList()[int(key)];

		return true;
	}
	else
		return false;
}

bool ImGui::Custom::KeyToVirtualKey(Key key, VirtualKey* out) noexcept
{
	UTIL_DEBUG_ASSERT(GetVirtualKeyList().size() == std::size_t(Key::_KeyCount));

	if (key != Key::_Invalid)
	{
		if (out)
			*out = GetVirtualKeyList()[int(key)];

		return true;
	}
	else
		return false;
}

bool ImGui::Custom::KeyToButtonCode(Key key, SourceSDK::ButtonCode* out) noexcept
{
	if (key != Key::_Invalid)
	{
		if (auto buttoncode = GetButtonCodeList()[int(key)];
			buttoncode != SourceSDK::ButtonCode::Invalid)
		{
			if (out)
				*out = buttoncode;

			return true;
		}
	}
	
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
		static Key newKey { Key::_Invalid };
		static Key copiedKey { Key::_Invalid };

		showKey(UTIL_CXOR("Current Key"), key);

		ImGui::Separator();

		showKey(UTIL_CXOR("Hover to change key\nNew Key"), newKey);

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(UTIL_CXOR("Press any key"));
			ImGui::EndTooltip();

			for (int i = int(Key::_Invalid) + 1; i < int(Key::_KeyCount); i++)
				if (Custom::GetAsyncKeyState(Key(i)))
					newKey = Key(i);
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
			key = newKey = Key::_Invalid;
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
