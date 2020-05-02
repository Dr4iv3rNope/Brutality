#include "namechanger.hpp"

#include "../shutdown.hpp"

#include "../imgui/imgui.h"
#include "../imgui/custom.hpp"

#include "../util/debug/labels.hpp"

#include "../sourcesdk/cvar.hpp"
#include "../sourcesdk/convar.hpp"
#include "../sourcesdk/const.hpp"
#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/clientstate.hpp"
#include "../sourcesdk/netsetconvar.hpp"

#include "../config/variable.hpp"

static bool initialized { false };

static bool nameSpam { false };
static std::string oldName, formatedName;

static struct
{
	std::string name;
	bool nameSwitch { false };
	float lastUpdate { 0.f };
} spam;

extern Config::String<char> nameChangerName;
extern Config::Bool nameChangerAutoSend;

void Features::NameChanger::SetName(const std::string& new_name) noexcept
{
	nameChangerName.SetValue(new_name);

	spam.name = new_name;
	spam.nameSwitch = false;

	formatedName = ImGuiCustom::FormatSysString(new_name);

	SourceSDK::SendConVarValue(UTIL_CXOR("name"), formatedName.c_str());
}

void Features::NameChanger::DrawMenu() noexcept
{
	if (!initialized)
		Initialize();

	if (ImGui::Begin(UTIL_CXOR("Name Changer"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (std::string temp = nameChangerName.GetValue();
			ImGuiCustom::InputSysText("", temp))
			nameChangerName.SetValue(temp);

		if (ImGui::Button(UTIL_CXOR("Send Name")))
			Features::NameChanger::SetName(*nameChangerName);

		ImGui::SameLine();
		if (ImGui::Button(UTIL_CXOR("Reset name")))
			SetName(oldName);
		
		if (bool autoSend = *nameChangerAutoSend;
			ImGui::Checkbox(UTIL_CXOR("Auto Send"), &autoSend))
			nameChangerAutoSend.SetValue(autoSend);

		ImGui::Checkbox(UTIL_CXOR("Name Spam"), &nameSpam);
	}
	ImGui::End();
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
					spam.name.push_back('\n');
					SetName(spam.name);
				}
				else
				{
					spam.name.pop_back();
					SetName(spam.name);
				}

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
			if (!isSent && SourceSDK::IsConnected())
			{
				SetName(formatedName);
				isSent = true;
			}
			else if (isSent && !SourceSDK::IsConnected())
				isSent = false;
		}
	}
}

static Main::ShutdownElement* shutdownElement;

void Features::NameChanger::Initialize()
{
	UTIL_XLOG(L"Initializing name changer");
	UTIL_DEBUG_ASSERT(!initialized);

	auto name = SourceSDK::cvar->FindVar(UTIL_CXOR("name"));	
	UTIL_ASSERT(name, "failed to find 'name' convar");

	formatedName = oldName = std::string(name->GetRaw()->stringValue, name->GetRaw()->stringValueLength);

	initialized = true;
}
