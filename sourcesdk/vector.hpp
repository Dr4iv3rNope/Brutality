#pragma once
#include <cmath>

// v1 *operator* v2
#define __SOURCE_SDK_VECTOR_MATH(_operator) \
	inline Vector operator##_operator##(const Vector& vector) const noexcept { \
		return Vector(x _operator vector.x, y _operator vector.y, z _operator vector.z); } 

// v1 *operator*= v2
#define __SOURCE_SDK_VECTOR_EQ_MATH(_operator) \
	inline Vector& operator##_operator##=(const Vector& vector) noexcept { \
		x _operator##= vector.x; y _operator##= vector.y; z _operator##= vector.z; return *this; }

// v1 *operator* v2
// v1 *operator*= v2
#define __SORUCE_SDK_VECTOR_BASE_MATH(_operator) \
	__SOURCE_SDK_VECTOR_MATH(_operator); \
	__SOURCE_SDK_VECTOR_EQ_MATH(_operator);

namespace SourceSDK
{
	struct W2SOutput;

	struct Vector2 final
	{
		float x, y;

		inline Vector2(float x = 0.f, float y = 0.f) noexcept
		{
			this->x = x;
			this->y = y;
		}

		inline Vector2(float xy) noexcept : Vector2(xy, xy) {}

		inline bool IsNull() const noexcept
		{
			return x == y && y == 0.f;
		}
	};

	struct Vector final
	{
		float x, y, z;

		inline Vector(float x = 0.f, float y = 0.f, float z = 0.f) noexcept
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		inline Vector(float xyz) noexcept : Vector(xyz, xyz, xyz) {}

		inline bool IsNull() const noexcept
		{
			return x == y && y == z && z == 0.f;
		}

		W2SOutput ToScreen() const noexcept;

		__SORUCE_SDK_VECTOR_BASE_MATH(+);
		__SORUCE_SDK_VECTOR_BASE_MATH(-);
		__SORUCE_SDK_VECTOR_BASE_MATH(/);
		__SORUCE_SDK_VECTOR_BASE_MATH(*);

		inline bool operator>(const Vector& vec) const noexcept
		{
			return x > vec.x && y > vec.y && z > vec.z;
		}

		inline bool operator<(const Vector& vec) const noexcept
		{
			return x < vec.x && y < vec.y && z < vec.z;
		}

		inline bool operator>=(const Vector& vec) const noexcept
		{
			return x >= vec.x && y >= vec.y && z >= vec.z;
		}

		inline bool operator<=(const Vector& vec) const noexcept
		{
			return x <= vec.x && y <= vec.y && z <= vec.z;
		}

		inline auto Pitch() const noexcept { return x; }
		inline auto& Pitch() noexcept { return x; }

		inline auto Yaw() const noexcept { return y; }
		inline auto& Yaw() noexcept { return y; }

		inline auto Roll() const noexcept { return z; }
		inline auto& Roll() noexcept { return z; }

		inline auto& NormalizeThisAngle() noexcept
		{
			this->Pitch() = std::remainder(this->Pitch(), 360.f);
			this->Yaw() = std::remainder(this->Yaw(), 180.f);
			this->Roll() = std::remainder(this->Roll(), 360.f);

			return *this;
		}

		inline auto NormalizeAngle() const noexcept
		{
			return Vector(Pitch(), Yaw(), Roll()).NormalizeThisAngle();
		}

		inline float Length() const noexcept
		{
			return std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
		}

		inline float Distance(const Vector& vec) const noexcept
		{
			return std::sqrt(
				std::pow(x - vec.x, 2) +
				std::pow(y - vec.y, 2) +
				std::pow(z - vec.z, 2)
			);
		}
	};

	using VecColor = Vector;
	using Angle = Vector;
	using RadianEuler = Angle;

	struct Vector4
	{
		float x, y, z, w;

		inline Vector4(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		inline Vector4(float xyzw) : Vector4(xyzw, xyzw, xyzw, xyzw) {}
		inline Vector4(const Vector& vec3) : Vector4(vec3.x, vec3.y, vec3.z) {}

		inline bool IsNull() const noexcept
		{
			return x == y && y == z && z == w && w == 0.f;
		}
	};

	using Quaternion = Vector4;
}
