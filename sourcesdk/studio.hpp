#pragma once
#include <cstdint>

#include "sdk.hpp"
#include "const.hpp"
#include "vector.hpp"
#include "matrix.hpp"

#include "../util/pad.hpp"

#define __SOURCE_SDK_STUDIO_DECL_GETNAME \
	int nameIndex; \
	inline const char* GetName() const noexcept { \
		return this->nameIndex ? reinterpret_cast<const char*>(this) + this->nameIndex : nullptr; }

#define __SOURCE_SDK_STUDIO_DECL_DATA(typeName, name) \
	private: int _##name##Count, _##name##DataOffset; \
	public: inline const typeName* Get##name(int idx) const noexcept { \
		return idx >= 0 && idx < this->_##name##Count \
		? reinterpret_cast<typeName*>(std::uintptr_t(this) + this->_##name##DataOffset) + idx \
		: nullptr; } \
	inline int Get##name##Count() const noexcept { return this->_##name##Count; }

namespace SourceSDK
{
	namespace Studio
	{
		static constexpr int BONE_CONTROLLER_COUNT { 6 };

		// public/studio.h:234
		struct Bone final
		{
			__SOURCE_SDK_STUDIO_DECL_GETNAME;
			int parentIndex;
			int boneController[BONE_CONTROLLER_COUNT];

			Vector pos;
			Quaternion quat;
			RadianEuler rot;

			Vector posScale;
			Vector rotScale;

			Matrix3x4 poseToBone;
			Quaternion alignment;

			int flags;
			int procType;
			int procIndex;
			
			mutable int physicsBone;

			int surfacePropIndex;
			int contents;

		private:
			int _unused[8];
		};

		// public/studio.h:359
		struct Hitbox final
		{
			int boneIndex;
			int group;

			Vector bbMin, bbMax;

			__SOURCE_SDK_STUDIO_DECL_GETNAME;

		private:
			int _unused[8];
		};

		// public/studio.h:1553
		struct HitboxSet final
		{
			__SOURCE_SDK_STUDIO_DECL_GETNAME;
			__SOURCE_SDK_STUDIO_DECL_DATA(Hitbox, Hitbox);
		};
	}

	// public/studio.h:1965
	struct StudioHDR final
	{
		int id;
		int version;
		CRC32 checksum;
		char name[MAX_STUDIO_NAME_LENGTH];
		int length;

		Vector eyePos, illumPos, hullMin, hullMax, viewBBMin, viewBBMax;

		int flags;

		__SOURCE_SDK_STUDIO_DECL_DATA(Studio::Bone, Bone);
		UTIL_PAD(bone_controller, 8);
		__SOURCE_SDK_STUDIO_DECL_DATA(Studio::HitboxSet, HitboxSet);
		__SOURCE_SDK_STUDIO_DECL_DATA(Studio::Hitbox, Hitbox);
	};
}
