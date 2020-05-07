#include "vector.hpp"
#include "worldtoscreen.hpp"

SourceSDK::W2SOutput SourceSDK::Vector3::ToScreen() const noexcept
{
	return WorldToScreen(*this);
}

SourceSDK::Angle& SourceSDK::Angle::NormalizeThis() noexcept
{
	Pitch() = std::remainder(Pitch(), 360.f);
	Yaw() = std::remainder(Yaw(), 180.f);
	Roll() = std::remainder(Roll(), 360.f);

	return *this;
}


SourceSDK::Vector3 SourceSDK::Angle::Forward() const noexcept
{
	float rp = RadPitch();
	float ry = RadYaw();

	return Forward(
		std::sin(rp), std::sin(ry),
		std::cos(rp), std::cos(ry)
	);
}

SourceSDK::Vector3 SourceSDK::Angle::Right() const noexcept
{
	float rp = RadPitch();
	float ry = RadYaw();
	float rr = RadRoll();

	return Right(
		std::sin(rp), std::sin(ry), std::sin(rr),
		std::cos(rp), std::cos(ry), std::cos(rr)
	);
}

SourceSDK::Vector3 SourceSDK::Angle::Up() const noexcept
{
	float rp = RadPitch();
	float ry = RadYaw();
	float rr = RadRoll();

	return Up(
		std::sin(rp), std::sin(ry), std::sin(rr),
		std::cos(rp), std::cos(ry), std::cos(rr)
	);
}

void SourceSDK::Angle::AngleVectors(Vector3* forward, Vector3* right, Vector3* up) const noexcept
{
	if (!forward && !right && !up)
	{
		UTIL_DEBUG_ASSERT(false);
		return;
	}

	float rp = RadPitch();
	float ry = RadYaw();

	float sp = std::sin(rp);
	float sy = std::sin(ry);

	float cp = std::cos(rp);
	float cy = std::cos(ry);

	if (forward)
		*forward = Forward(sp, sy, cp, cy);

	if (right || up)
	{
		float rr = RadRoll();

		float sr = std::sin(rr);
		float cr = std::cos(rr);

		if (right)
			*right = Right(sp, sy, sr, cp, cy, cr);

		if (up)
			*up = Up(sp, sy, sr, cp, cy, cr);
	}
}
