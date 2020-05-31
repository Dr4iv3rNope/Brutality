//
// DO NOT INCLUDE THIS FILE
//

#pragma once
#include "../brutality.hpp"

#include "../imgui/imgui.h"

#include "../util/xorstr.hpp"

#include "../features/esp.hpp"
#include "../features/spectatorlist.hpp"
#include "../features/textradar.hpp"
#include "../features/crosshair.hpp"
#include "../features/backtrack.hpp"

void OnOverlayRender(ImDrawList* draw, bool isMenuOpen)
{
	Features::Esp::Draw(draw);
	Features::Crosshair::Draw(draw, isMenuOpen);
	Features::TextRadar::Draw(draw);
	Features::SpectatorList::Draw(draw);
	Features::Backtrack::DrawBacktrack(draw);

	// draw black transparent background
	// is menu is opened
	if (isMenuOpen)
		draw->AddRectFilledMultiColor(
			ImVec2(0.f, 0.f),
			ImGui::GetIO().DisplaySize,
			UI::GetPreviewColor(2), UI::GetPreviewColor(2),
			IM_COL32(0, 0, 0, 55), IM_COL32(0, 0, 0, 55)
		);

	// draw the watermark
	ImGui::GetForegroundDrawList()->AddText(ImVec2(4.f, 4.f), UI::GetPreviewColor(), UTIL_CXOR(BRUTALITY_WATERMARK));

	//
	// in this condition we should draw previews
	//
	if (isMenuOpen)
	{
		Features::Crosshair::DrawPreview(draw);
		Features::TextRadar::DrawPreview(draw);
		Features::SpectatorList::DrawPreview(draw);
	}
}
