#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

namespace SourceSDK
{
	template <typename T>
	constexpr T MATH_PI = T(M_PI);

	template <typename T>
	constexpr inline T RadToDeg(T rad) noexcept { return rad * T(180) / MATH_PI<T>; }

	template <typename T>
	constexpr inline T DegToRad(T rad) noexcept { return rad * MATH_PI<T> / T(180); }

	struct Angle;
	struct Vector3;
	extern Angle GetRelativeAngle(const Vector3& from, const Vector3& to) noexcept;
}
