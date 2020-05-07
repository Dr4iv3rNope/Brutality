#pragma once
#include "matrix.hpp"
#include "bonemasks.hpp"
#include "entity.hpp"
#include "studio.hpp"
#include "model.hpp"
#include "const.hpp"
#include "globals.hpp"

#include <functional>

namespace SourceSDK
{
	template <std::size_t Count>
	static inline bool GetBonePosition(const Matrix3x4(&matrices)[Count], int index, Vector3& out) noexcept
	{
		if (index + 1 < Count)
		{
			out = Vector3(
				matrices[index][0][3],
				matrices[index][1][3],
				matrices[index][2][3]
			);
			return true;
		}
		else
			return false;
	}

	class Renderable final
	{
	public:
		inline BaseEntity* ToEntity() noexcept
		{
			return (BaseEntity*)(std::uintptr_t(this) - BaseEntity::RENDERABLE_VMT_OFFSET);
		}

		// can be null!
		const Model* GetModel() noexcept;

		bool SetupBones(Matrix3x4* matrices, int max_bones, BoneMask bone_mask, float current_time = globals->curTime);

		bool EnumerateBones(std::function<bool(int, const Studio::Bone*)>) noexcept;

		bool GetBoneIndex(const std::string& bone_name, int& bone_idx) noexcept;
		bool FindBoneIndex(const std::string& bone_name, int& bone_idx) noexcept;
		bool GetBonePosition(int bone_idx, Vector3& bone_pos, float time = globals->curTime) noexcept;

		inline bool GetBonePosition(const std::string& bone_name, Vector3& bone_pos, float time = globals->curTime) noexcept
		{
			if (int bone; GetBoneIndex(bone_name, bone))
				if (GetBonePosition(bone, bone_pos, time))
					return true;
			
			return false;
		}
	};
}
