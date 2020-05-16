#pragma once
#include <array>

namespace SourceSDK
{
	template <typename T, std::size_t X, std::size_t Y>
	using Matrix = std::array<std::array<T, Y>, X>;

	using Matrix3x3 = Matrix<float, 3, 3>;
	using Matrix3x4 = Matrix<float, 3, 4>;
	using Matrix4x4 = Matrix<float, 4, 4>;

	using VMatrix = Matrix4x4;
}
