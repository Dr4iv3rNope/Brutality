#pragma once

namespace SourceSDK
{
	template <unsigned X, unsigned Y>
	using Matrix = float[X][Y];

	using Matrix3x3 = Matrix<3, 3>;
	using Matrix3x4 = Matrix<3, 4>;
	using Matrix4x4 = Matrix<4, 4>;

	using VMatrix = Matrix4x4;
}
