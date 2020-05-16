#include "math.hpp"
#include "vector.hpp"

SourceSDK::Angle SourceSDK::GetRelativeAngle(const Vector3& from, const Vector3& to) noexcept
{
	const Vector3 local = to - from;

	return Angle(
		RadToDeg(std::acosf(local.X() / from.DistanceTo(to))),
		RadToDeg(std::atan2f(local.Y(), local.X())),
		0.f
	);
}
