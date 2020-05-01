#pragma once
#include "matrix.hpp"
#include "bonemasks.hpp"
#include "entity.hpp"
#include "studio.hpp"
#include "model.hpp"
#include "const.hpp"

#include <functional>

namespace SourceSDK
{
	template <std::size_t Count>
	static inline bool GetBonePosition(const Matrix3x4(&matrices)[Count], int index, Vector& out) noexcept
	{
		if (index + 1 < Count)
		{
			out = Vector(
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
		const Model* GetModel();

		bool SetupBones(Matrix3x4* matrices, int max_bones, BoneMaskFlags bone_mask, float current_time);

		bool EnumerateBones(std::function<bool(int, const Studio::Bone*)>) noexcept;

		bool GetBoneIndex(const std::string& bone_name, int& bone_idx) noexcept;
		bool FindBoneIndex(const std::string& bone_name, int& bone_idx) noexcept;
		bool GetBonePosition(int bone_idx, Vector& bone_pos, float time = 0.f) noexcept;

		inline bool GetBonePosition(const std::string& bone_name, Vector& bone_pos, float time = 0.f) noexcept
		{
			if (int bone; GetBoneIndex(bone_name, bone))
				if (GetBonePosition(bone, bone_pos, time))
					return true;
			
			return false;
		}
	};
}
