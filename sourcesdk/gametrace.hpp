#pragma once
#include "../build.hpp"
#include "vector.hpp"

#include "../util/pad.hpp"

namespace SourceSDK
{
	class BaseEntity;

	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/mathlib/mathlib.h#L148
	struct Plane final
	{
		Vector3 normal;
		float dist;

		UTIL_PAD(0, 4);
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/trace.h#L36
	struct IGameTrace
	{
		Vector3 startPos;
		Vector3 endPos;
		Plane plane;
		float fraction;
		int contents;
		unsigned short dispFlags;
		bool allSolid;
		bool startSolid;
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/cmodel.h#L50
	struct Surface
	{
		const char* name;
		short surfaceProps;
		unsigned short flags;
	};

	// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/public/gametrace.h#L30
	struct GameTrace final : public IGameTrace
	{
		float fractionLeftSolid;
		Surface surface;
		int hit_group;
		short physicsBone;
		BaseEntity* entity;
		int hitbox;

		inline bool DidHit() const noexcept
		{
			return fraction < 1.f || allSolid || startSolid;
		}
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/cmodel.h#L61
	struct Ray final
	{
		Vector4 start;
		Vector4 delta;
		Vector4 start_offset;
		Vector4 extents;

		bool isRay{ true };
		bool isSwept;

		inline Ray(const Vector3& start, const Vector3& end) noexcept
		{
			this->delta = Vector4(end - start);
			this->isSwept = this->delta.IsNull();
			this->start = start;
		}
	};
}
