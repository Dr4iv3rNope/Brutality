#include "customdisconnect.hpp"

#include "../sourcesdk/clientstate.hpp"
#include "../sourcesdk/netchannel.hpp"
#include "../sourcesdk/globals.hpp"

#include "../imgui/imgui.h"
#include "../imgui/custom.hpp"

#include "../util/xorstr.hpp"

#include <deque>
#include <functional>
#include <utility>
#include <filesystem>

static const std::string& GetRandomFileName()
{
	static std::deque<std::string> fileNames;
	static bool isFilled { false };

	if (!isFilled)
		for (auto file : std::filesystem::directory_iterator(UTIL_SXOR("bin\\")))
			if (file.is_regular_file())
				fileNames.push_back(file.path().filename().string());

	return fileNames[rand() % fileNames.size()];
}

//
// reasons
// 

#define ADVANCED_REASON(name, func) \
	std::make_pair(UTIL_SXOR(name), [] () -> std::string { return (func); })

#define BASIC_REASON(name, reason) ADVANCED_REASON(name, UTIL_SXOR(reason))

static std::deque<std::pair<std::string, std::function<std::string()>>> reason_list =
{
	BASIC_REASON("Invalid Ticket", "Invalid STEAM UserID Ticket\n"),
	BASIC_REASON("Invalid Game Version", "Invalid STEAM UserID Ticket\n"),
	BASIC_REASON("No Steam logon", "No Steam logon\n"),
	BASIC_REASON("Vac Banned", "VAC banned from secure server\n"),
	BASIC_REASON("Steam Logged In Elsewhere", "This Steam account is being used in another location\n"),
	BASIC_REASON("No Game License", "This Steam account does not own this game. \nPlease login to the correct Steam account"),
	BASIC_REASON("Dropped by server", "Client dropped by server"),
	BASIC_REASON("Incompatible external tool", "You are running an external tool that is\nincompatible with Secure servers"),
	BASIC_REASON("Memory corruption", "Memory corruption detected"),
	BASIC_REASON("Incompatible Software", "You are running software that is\nnot compatible with Secure servers"),
	BASIC_REASON("Timeout", "Client timed out"),
	ADVANCED_REASON(
		"Client overflowed channel",
		UTIL_FORMAT(
			UTIL_XOR("Client ") <<
			(rand() % SourceSDK::globals->maxClients) <<
			UTIL_XOR(" overflowed reliable channel")
		)
	),
	ADVANCED_REASON(
		"Couldn't CRC map",
		UTIL_FORMAT(
			UTIL_XOR("Couldn't CRC map ") <<
			std::string(SourceSDK::clientState->currentMap, sizeof(SourceSDK::clientState->currentMap)) <<
			UTIL_XOR(", disconnecting\n")
		)
	),
	ADVANCED_REASON(
		"Missing map",
		UTIL_FORMAT(
			UTIL_XOR("Missing map ") <<
			std::string(SourceSDK::clientState->currentMap, sizeof(SourceSDK::clientState->currentMap)) <<
			UTIL_XOR(", disconnecting\n")
		)
	),
	ADVANCED_REASON(
		"Map is differs from the server's",
		UTIL_FORMAT(
			UTIL_XOR("Your map [") <<
			std::string(SourceSDK::clientState->currentMap, sizeof(SourceSDK::clientState->currentMap)) <<
			UTIL_XOR("] differs from the server's.\n")
		)
	),
	ADVANCED_REASON(
		"Dll is differs from the server's",
		UTIL_FORMAT(UTIL_XOR("Your .dll [") << GetRandomFileName() << UTIL_XOR("] differs from the server's.\n"))
	),
	BASIC_REASON("[XBOX] BSP CRC failed", "Disconnect: BSP CRC failed!\n"),
	ADVANCED_REASON(
		"Bad CRC for file",
		UTIL_FORMAT(UTIL_XOR("Bad CRC for ") << GetRandomFileName() << '\n')
	),
	BASIC_REASON("Couldn't CRC client dll", "Couldn't CRC client side dll client.dll.\n"),
	BASIC_REASON("Lost connection to server", "Lost connection to server."),
	BASIC_REASON("Cannot continue without model", "Cannot continue without model models/player.mdl, disconnecting\n"),
	ADVANCED_REASON(
		"File consistency error",
		UTIL_FORMAT(UTIL_XOR("Server is enforcing file consistency for ") << GetRandomFileName() << '\n')
	),
	BASIC_REASON("Different class tables", "Server uses different class tables"),
	BASIC_REASON("Buffer overflow", "Server overflowed reliable buffer\n"),
	BASIC_REASON("Bad relay password", "Bad relay password"),
	BASIC_REASON("Bad spectator password", "Bad spectator password"),
	BASIC_REASON("SourceTV only local spectators", "SourceTV server is restricted to local spectators (class C).\n"),
	BASIC_REASON("Reliable snapshot overflow", "ERROR! Reliable snapshot overflow."),
	BASIC_REASON("Couldn't send snapshot", "ERROR! Couldn't send snapshot.")
};

void Features::CustomDisconnect::DrawMenu() noexcept
{
	if (ImGui::Begin(UTIL_CXOR("Custom Disconnect"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string reason_buf, formated_buf;

		if (ImGuiCustom::InputSysText("", reason_buf, &formated_buf) ||
			ImGui::Button(UTIL_CXOR("Disconnect")))
			Disconnect(formated_buf.c_str());

		ImGui::NewLine();

		if (int selected_item = -1; ImGui::ListBox("", &selected_item, [] (void*, int idx, const char** out) -> bool
		{
			*out = reason_list[idx].first.c_str();
			return true;
		}, nullptr, reason_list.size()))
		{
			auto reason = reason_list[selected_item].second();

			Disconnect(reason.c_str());
		}
	}
	ImGui::End();
}

bool Features::CustomDisconnect::Disconnect(const char* reason) noexcept
{
	if (SourceSDK::IsConnected())
	{
		SourceSDK::clientState->netChannel->Shutdown(reason);
		return true;
	}
	else
		return false;
}
