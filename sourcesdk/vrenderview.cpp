#include "vrenderview.hpp"

#include "../build.hpp"

#include "../util/vmt.hpp"
#include "../util/pattern.hpp"

void SourceSDK::RenderView::SetColorModulation(const VecColor3& color) noexcept
{
	// "CViewRender::DrawOpaqueRenderables"
	// DrawOpaqueRenderables_DrawBrushModels
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"8B 0D ?? ?? ?? ?? 8D 55 ?? 52 8B 01 FF 50 ?? 83 FE 01",
			14
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, const float*>(this, offset, color.data.data());
}

void SourceSDK::RenderView::SetBlend(float alpha) noexcept
{
	// "debug/debugspritewireframe" and "$HDRCOLORSCALE"
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"8B 0D ?? ?? ?? ?? 51 C7 04 ?? 00 00 80 3F 8B 01 FF 50 ?? F3",
			18
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, float>(this, offset, alpha);
}
