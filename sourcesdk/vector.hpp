#pragma once
#include "math.hpp"

#include <array>
#include <cmath>
#include <algorithm>
#include <type_traits>

#include "../util/debug/assert.hpp"

namespace SourceSDK
{
	struct W2SOutput;

	//
	// base vector for all vectors!
	//
	// if you want to use vector as argument,
	// you should use MyVector::Base typedef
	//
	// example:
	// ```
	// struct MyVector : public BaseVector<int, 3> { ... };
	// int SomeWork(const MyVector::Base& my_vec) { ... }
	// ```
	//
	// oh, i forgot to mention that you MUST implement
	// copy constructors using MyVector::Base typedef
	//
	template <typename T, unsigned C>
	struct BaseVector
	{
		static_assert(C >= 2, "Vector cant be with 1 item");
		static_assert(std::is_arithmetic<T>(), "Vector data must be arthmetic!");


		enum { ItemCount = C };
		using Type = T;
		using Array = std::array<Type, ItemCount>;
		using Base = BaseVector<Type, ItemCount>;

		Array data;

		inline BaseVector(T all = T {}) noexcept { data.fill(all); }
		inline BaseVector(const Array& data) noexcept : data { data } {}

		BaseVector(const BaseVector&) noexcept = default;
		~BaseVector() noexcept = default;


		//
		// Array iterators
		//

		inline auto begin() noexcept { return data.begin(); }
		inline auto rbegin() noexcept { return data.rbegin(); }
		inline auto cbegin() noexcept { return data.cbegin(); }

		inline auto end() noexcept { return data.end(); }
		inline auto rend() noexcept { return data.rend(); }
		inline auto cend() noexcept { return data.cend(); }


		inline Type Get(std::size_t idx) const noexcept;
		inline Type& Get(std::size_t idx) noexcept;

		inline Type operator[](std::size_t idx) const noexcept { return Get(idx); }
		inline Type& operator[](std::size_t idx) noexcept { return Get(idx); }


		inline bool IsNull() const noexcept;

		//
		// math part
		//

		inline BaseVector operator+(const BaseVector& vec) const noexcept;
		inline BaseVector operator-(const BaseVector& vec) const noexcept;
		inline BaseVector operator/(const BaseVector& vec) const noexcept;
		inline BaseVector operator*(const BaseVector& vec) const noexcept;

		inline BaseVector& operator+=(const BaseVector& vec) noexcept;
		inline BaseVector& operator-=(const BaseVector& vec) noexcept;
		inline BaseVector& operator/=(const BaseVector& vec) noexcept;
		inline BaseVector& operator*=(const BaseVector& vec) noexcept;

		inline T Length() const noexcept;
		inline T DistanceTo(const BaseVector& vec) const noexcept;
	};
	
	enum Vector_ { Vector_X, Vector_Y, Vector_Z, Vector_W };


	struct Vector2 : public BaseVector<float, 2>
	{
		inline Vector2(float x, float y) noexcept : BaseVector({ x, y }) {}
		inline Vector2(float all = 0.f) noexcept : BaseVector(all) {}
		inline Vector2(const Base& vec2) noexcept : BaseVector(vec2) {}

		inline float& X() noexcept { return Get(Vector_X); }
		inline float X() const noexcept { return Get(Vector_X); }
		inline float& Y() noexcept { return Get(Vector_Y); }
		inline float Y() const noexcept { return Get(Vector_Y); }
	};


	struct Vector3 : public BaseVector<float, 3>
	{
		inline Vector3(float x, float y, float z) noexcept : BaseVector({ x, y, z }) {}
		inline Vector3(float all = 0.f) noexcept : BaseVector(all) {}
		inline Vector3(const Base& vec3) noexcept : BaseVector(vec3) {}

		inline float& X() noexcept { return Get(Vector_X); }
		inline float X() const noexcept { return Get(Vector_X); }
		inline float& Y() noexcept { return Get(Vector_Y); }
		inline float Y() const noexcept { return Get(Vector_Y); }
		inline float& Z() noexcept { return Get(Vector_Z); }
		inline float Z() const noexcept { return Get(Vector_Z); }

		W2SOutput ToScreen() const noexcept;
	};


	struct Vector4 : public BaseVector<float, 4>
	{
		inline Vector4(float x, float y, float z, float w) noexcept : BaseVector({ x, y, z, w }) {}
		inline Vector4(float all = 0.f) noexcept : BaseVector(all) {}
		inline Vector4(const Base& vec4) noexcept : BaseVector(vec4) {}

		inline Vector4(const Vector3::Base& vec3) noexcept
			: Vector4(vec3[Vector_X], vec3[Vector_Y], vec3[Vector_Z], 0.f) {}

		inline float& X() noexcept { return Get(Vector_X); }
		inline float X() const noexcept { return Get(Vector_X); }
		inline float& Y() noexcept { return Get(Vector_Y); }
		inline float Y() const noexcept { return Get(Vector_Y); }
		inline float& Z() noexcept { return Get(Vector_Z); }
		inline float Z() const noexcept { return Get(Vector_Z); }
		inline float& W() noexcept { return Get(Vector_W); }
		inline float W() const noexcept { return Get(Vector_W); }
	};

	using Quaternion = Vector4;
	
	
	enum Angle_ { Angle_P, Angle_Y, Angle_R, };

	struct Angle : public BaseVector<float, 3>
	{
		inline Angle(float p, float y, float r) noexcept : BaseVector({ p, y, r }) {}
		inline Angle(float all = 0.f) noexcept : BaseVector(all) {}
		inline Angle(const Base& ang) noexcept : BaseVector(ang) {}


		inline float& Pitch() noexcept { return Get(Angle_P); }
		inline float Pitch() const noexcept { return Get(Angle_P); }
		inline float& Yaw() noexcept { return Get(Angle_Y); }
		inline float Yaw() const noexcept { return Get(Angle_Y); }
		inline float& Roll() noexcept { return Get(Angle_R); }
		inline float Roll() const noexcept { return Get(Angle_R); }

		//
		// radians
		//

		inline float RadPitch() const noexcept { return DegToRad(Pitch()); }
		inline float RadYaw() const noexcept { return DegToRad(Yaw()); }
		inline float RadRoll() const noexcept { return DegToRad(Roll()); }

		// converts whole angle to radian angle
		inline Angle ToRad() const noexcept { return Angle(RadPitch(), RadYaw(), RadRoll()); }


		//
		// degrees
		//

		inline float DegPitch() const noexcept { return RadToDeg(Pitch()); }
		inline float DegYaw() const noexcept { return RadToDeg(Yaw()); }
		inline float DegRoll() const noexcept { return RadToDeg(Roll()); }

		// converts whole angle to degrees angle
		inline Angle ToDeg() const noexcept { return Angle(DegPitch(), DegYaw(), DegRoll()); }


		Angle& NormalizeThis() noexcept;
		inline Angle Normalize() const noexcept { return Angle(*this).NormalizeThis(); }

		// @sp	sin rad pitch
		// @sy	sin rad yaw
		//
		// @cp	cos rad pitch
		// @cy	cos rad yaw
		static inline Vector3 Forward(float sp, float sy, float cp, float cy) noexcept
		{
			return Vector3(
				cp * cy,
				cp * sy,
				-sp
			);
		}

		// @sp	sin rad pitch
		// @sy	sin rad yaw
		// @sr	sin rad roll
		//
		// @cp	cos rad pitch
		// @cy	cos rad yaw
		// @cr	cos rad roll
		static inline Vector3 Right(float sp, float sy, float sr,
									float cp, float cy, float cr
		) noexcept
		{
			return Vector3(
				-1 * sr * sp * cy + -1 * cr * -sy,
				-1 * sr * sp * sy + -1 * cr * cy,
				-1 * sr * cp
			);
		}

		// @sp	sin rad pitch
		// @sy	sin rad yaw
		// @sr	sin rad roll
		//
		// @cp	cos rad pitch
		// @cy	cos rad yaw
		// @cr	cos rad roll
		static inline Vector3 Up(float sp, float sy, float sr,
								 float cp, float cy, float cr
		) noexcept
		{
			return Vector3(
				cr * sp * cy + -sr * -sy,
				cr * sp * sy + -sr * cy,
				cr * cp
			);
		}

		//
		// if you want to use Forward, Right, Up together
		// you shall use AngleVectors function for faster calculations
		//

		Vector3 Forward() const noexcept;
		Vector3 Right() const noexcept;
		Vector3 Up() const noexcept;

		// if forward & right & up is nullptr
		// that function will assert (ONLY IN DEBUG)
		// so that can be marked as bug!
		void AngleVectors(
			Vector3* forward = nullptr,
			Vector3* right = nullptr,
			Vector3* up = nullptr
		) const noexcept;
	};

	using RadianEuler = Angle;


	enum VecColor_ { VecColor_R, VecColor_G, VecColor_B, VecColor_A };

	struct VecColor3 : public BaseVector<float, 3>
	{
	public:
		inline VecColor3(float r, float g, float b) noexcept : BaseVector({ r, g, b }) {}
		inline VecColor3(float all = 0.f) noexcept : BaseVector(all) {}
		inline VecColor3(const Base& vec) noexcept : BaseVector(vec) {}

		inline VecColor3& NormalizeThis() noexcept
		{
			Red() = std::clamp(Red(), 0.f, 1.f);
			Green() = std::clamp(Green(), 0.f, 1.f);
			Blue() = std::clamp(Blue(), 0.f, 1.f);

			return *this;
		}

		inline VecColor3 Normalize() const noexcept
		{
			return VecColor3(*this).NormalizeThis();
		}

		inline float& Red() noexcept { return Get(VecColor_R); }
		inline float Red() const noexcept { return Get(VecColor_R); }
		inline float& Green() noexcept { return Get(VecColor_G); }
		inline float Green() const noexcept { return Get(VecColor_G); }
		inline float& Blue() noexcept { return Get(VecColor_B); }
		inline float Blue() const noexcept { return Get(VecColor_B); }
	};

	struct VecColor4 : public BaseVector<float, 4>
	{
	public:
		inline VecColor4(float r, float g, float b, float a = 1.f) noexcept : BaseVector({ r, g, b, a }) {}
		inline VecColor4(float all = 0.f) noexcept : BaseVector(all) {}
		inline VecColor4(const Base& vec) noexcept : BaseVector(vec) {}

		inline VecColor4& NormalizeThis() noexcept
		{
			Red() = std::clamp(Red(), 0.f, 1.f);
			Green() = std::clamp(Green(), 0.f, 1.f);
			Blue() = std::clamp(Blue(), 0.f, 1.f);
			Alpha() = std::clamp(Alpha(), 0.f, 1.f);

			return *this;
		}

		inline VecColor4 Normalize() const noexcept
		{
			return VecColor4(*this).NormalizeThis();
		}

		inline float& Red() noexcept { return Get(VecColor_R); }
		inline float Red() const noexcept { return Get(VecColor_R); }
		inline float& Green() noexcept { return Get(VecColor_G); }
		inline float Green() const noexcept { return Get(VecColor_G); }
		inline float& Blue() noexcept { return Get(VecColor_B); }
		inline float Blue() const noexcept { return Get(VecColor_B); }
		inline float& Alpha() noexcept { return Get(VecColor_A); }
		inline float Alpha() const noexcept { return Get(VecColor_A); }
	};
}

template<typename T, unsigned C>
inline T SourceSDK::BaseVector<T, C>::Get(std::size_t idx) const noexcept
{
	UTIL_DEBUG_ASSERT(idx < C);
	return data[idx];
}

template<typename T, unsigned C>
inline T& SourceSDK::BaseVector<T, C>::Get(std::size_t idx) noexcept
{
	UTIL_DEBUG_ASSERT(idx < C);
	return data[idx];
}

template<typename T, unsigned C>
inline bool SourceSDK::BaseVector<T, C>::IsNull() const noexcept
{
	for (auto value : data)
		if (value != T(0))
			return false;

	return true;
}


template<typename T, unsigned C>
inline SourceSDK::BaseVector<T, C> SourceSDK::BaseVector<T, C>::operator+(const BaseVector& vec) const noexcept
{
	return BaseVector(*this) += vec;
}

template<typename T, unsigned C>
inline SourceSDK::BaseVector<T, C> SourceSDK::BaseVector<T, C>::operator-(const BaseVector& vec) const noexcept
{
	return BaseVector(*this) -= vec;
}

template<typename T, unsigned C>
inline SourceSDK::BaseVector<T, C> SourceSDK::BaseVector<T, C>::operator/(const BaseVector& vec) const noexcept
{
	return BaseVector(*this) /= vec;
}

template<typename T, unsigned C>
inline SourceSDK::BaseVector<T, C> SourceSDK::BaseVector<T, C>::operator*(const BaseVector& vec) const noexcept
{
	return BaseVector(*this) *= vec;
}


template<typename T, unsigned C>
inline SourceSDK::BaseVector<T, C>& SourceSDK::BaseVector<T, C>::operator+=(const BaseVector& vec) noexcept
{
	for (auto i = 0; i < C; i++)
		Get(i) += vec[i];

	return *this;
}

template<typename T, unsigned C>
inline SourceSDK::BaseVector<T, C>& SourceSDK::BaseVector<T, C>::operator-=(const BaseVector& vec) noexcept
{
	for (auto i = 0; i < C; i++)
		Get(i) -= vec[i];

	return *this;
}

template<typename T, unsigned C>
inline SourceSDK::BaseVector<T, C>& SourceSDK::BaseVector<T, C>::operator/=(const BaseVector& vec) noexcept
{
	for (auto i = 0; i < C; i++)
		Get(i) /= vec[i];

	return *this;
}

template<typename T, unsigned C>
inline SourceSDK::BaseVector<T, C>& SourceSDK::BaseVector<T, C>::operator*=(const BaseVector& vec) noexcept
{
	for (auto i = 0; i < C; i++)
		Get(i) *= vec[i];

	return *this;
}


template<typename T, unsigned C>
inline T SourceSDK::BaseVector<T, C>::Length() const noexcept
{
	T pow_sum = 0.f;

	for (auto i = 0; i < C; i++)
		pow_sum += std::pow<T>(Get(i), T(2));

	return std::sqrt(pow_sum);
}

template<typename T, unsigned C>
inline T SourceSDK::BaseVector<T, C>::DistanceTo(const BaseVector& vec) const noexcept
{
	return (*this - vec).Length();
}
