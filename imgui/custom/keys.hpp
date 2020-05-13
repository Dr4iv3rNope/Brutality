#pragma once
#include <cstdint>
#include <string>

namespace ImGui
{
	namespace Custom
	{
		using Key = std::uint16_t;
		using VirtualKey = std::uint8_t;

		namespace Keys
		{
			// functional key (F*)
			// 0-23

			constexpr Key F1 = 0;
			constexpr Key F2 = 1;
			constexpr Key F3 = 2;
			constexpr Key F4 = 3;
			constexpr Key F5 = 4;
			constexpr Key F6 = 5;
			constexpr Key F7 = 6;
			constexpr Key F8 = 7;
			constexpr Key F9 = 8;
			constexpr Key F10 = 9;
			constexpr Key F11 = 10;
			constexpr Key F12 = 11;
			constexpr Key F13 = 12;
			constexpr Key F14 = 13;
			constexpr Key F15 = 14;
			constexpr Key F16 = 15;
			constexpr Key F17 = 16;
			constexpr Key F18 = 17;
			constexpr Key F19 = 18;
			constexpr Key F20 = 19;
			constexpr Key F21 = 20;
			constexpr Key F22 = 21;
			constexpr Key F23 = 22;
			constexpr Key F24 = 23;

			constexpr bool IsFunctionalKey(Key key) noexcept
			{
				return key <= 23;
			}

			// Number keys (24-33)

			constexpr Key _1 = 24;
			constexpr Key _2 = 25;
			constexpr Key _3 = 26;
			constexpr Key _4 = 27;
			constexpr Key _5 = 28;
			constexpr Key _6 = 29;
			constexpr Key _7 = 30;
			constexpr Key _8 = 31;
			constexpr Key _9 = 32;
			constexpr Key _0 = 33;

			constexpr bool IsNumberKey(Key key) noexcept
			{
				return key >= 24 && key <= 33;
			}

			// Letters (34-59)

			constexpr Key Q = 34;
			constexpr Key W = 35;
			constexpr Key E = 36;
			constexpr Key R = 37;
			constexpr Key T = 38;
			constexpr Key Y = 39;
			constexpr Key U = 40;
			constexpr Key I = 41;
			constexpr Key O = 42;
			constexpr Key P = 43;
			constexpr Key A = 44;
			constexpr Key S = 45;
			constexpr Key D = 46;
			constexpr Key F = 47;
			constexpr Key G = 48;
			constexpr Key H = 49;
			constexpr Key J = 50;
			constexpr Key K = 51;
			constexpr Key L = 52;
			constexpr Key Z = 53;
			constexpr Key X = 54;
			constexpr Key C = 55;
			constexpr Key V = 56;
			constexpr Key B = 57;
			constexpr Key N = 58;
			constexpr Key M = 59;

			constexpr bool IsLetterKey(Key key) noexcept
			{
				return key >= 34 && key <= 59;
			}

			// Symbols (60-70)

			constexpr Key GRAVE = 60;
			constexpr Key LEFT_SQUARE_BRACKET = 61;
			constexpr Key RIGHT_SQUARE_BRACKET = 62;
			constexpr Key SEMICOLON = 63;
			constexpr Key APOSTROPHE = 64;
			constexpr Key BACKSLASH = 65;
			constexpr Key COMMA = 66;
			constexpr Key DOT = 67;
			constexpr Key SLASH = 68;
			constexpr Key MINUS = 69;
			constexpr Key EQUAL = 70;

			constexpr bool IsSymbolKey(Key key) noexcept
			{
				return key >= 60 && key <= 70;
			}

			// Control keys (71-89)

			constexpr Key TAB = 71;
			constexpr Key CAPSLOCK = 72;
			constexpr Key SHIFT = 73;
			constexpr Key CTRL = 74;
			constexpr Key ALT = 75;
			constexpr Key SPACE = 76;
			constexpr Key ENTER = 77;
			constexpr Key BACKSPACE = 78;
			constexpr Key INSERT = 79;
			constexpr Key HOME = 80;
			constexpr Key PAGEUP = 81;
			constexpr Key DEL = 82; // delete
			constexpr Key END = 83;
			constexpr Key PAGEDOWN = 84;
			constexpr Key SCROLLLOCK = 85;
			constexpr Key UPARROW = 86;
			constexpr Key DOWNARROW = 87;
			constexpr Key LEFTARROW = 88;
			constexpr Key RIGHTARROW = 89;

			constexpr bool IsControlKey(Key key) noexcept
			{
				return key >= 71 && key <= 89;
			}

			// Numpad (90-104)

			constexpr Key NUMPAD_SLASH = 90;
			constexpr Key NUMPAD_ASTERISK = 91;
			constexpr Key NUMPAD_MINUS = 92;
			constexpr Key NUMPAD_PLUS = 93;
			constexpr Key NUMPAD_DOT = 94;
			constexpr Key NUMPAD_1 = 95;
			constexpr Key NUMPAD_2 = 96;
			constexpr Key NUMPAD_3 = 87;
			constexpr Key NUMPAD_4 = 98;
			constexpr Key NUMPAD_5 = 99;
			constexpr Key NUMPAD_6 = 100;
			constexpr Key NUMPAD_7 = 101;
			constexpr Key NUMPAD_8 = 102;
			constexpr Key NUMPAD_9 = 103;
			constexpr Key NUMPAD_0 = 104;

			constexpr bool IsNumpadKey(Key key) noexcept
			{
				return key >= 90 && key <= 104;
			}

			// Mouse (105-109)

			constexpr Key MOUSE_LEFT = 105;
			constexpr Key MOUSE_RIGHT = 106;
			constexpr Key MOUSE_MIDDLE = 107;
			constexpr Key MOUSE_4 = 108;
			constexpr Key MOUSE_5 = 109;

			constexpr bool IsMouseKey(Key key) noexcept
			{
				return key >= 105 && key <= 109;
			}


			constexpr Key INVALID = -1;
			constexpr Key KEY_COUNT { 110 };
		}

		// @out can be null if you want to check if key is valid
		extern bool KeyToString(Key key, const std::string** out) noexcept;

		// @out can be null if you want to check if key is valid
		extern bool KeyToVirtualKey(Key key, VirtualKey* out) noexcept;

		inline bool IsKeyValid(Key key) noexcept { return KeyToVirtualKey(key, nullptr); }

		extern bool IsKeyPressed(Key key, bool repeat = true) noexcept;
		extern bool IsKeyDown(Key key) noexcept;
		extern bool IsKeyReleased(Key key) noexcept;

		extern bool InputKey(const char* label, Key& key) noexcept;
	}
}
