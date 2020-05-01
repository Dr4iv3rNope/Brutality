#pragma once
#include "renderable.hpp"
#include "sdk.hpp"
#include "modelcache.hpp"

#include "../util/vmt.hpp"
#include "../util/xorstr.hpp"
#include "../util/memory.hpp"
#include "../util/strings.hpp"

#include "../util/debug/assert.hpp"

const SourceSDK::Model* SourceSDK::Renderable::GetModel()
{
	// "mod_studio: MOVETYPE_FOLLOW with no model."
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"8B 47 04 53 8D 5F 04 8B CB 89 5D FC FF 50 ?? 85 C0", 14
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<Model*>(this, offset);
}


bool SourceSDK::Renderable::SetupBones(Matrix3x4* matrices, int max_bones, BoneMaskFlags bone_mask, float current_time)
{
	UTIL_DEBUG_ASSERT(matrices);

	// "C_BaseAnimating::SetupBones"
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("50 FF 73 ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? A1", 2)
	};
	#endif

	return Util::Vmt::CallMethod<bool, Matrix3x4*, int, int, float>
		(this, offset, matrices, max_bones, bone_mask, current_time);
}

bool SourceSDK::Renderable::EnumerateBones(std::function<bool(int, const Studio::Bone*)> callback) noexcept
{
	UTIL_DEBUG_ASSERT(callback);

	if (auto studio = mdlCache->GetStudioHDR(ToEntity()->ToRenderable()->GetModel()))
	{
		for (auto bone_idx = 0; bone_idx < studio->GetBoneCount(); bone_idx++)
			if (auto bone = studio->GetBone(bone_idx))
				if (callback(bone_idx, bone))
					break;

		return true;
	}
	else
		return false;
}

bool SourceSDK::Renderable::GetBoneIndex(const std::string& bone_name, int& bone_out) noexcept
{
	bone_out = -1;

	return EnumerateBones([bone_name, &bone_out] (int bone_idx, const Studio::Bone* bone) -> bool
	{
		if (bone->GetName() == bone_name)
		{
			bone_out = bone_idx;
			return true;
		}
		else
			return false;
	}) && bone_out != -1;
}

bool SourceSDK::Renderable::FindBoneIndex(const std::string& bone_name, int& bone_out) noexcept
{
	bone_out = -1;

	return EnumerateBones([bone_name, &bone_out] (int bone_idx, const Studio::Bone* bone) -> bool
	{
		if (Util::ToLower(std::string(bone->GetName())).find(bone_name) != std::string::npos)
		{
			bone_out = bone_idx;
			return true;
		}
		else
			return false;
	}) && bone_out != -1;
}

bool SourceSDK::Renderable::GetBonePosition(int bone_idx, Vector& bone_pos, float time) noexcept
{
	static Matrix3x4 matrices[MAX_STUDIO_BONES];

	return this->SetupBones(matrices, MAX_STUDIO_BONES, int(BoneMask::Anything), time) &&
		SourceSDK::GetBonePosition(matrices, bone_idx, bone_pos);
}
