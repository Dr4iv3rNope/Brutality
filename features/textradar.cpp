#include "textradar.hpp"
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
#include <mutex>

using namespace SourceSDK;

extern Config::Bool textRadarEnable;

extern Config::UInt32 textRadarX;
extern Config::UInt32 textRadarY;

extern Config::LUInt32 textRadarMaxPlayers;
extern Config::LFloat textRadarMaxDistance;

extern Config::Bool textRadarDrawDistance;

extern Config::Color playerColorNormal;
extern Config::Color playerColorDangerous;
extern Config::Color playerColorFriends;
extern Config::Color playerColorRages;


struct TextRadarInfo
{
	float distance;
	std::string name;
	ImU32 color;
};

static std::mutex playerOrderMutex;
// top values are max distance
static std::list<TextRadarInfo> playerOrder;

// when we calling it, we guarantee that mutex is locked
static bool PushPlayer(float distance, const PlayerInfo& engine_info, const Config::Color& color)
{
	// dont overflow player order
	if (*textRadarMaxPlayers &&
		playerOrder.size() == *textRadarMaxPlayers)
		return false;

	TextRadarInfo info;
	info.distance = distance;
	info.color = color.Hex();
	info.name = std::string(engine_info.name, MAX_PLAYER_NAME_LENGTH);

	// sorting values
	{
		if (!playerOrder.empty())
			for (auto i = playerOrder.begin(); i != playerOrder.end(); i++)
				if (i->distance <= distance)
				{
					playerOrder.insert(i, info);
					return true;
				}
		
		playerOrder.push_back(info);
	}

	return true;
}

void Features::TextRadar::Update() noexcept
{
	if (!IsInGame() || !textRadarEnable)
		return;

	auto localPlayer = BasePlayer::GetLocalPlayer();

	if (!localPlayer)
		return;

	playerOrderMutex.lock();
	playerOrder.clear();

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

		if (player->IsDead())
			continue;

		static PlayerInfo playerInfo;

		if (!engine->GetPlayerInfo(i, playerInfo))
			continue;

		const auto distance = player->GetOrigin().DistanceTo(localPlayer->GetOrigin());

		if ((float)textRadarMaxDistance &&
			distance > (float)textRadarMaxDistance)
			continue;

		PlayerList::PlayerType type;
		if (PlayerList::GetPlayerType(i - 1, type))
		{
			switch (type)
			{
				case PlayerList::PlayerType::Normal:
					PushPlayer(distance, playerInfo, playerColorNormal);
					break;

				case PlayerList::PlayerType::Dangerous:
					PushPlayer(distance, playerInfo, playerColorDangerous);
					break;

				case PlayerList::PlayerType::Friend:
					PushPlayer(distance, playerInfo, playerColorFriends);
					break;

				case PlayerList::PlayerType::Rage:
					PushPlayer(distance, playerInfo, playerColorRages);
					break;
			}
		}
	}

	playerOrderMutex.unlock();
}

void Features::TextRadar::DrawPreview(ImDrawList* list)
{
	if (!*textRadarEnable)
		return;

	list->AddRectFilledMultiColor(
		ImVec2(float(*textRadarX), float(*textRadarY)),
		ImVec2(
			float(*textRadarX) + 150.f,
			float(*textRadarY) + (ImGui::GetCurrentContext()->Font->FontSize * *textRadarMaxPlayers)
		),
		IM_COL32(220, 134, 40, 100), IM_COL32_BLACK_TRANS,
		IM_COL32_BLACK_TRANS, IM_COL32(220, 134, 40, 100)
	);

	list->AddText(
		ImVec2(float(*textRadarX) + 4.f, float(*textRadarY) + 4.f),
		IM_COL32_WHITE,
		UTIL_CXOR("Text Radar")
	);
}

void Features::TextRadar::Draw(ImDrawList* list)
{	
	if (!IsInGame() || !textRadarEnable)
		return;

	playerOrderMutex.lock();

	if (!playerOrder.empty())
	{
		int y_offset { 0 };

		for (auto i = playerOrder.rbegin(); i != playerOrder.rend(); i++)
			list->AddText(
				ImVec2(
					(float)*textRadarX,
					(float)*textRadarY + (ImGui::GetCurrentContext()->Font->FontSize * y_offset++)
				),
				i->color,
				textRadarDrawDistance
				? UTIL_FORMAT('[' << i->distance << UTIL_XOR("] ") << i->name).c_str()
				: i->name.c_str()
			);
	}

	playerOrderMutex.unlock();
}
