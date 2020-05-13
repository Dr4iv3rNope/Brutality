#include "modelrender.hpp"
#include "../build.hpp"

#include "../util/vmt.hpp"
#include "../util/pattern.hpp"

#include "../hooks/drawmodelexecute.hpp"

std::size_t SourceSDK::ModelRender::GetDrawModelExecuteIndex()
{
	// "C_BaseAnimating::DrawModel"
	// C_BaseAnimating::InternalDrawModel
	// C_BaseAnimating::DoInternalDrawModel
	#if BUILD_GAME_IS_GMOD
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

void SourceSDK::ModelRender::ForcedMaterialOverride(Material* material, OverrideType type)
{
	// "CShadowDepthView::Draw"
	#if BUILD_GAME_IS_GMOD
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
