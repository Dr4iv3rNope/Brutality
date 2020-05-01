#include "crosshair.hpp"

#include "../sourcesdk/clientstate.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../config/variable.hpp"

using namespace SourceSDK;


extern Config::Bool crosshairEnable;

extern Config::LUInt32 crosshairGap;
extern Config::LUInt32 crosshairSize;
extern Config::LUInt32 crosshairThickness;

extern Config::Color crosshairColor;


static inline void Draw(const ImVec2& center, ImDrawList* list) noexcept
{
	// top
	list->AddRectFilled(
		ImVec2(
			center.x - float(*crosshairThickness * 2) / 2,
			center.y - *crosshairGap
		),
		ImVec2(
			center.x + float(*crosshairThickness * 2) / 2,
			center.y - *crosshairGap - *crosshairSize
		),
		crosshairColor.Hex()
	);

	// bottom
	list->AddRectFilled(
		ImVec2(
			center.x - float(*crosshairThickness * 2) / 2,
			center.y + *crosshairGap
		),
		ImVec2(
			center.x + float(*crosshairThickness * 2) / 2,
			center.y + *crosshairGap + *crosshairSize
		),
		crosshairColor.Hex()
	);

	// right
	list->AddRectFilled(
		ImVec2(
			center.x + *crosshairGap,
			center.y + float(*crosshairThickness * 2) / 2
		),
		ImVec2(
			center.x + *crosshairGap + *crosshairSize,
			center.y - float(*crosshairThickness * 2) / 2
		),
		crosshairColor.Hex()
	);

	// left
	list->AddRectFilled(
		ImVec2(
			center.x - *crosshairGap,
			center.y + float(*crosshairThickness * 2) / 2
		),
		ImVec2(
			center.x - *crosshairGap - *crosshairSize,
			center.y - float(*crosshairThickness * 2) / 2
		),
		crosshairColor.Hex()
	);
}

inline ImVec2 GetDisplayCenter() noexcept
{
	return ImVec2(
		ImGui::GetIO().DisplaySize.x / 2,
		ImGui::GetIO().DisplaySize.y / 2
	);
}

void Features::Crosshair::DrawPreview(ImDrawList* list) noexcept
{
	if (!*crosshairEnable)
		return;

	const auto center = GetDisplayCenter();
	const auto text_size = ImGui::CalcTextSize(UTIL_CXOR("Crosshair"));

	list->AddRectFilled(
		ImVec2(
			center.x - *crosshairGap - *crosshairSize - 8.f,
			center.y - *crosshairGap - *crosshairSize - 12.f
			- ImGui::GetCurrentContext()->Font->FontSize
		),
		ImVec2(
			center.x + *crosshairGap + *crosshairSize + text_size.x,
			center.y + *crosshairGap + *crosshairSize + 8.f
		),
		IM_COL32(220, 134, 40, 100)
	);

	list->AddText(
		ImVec2(
			center.x - *crosshairGap - *crosshairSize - 6.f,
			center.y - *crosshairGap - *crosshairSize - 10.f
			- ImGui::GetCurrentContext()->Font->FontSize
		),
		IM_COL32_WHITE,
		UTIL_CXOR("Crosshair")
	);

	::Draw(center, list);
}

void Features::Crosshair::Draw(ImDrawList* list, bool isMenuOpen) noexcept
{
	if (!IsInGame() || !*crosshairEnable || isMenuOpen)
		return;

	::Draw(GetDisplayCenter(), list);
}
