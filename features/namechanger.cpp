#include "namechanger.hpp"

#include "../shutdown.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../imgui/custom/specialchars.hpp"
#include "../imgui/custom/windowmanager.hpp"
#include "../imgui/custom/variableui.hpp"

#include "../util/debug/labels.hpp"

#include "../sourcesdk/cvar.hpp"
#include "../sourcesdk/convar.hpp"
#include "../sourcesdk/const.hpp"
#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/clientstate.hpp"
#include "../sourcesdk/netsetconvar.hpp"

#include "../config/variable.hpp"

static bool nameSpam { false };
static std::string oldName, formatedName;

static struct
{
	bool nameSwitch { false };
	float lastUpdate { 0.f };
} spam;

extern Config::String<char> nameChangerName;
extern Config::Bool nameChangerAutoSend;

static inline void SendName() noexcept
{
	SourceSDK::SendConVarValue(UTIL_CXOR("name"), formatedName.c_str());
}

void Features::NameChanger::SetName(const std::string& new_name) noexcept
{
	nameChangerName.SetValue(new_name);

	formatedName = ImGui::Custom::FormatSpecialChars(new_name);

	SendName();
}

static void DrawMenu(ImGui::Custom::Window&) noexcept
{
	ImGui::Custom::Variable::String(nameChangerName);

	if (ImGui::IsItemHovered())
		ImGui::Custom::ShowSpecialChars();


	if (ImGui::Button(UTIL_CXOR("Send Name")))
		Features::NameChanger::SetName(*nameChangerName);

	ImGui::SameLine();
	if (ImGui::Button(UTIL_CXOR("Reset name")))
		Features::NameChanger::SetName(oldName);
		
	ImGui::Custom::Variable::Boolean(nameChangerAutoSend);
	ImGui::Checkbox(UTIL_CXOR("Name Spam"), &nameSpam);
}

void Features::NameChanger::Update() noexcept
{
	//
	// name spam
	//

	if (nameSpam)
	{
		static auto lastUpdate = 0.f;

		if (!SourceSDK::IsInGame())
			lastUpdate = 0.f;
		else
		{
			if (lastUpdate < SourceSDK::globals->curTime)
			{
				lastUpdate = SourceSDK::globals->curTime + 15.f;

				if (spam.nameSwitch)
				{
					SetName(*nameChangerName + '\n');
				}
				else
					SetName(*nameChangerName);

				spam.nameSwitch = !spam.nameSwitch;
			}
		}
	}

	//
	// auto send
	//

	{
		static bool isSent { false };

		if (!*nameChangerAutoSend)
			isSent = false;
		else
		{
			if (!isSent && SourceSDK::IsInGame())
			{
				SendName();
				isSent = true;
			}
			else if (isSent && !SourceSDK::IsInGame())
				isSent = false;
		}
	}
}

void Features::NameChanger::Initialize() noexcept
{
	UTIL_XLOG(L"Initializing name changer");

	auto name = SourceSDK::cvar->FindVar(UTIL_CXOR("name"));	
	UTIL_ASSERT(name, "failed to find 'name' convar");

	formatedName = oldName = std::string(name->GetRaw()->stringValue, name->GetRaw()->stringValueLength);
}

void Features::NameChanger::RegisterWindow() noexcept
{
	ImGui::Custom::windowManager->RegisterWindow(
		ImGui::Custom::Window(
			UTIL_SXOR("Name Changer"),
			ImGuiWindowFlags_AlwaysAutoResize,
			DrawMenu
		)
	);
}
