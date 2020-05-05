#pragma once
#include "../../config/variable.hpp"

namespace ImGui
{
	namespace Custom
	{
		namespace Variable
		{
			extern bool Unsigned(Config::UInt32& value) noexcept;
			extern bool LimitedUnsigned(Config::LUInt32& value) noexcept;

			extern bool Signed(Config::Int32& value) noexcept;
			extern bool LimitedSigned(Config::LInt32& value) noexcept;

			extern bool Float(Config::Float& value) noexcept;
			extern bool LimitedFloat(Config::LFloat& value) noexcept;

			extern bool String(Config::String<char>& value) noexcept;
			extern bool LimitedString(Config::LimitedString<char>& value) noexcept;

			extern bool Boolean(Config::Bool& boolean) noexcept;
			extern bool Enum(Config::Enum& value) noexcept;
			extern bool Color(Config::Color& value) noexcept;
			extern bool Key(Config::Key& value) noexcept;
		}
	}
}