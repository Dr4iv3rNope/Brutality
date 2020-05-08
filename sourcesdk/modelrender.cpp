#include "modelrender.hpp"
#include "sdk.hpp"

#include "../util/vmt.hpp"
#include "../util/pattern.hpp"

#include "../hooks/drawmodelexecute.hpp"

std::size_t SourceSDK::ModelRender::GetDrawModelExecuteIndex()
{
	// "C_BaseAnimating::DrawModel"
	// C_BaseAnimating::InternalDrawModel
	// C_BaseAnimating::DoInternalDrawModel
	//
	// or
	//
	// call    sub_100C11C0    ; #STR: "lod: %d/%d\n", "tris: %d\n", "hardware bones: %d\n", "num batches: %d\n" ...
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"52 8D 95 ?? ?? ?? ?? 52 8B 01 8D 95 ?? ?? ?? ?? 52 FF 50",
			19
		)) / 4
	};
	#endif

	return offset;
}

void SourceSDK::ModelRender::DrawModelExecute(const SourceSDK::DrawModelState& state,
											  const SourceSDK::ModelRenderInfo& info,
											  SourceSDK::Matrix3x4* boneToWorld)
{
	if (!Hooks::oldDrawModelExecute)
		Util::Vmt::CallMethod
		<void, const SourceSDK::DrawModelState&, const SourceSDK::ModelRenderInfo&, SourceSDK::Matrix3x4*>
		(this, GetDrawModelExecuteIndex(), state, info, boneToWorld);
	else
		Hooks::OldDrawModelExecute(state, info, boneToWorld);
}

void SourceSDK::ModelRender::ForcedMaterialOverride(Material* material, OverrideType type)
{
	// "CShadowDepthView::Draw"
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"8B 0D ?? ?? ?? ?? 6A 02 6A 00 8B 01 FF 50 ?? 8B 0D",
			14
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, Material*, OverrideType>(this, offset, material, type);
}
