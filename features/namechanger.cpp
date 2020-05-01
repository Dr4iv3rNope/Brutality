#include "namechanger.hpp"

#include "../shutdown.hpp"

#include "../imgui/imgui.h"
#include "../imgui/custom.hpp"

#include "../util/debug/labels.hpp"

#include "../sourcesdk/cvar.hpp"
#include "../sourcesdk/convar.hpp"
#include "../sourcesdk/const.hpp"
#include "../sourcesdk/netsetconvar.hpp"

static bool initialized { false };

static std::string oldName;
static std::string formatedName;

void Features::NameChanger::SetName(const std::string& new_name) noexcept
{
	formatedName = ImGuiCustom::FormatSysString(new_name);

	SourceSDK::SendConVarValue(UTIL_CXOR("name"), formatedName.c_str());
}

void Features::NameChanger::DrawMenu() noexcept
{
	if (!initialized)
		Initialize();

	if (ImGui::Begin(UTIL_CXOR("Name Changer"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string newName;

		if (ImGuiCustom::InputSysText("", newName) ||
			ImGui::Button(UTIL_CXOR("Send Name")))
			Features::NameChanger::SetName(newName);

		ImGui::SameLine();
		if (ImGui::Button(UTIL_CXOR("Reset name")))
			SetName(oldName);
	}
	ImGui::End();
}

static Main::ShutdownElement* shutdownElement;

void Features::NameChanger::Initialize()
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Initializing name changer"));
	UTIL_DEBUG_ASSERT(!initialized);

	auto name = SourceSDK::cvar->FindVar(UTIL_CXOR("name"));	
	UTIL_ASSERT(name, "failed to find 'name' convar");

	formatedName = oldName = std::string(name->GetRaw()->stringValue, name->GetRaw()->stringValueLength);

	initialized = true;
	UTIL_LABEL_OK();
}
