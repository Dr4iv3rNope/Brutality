#pragma once
#include <cstdint>
#include <array>

namespace SourceSDK
{
	enum class ColorType
	{
		Red, Green, Blue, Alpha
	};

	struct ByteColor final
	{
		std::array<std::uint8_t, 4> colors { 0, 0, 0, 0 };

		constexpr inline ByteColor(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) noexcept
		{
			colors = { r, g, b, a };
		}

		constexpr inline ByteColor(std::uint32_t hex_color)
		{
			auto* hex_color_arr { (std::uint8_t*)hex_color };

			colors =
			{
				hex_color_arr[0],	// red
				hex_color_arr[1],	// green
				hex_color_arr[2],	// blue
				hex_color_arr[3]	// alpha
			};
		}

		constexpr inline auto& operator[](ColorType type) noexcept
		{
			return this->colors[int(type)];
		}

		constexpr inline auto operator[](ColorType type) const noexcept
		{
			return this->colors[int(type)];
		}
	};
}
