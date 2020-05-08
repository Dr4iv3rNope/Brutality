#include "spectatorlist.hpp"
#include "playerlist.hpp"

#include "../util/strings.hpp"

#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/player.hpp"
#include "../sourcesdk/clientstate.hpp"
#include "../sourcesdk/engineclient.hpp"
#include "../sourcesdk/networkable.hpp"

#include "../config/variable.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include <list>
#include <deque>
#include <mutex>

using namespace SourceSDK;

extern Config::Bool spectatorListEnable;

extern Config::UInt32 spectatorListX;
extern Config::UInt32 spectatorListY;

extern Config::LUInt32 spectatorListMaxPlayers;


extern Config::Color playerColorNormal;
extern Config::Color playerColorDangerous;
extern Config::Color playerColorFriends;
extern Config::Color playerColorRages;


struct SpectatorInfo
{
	ObserverMode obs_mode;
	std::string name;
	ImU32 color;
};

static std::mutex spectatorListMutex;
static std::list<SpectatorInfo> spectatorList;

// when we calling it, we guarantee that mutex is locked
static bool PushPlayer(ObserverMode obs_mode, const PlayerInfo& engine_info, const Config::Color& color)
{
	// dont overflow player order
	if (*spectatorListMaxPlayers &&
		spectatorList.size() == *spectatorListMaxPlayers)
		return false;

	SpectatorInfo info;
	info.obs_mode = obs_mode;
	info.color = color.Hex();
	info.name = engine_info.GetName();

	spectatorList.push_back(info);
	return true;
}

void Features::SpectatorList::Update() noexcept
{
	if (!IsInGame() || !spectatorListEnable)
		return;

	auto localPlayer = BasePlayer::GetLocalPlayer();

	if (!localPlayer)
		return;

	spectatorListMutex.lock();
	spectatorList.clear();

	//
	// add players to order
	//
	for (auto i = 1; i <= globals->maxClients; i++)
	{
		auto* player = (BasePlayer*)BaseEntity::GetByIndex(i);

		if (!player)
			continue;

		if (player == localPlayer)
			continue;

		auto obs_mode = player->GetObserverMode();

		// if not in spectator mode, then skip him
		if (obs_mode == ObserverMode::None)
			continue;

		auto target = BaseEntity::GetByHandle(player->GetObserverTarget());

		if (!target || target != localPlayer)
			continue;

		static PlayerInfo playerInfo;

		if (!engine->GetPlayerInfo(i, playerInfo))
			continue;

		PlayerList::PlayerType type;
		if (PlayerList::GetPlayerType(i - 1, type))
		{
			switch (type)
			{
				case PlayerList::PlayerType::Normal:
					PushPlayer(obs_mode, playerInfo, playerColorNormal);
					break;

				case PlayerList::PlayerType::Dangerous:
					PushPlayer(obs_mode, playerInfo, playerColorDangerous);
					break;

				case PlayerList::PlayerType::Friend:
					PushPlayer(obs_mode, playerInfo, playerColorFriends);
					break;

				case PlayerList::PlayerType::Rage:
					PushPlayer(obs_mode, playerInfo, playerColorRages);
					break;
			}
		}
	}

	spectatorListMutex.unlock();
}

void Features::SpectatorList::DrawPreview(ImDrawList* list)
{
	if (!*spectatorListEnable)
		return;

	list->AddRectFilledMultiColor(
		ImVec2(float(*spectatorListX), float(*spectatorListY)),
		ImVec2(
			float(*spectatorListX) + 150.f,
			float(*spectatorListY) + (ImGui::GetCurrentContext()->Font->FontSize * *spectatorListMaxPlayers)
		),
		IM_COL32(220, 134, 40, 100), IM_COL32_BLACK_TRANS,
		IM_COL32_BLACK_TRANS, IM_COL32(220, 134, 40, 100)
	);

	list->AddText(
		ImVec2(float(*spectatorListX) + 4.f, float(*spectatorListY) + 4.f),
		IM_COL32_WHITE,
		UTIL_CXOR("Spectator List")
	);
}

static inline const std::string& ObserverModeToString(ObserverMode obs_mode) noexcept
{
	static std::deque strModes =
	{
		UTIL_SXOR("None"),
		UTIL_SXOR("Death Cam"),
		UTIL_SXOR("Freeze Cam"),
		UTIL_SXOR("Fixed"),
		UTIL_SXOR("First-Person"),
		UTIL_SXOR("Third-Person"),
		UTIL_SXOR("Roaming"),
		UTIL_SXOR("*Unknown*")
	};

	if (std::size_t(obs_mode) + 1 >= strModes.size() - 1)
		return strModes.back();

	return strModes[std::size_t(obs_mode)];
}

void Features::SpectatorList::Draw(ImDrawList* list)
{
	if (!IsInGame() || !spectatorListEnable)
		return;

	spectatorListMutex.lock();

	if (!spectatorList.empty())
	{
		int y_offset { 0 };

		for (const auto& spectator : spectatorList)
			list->AddText(
				ImVec2(
					(float)*spectatorListX,
					(float)*spectatorListY + (ImGui::GetCurrentContext()->Font->FontSize * y_offset++)
				),
				spectator.color,
				UTIL_FORMAT(
					'[' << ObserverModeToString(spectator.obs_mode) <<
					UTIL_XOR("] ") <<
					spectator.name
				).c_str()
			);
	}

	spectatorListMutex.unlock();
}
