#pragma once
#include <cstdint>
#include <string>

#include "../../sourcesdk/buttoncode.hpp"

namespace ImGui
{
	namespace Custom
	{
		using VirtualKey = std::uint8_t;

		enum class Key
		{
			_Invalid = -1,

			// functional keys (F*)

			F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
			F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,

			// Number keys

			N1, N2, N3, N4, N5, N6, N7, N8, N9, N0,

			// letters

			A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

			// Symbols

			Grave,					// ~
			LeftSquareBracket,		// [
			RIGHT_SQUARE_BRACKET,	// ]
			Semicolon,				// ;
			Apostrophe,				// '
			Backslash,
			Comma,					// ,
			Dot,					// .
			Slash,					// /
			Minus,					// -
			Equal,					// =

			// control keys

			Tab,
			Capslock,
			Shift,
			Ctrl,
			Alt,
			Space,
			Enter,
			Backspace,
			Insert,
			Home,
			PageUp,
			Delete,
			End,
			PageDown,
			ScrollLock,
			UpArrow,
			DownArrow,
			LeftArrow,
			RightArrow,

			// numpad keys (90-104)

			NumpadSlash,
			NumpadAsterisk, // *
			NumpadMinus,
			NumpadPlus,
			NumpadDot,
			Numpad1,
			Numpad2,
			Numpad3,
			Numpad4,
			Numpad5,
			Numpad6,
			Numpad7,
			Numpad8,
			Numpad9,
			Numpad0,

			// Mouse (105-109)

			MouseLeft,
			MouseRight,
			MouseMiddle,
			Mouse4,
			Mouse5,

			_KeyCount
		};

		constexpr bool IsFunctionalKey(Key key) noexcept
		{
			return key >= Key::F1 && key <= Key::F24;
		}

		constexpr bool IsNumberKey(Key key) noexcept
		{
			return key >= Key::N1 && key <= Key::N0;
		}

		constexpr bool IsLetterKey(Key key) noexcept
		{
			return key >= Key::A && key <= Key::Z;
		}

		constexpr bool IsSymbolKey(Key key) noexcept
		{
			return key >= Key::Grave && key <= Key::Equal;
		}

		constexpr bool IsControlKey(Key key) noexcept
		{
			return key >= Key::Tab && key <= Key::RightArrow;
		}

		constexpr bool IsNumpadKey(Key key) noexcept
		{
			return key >= Key::NumpadSlash && key <= Key::Numpad0;
		}

		constexpr bool IsMouseKey(Key key) noexcept
		{
			return key >= Key::MouseLeft && key <= Key::Mouse5;
		}

		// @out can be null if you want to check if key is valid
		extern bool KeyToString(Key key, const std::string** out) noexcept;

		// @out can be null if you want to check if key is valid
		extern bool KeyToVirtualKey(Key key, VirtualKey* out) noexcept;

		// @out can be null if you want to check if key is valid
		extern bool KeyToButtonCode(Key key, SourceSDK::ButtonCode* out) noexcept;

		inline bool IsKeyValid(Key key) noexcept { return KeyToVirtualKey(key, nullptr); }

		extern bool IsKeyPressed(Key key, bool repeat = true) noexcept;
		extern bool IsKeyDown(Key key) noexcept;
		extern bool IsKeyReleased(Key key) noexcept;

		// uses winapi GetAsyncKeyState
		extern short GetAsyncKeyState(Key key) noexcept;

		extern bool InputKey(const char* label, Key& key) noexcept;
	}
}
