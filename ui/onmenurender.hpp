//
// DO NOT INCLUDE THIS FILE
//

#pragma once
#include "../imgui/imgui.h"
#include "../imgui/custom/windowmanager.hpp"

#include "../config/config.hpp"

void OnMenuRender()
{
	ImGui::Custom::windowManager.Render();
	ImGui::Custom::windowManager.RenderWindows();
}
