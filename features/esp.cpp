#include "esp.hpp"
#include "playerlist.hpp"
#include "entlist.hpp"

#include "../util/strings.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/custom/colors.hpp"
#include "../imgui/custom/windowmanager.hpp"

#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/weapon.hpp"
#include "../sourcesdk/player.hpp"
#include "../sourcesdk/entitylist.hpp"
#include "../sourcesdk/clientstate.hpp"
#include "../sourcesdk/engineclient.hpp"
#include "../sourcesdk/networkable.hpp"
#include "../sourcesdk/renderable.hpp"
#include "../sourcesdk/worldtoscreen.hpp"
#include "../sourcesdk/localize.hpp"

#include "../gmod/sourcesdk/gmodplayer.hpp"

#include "../config/variable.hpp"

#include <utility>
#include <mutex>
#include <list>

using namespace SourceSDK;

extern Config::Bool espEnabled;
extern Config::LFloat espMaxDistance;

extern Config::Bool espUpdatePerFrame;

extern Config::Bool espDrawEntities;
extern Config::Bool espDrawNormal;
extern Config::Bool espDrawDangerous;
extern Config::Bool espDrawFriends;
extern Config::Bool espDrawRages;
extern Config::Bool espDrawDeadPlayers;
extern Config::Bool espDrawDormantPlayers;

extern Config::Color playerColorNormal;
extern Config::Color playerColorDangerous;
extern Config::Color playerColorFriends;
extern Config::Color playerColorRages;

extern Config::Bool espHealth;
extern Config::Color espHealthColor;

extern Config::Bool espName;
extern Config::Color espNameColor;

extern Config::Bool espSkeleton;
#ifdef _DEBUG
extern Config::Bool dbg_espSkeleton;
#endif
extern Config::LFloat espSkeletonThickness;

extern Config::Bool espActiveWeapon;
extern Config::Color espActiveWeaponColor;


inline bool ShouldDrawPlayers() noexcept
{
	return espDrawNormal ||
		espDrawDangerous ||
		espDrawFriends ||
		espDrawRages ||
		espDrawDeadPlayers;
}

inline bool IsEspEnabled() noexcept
{
	return espEnabled && (espDrawEntities || ShouldDrawPlayers());
}


struct PlayerDrawInfo
{
	bool isDead { false };

	const Config::Color* color { nullptr }; // cannot be null

	std::string status;
	std::string healthText;
	std::optional<std::string> activeWeaponText;
	std::string nameText;
	std::list<std::pair<Vector3, Vector3>> skeletonPath;
};

struct EntityDrawInfo
{
	ImU32 color {};
	std::string classname;
};

struct DrawInfo
{
private:
	union
	{
		PlayerDrawInfo* _player;
		EntityDrawInfo* _entity;
	};

	bool _isPlayer;

public:
	bool isDormant;

	#if _DEBUG
	BaseEntity* dbg_entity { nullptr };
	#endif

	Vector3 center {};

	inline DrawInfo(bool isPlayer) noexcept
		: _isPlayer { isPlayer }
	{
		if (isPlayer)
			_player = new PlayerDrawInfo {};
		else
			_entity = new EntityDrawInfo {};
	}

	inline DrawInfo(const DrawInfo& info) noexcept
	{
		_isPlayer = info._isPlayer;
		isDormant = info.isDormant;
		center = info.center;
		#if _DEBUG
		dbg_entity = info.dbg_entity;
		#endif

		if (_isPlayer)
			_player = new PlayerDrawInfo { *info._player };
		else
			_entity = new EntityDrawInfo { *info._entity };
	}

	inline ~DrawInfo()
	{
		if (_isPlayer)
			delete _player;
		else
			delete _entity;
	}

	inline bool IsPlayer() const noexcept { return _isPlayer; }

	inline PlayerDrawInfo* GetPlayerInfo() noexcept { return _isPlayer ? _player : nullptr; }
	inline EntityDrawInfo* GetEntityInfo() noexcept { return !_isPlayer ? _entity : nullptr; }

	inline const PlayerDrawInfo* GetPlayerInfo() const noexcept { return _isPlayer ? _player : nullptr; }
	inline const EntityDrawInfo* GetEntityInfo() const noexcept { return !_isPlayer ? _entity : nullptr; }
};
using DrawInfoList = std::list<DrawInfo>;

static std::mutex drawInfoMutex;
static DrawInfoList drawInfo;


static void EspGetCenter(BasePlayer* player, Vector3& center)
{
	UTIL_DEBUG_ASSERT(player);

	if (!espDrawDeadPlayers || player->IsAlive())
	{
		if (int pelvis_idx; player->ToRenderable()->FindBoneIndex(UTIL_CXOR("pelvis"), pelvis_idx))
		{
			if (player->ToRenderable()->GetBonePosition(pelvis_idx, center))
				return;
			else
				UTIL_DEBUG_XLOG(L"Failed to get bone position");
		}
		else
			UTIL_DEBUG_XLOG(L"Failed to find player's pelvis");
	}

	//
	// if failed to get pelvis or player is dead
	// then our center will be player's origin
	//
	center = player->GetOrigin();
}

static void PushPlayer(
	int idx, BasePlayer* player,
	const Config::Color& color, const std::string& status)
{
	UTIL_DEBUG_ASSERT(player);

	DrawInfo info(true);
	#if _DEBUG
	info.dbg_entity = player;
	#endif
	info.isDormant = player->ToNetworkable()->IsDormant();
	info.GetPlayerInfo()->status = status;
	info.GetPlayerInfo()->color = &color;
	info.GetPlayerInfo()->isDead = player->IsDead();

	EspGetCenter(player, info.center);

	if (espHealth)
		info.GetPlayerInfo()->healthText = UTIL_FORMAT(UTIL_XOR("Health: ") << player->GetHealth() << '%');

	if (espName)
		if (static PlayerInfo engineInfo; interfaces->engine->GetPlayerInfo(idx, engineInfo))
			info.GetPlayerInfo()->nameText = std::string(engineInfo.name, MAX_PLAYER_NAME_LENGTH);

	if (espActiveWeapon)
		if (auto weapon = player->GetActiveWeapon())
		{
			if (auto text = interfaces->localize->Find(weapon->GetWeaponInfo().printname); text)
				info.GetPlayerInfo()->activeWeaponText = Util::ToMultiByte(text);
			else
				info.GetPlayerInfo()->activeWeaponText = weapon->GetClassname();
		}

	if (espSkeleton)
		player->ToRenderable()->EnumerateBones([&player, &info] (int idx, const Studio::Bone* bone) -> bool
		{
			if (bone->parentIndex != -1 && bone->flags & BoneMask_Hitbox)
				if (Vector3 parent; player->ToRenderable()->GetBonePosition(bone->parentIndex, parent))
					if (Vector3 child; player->ToRenderable()->GetBonePosition(idx, child))
						info.GetPlayerInfo()->skeletonPath.push_back(std::make_pair(parent, child));

			return false;
		});

	drawInfo.push_back(info);
}

static void PushEntity(BaseEntity* entity)
{
	UTIL_DEBUG_ASSERT(entity);

	if (auto settings = Features::EntityList::GetEntitySettings(entity->GetClassname());
		settings && settings->espDraw)
	{
		DrawInfo info(false);
		#if _DEBUG
		info.dbg_entity = entity;
		#endif
		info.isDormant = entity->ToNetworkable()->IsDormant();
		info.center = entity->GetOrigin();
		info.GetEntityInfo()->color = settings->espColor;
		info.GetEntityInfo()->classname = settings->classname;

		drawInfo.push_back(info);
	}
}

static void UpdateESP() noexcept
{
	using namespace Features;

	drawInfo.clear();

	for (auto i = 1; i < interfaces->entitylist->GetMaxEntities(); i++)
	{
		auto* entity = BaseEntity::GetByIndex(i);

		if (!entity)
			continue;

		if (*espMaxDistance &&
			entity->GetOrigin().DistanceTo(BasePlayer::GetLocalPlayer()->GetOrigin()) > *espMaxDistance)
			continue;

		// players
		if (i <= interfaces->globals->maxClients)
		{
			// if we dont want to render players
			// then skip enumerating players
			if (!ShouldDrawPlayers())
			{
				i = interfaces->globals->maxClients;
				continue;
			}

			auto player = (BasePlayer*)entity;

			if (player == BasePlayer::GetLocalPlayer())
				continue;

			if (!espDrawDeadPlayers && player->IsDead())
				continue;

			if (!espDrawDormantPlayers && player->ToNetworkable()->IsDormant())
				continue;

			PlayerList::PlayerType type;
			if (PlayerList::GetPlayerType(i - 1, type))
			{
				switch (type)
				{
					case PlayerList::PlayerType::Normal:
						if (espDrawNormal)
							PushPlayer(i, player, playerColorNormal, UTIL_SXOR("Normal"));
						break;

					case PlayerList::PlayerType::Dangerous:
						if (espDrawDangerous)
							PushPlayer(i, player, playerColorDangerous, UTIL_SXOR("Dangerous"));
						break;

					case PlayerList::PlayerType::Friend:
						if (espDrawFriends)
							PushPlayer(i, player, playerColorFriends, UTIL_SXOR("Friend"));
						break;

					case PlayerList::PlayerType::Rage:
						if (espDrawRages)
							PushPlayer(i, player, playerColorRages, UTIL_SXOR("Rage"));
						break;
				}
			}
		}
		else
		{
			if (!espDrawEntities)
				break;

			PushEntity(entity);
		}
	}
}

void Features::Esp::Update() noexcept
{
	if (espUpdatePerFrame || !interfaces->clientstate->IsInGame() || !IsEspEnabled())
		return;

	auto localPlayer = BasePlayer::GetLocalPlayer();

	if (!localPlayer)
		return;

	#ifdef _DEBUG
	static int dbg_lastTickUpdated { interfaces->globals->tickCount };

	if (dbg_lastTickUpdated + 1 < interfaces->globals->tickCount)
	{
		UTIL_DEBUG_LOG(UTIL_WFORMAT(
			UTIL_XOR(L"Esp Update tick desync! ") <<
			dbg_lastTickUpdated + 1 << ' ' << interfaces->globals->tickCount
		));

		dbg_lastTickUpdated = interfaces->globals->tickCount;
	}
	#endif

	drawInfoMutex.lock();
	UpdateESP();
	drawInfoMutex.unlock();
}


constexpr float INFO_OFFSET { 24.f };

static inline void DrawTextCentered(const ImVec2& pos, ImU32 color, const std::string& text, ImDrawList* list) noexcept
{
	UTIL_DEBUG_ASSERT(list);

	auto text_size = ImGui::CalcTextSize(text.c_str());

	list->AddText(ImVec2(pos.x - (text_size.x / 2), pos.y - text_size.y), color, text.c_str());
};

static void EspRenderPlayer(const DrawInfo& info, ImDrawList* list)
{
	UTIL_DEBUG_ASSERT(list);
	UTIL_DEBUG_ASSERT(info.IsPlayer());

	if (auto screen = info.center.ToScreen())
	{
		list->AddRectFilled(
			// left top corner
			ImVec2(screen->X() - 3.f, screen->Y() - 3.f),

			// right bottom corner
			ImVec2(screen->X() + 3.f, screen->Y() + 3.f),

			info.GetPlayerInfo()->color->Hex()
		);

		list->AddRect(
			// left top corner
			ImVec2(screen->X() - 6.f, screen->Y() - 6.f),

			// right bottom corner
			ImVec2(screen->X() + 6.f, screen->Y() + 6.f),

			info.GetPlayerInfo()->color->Hex()
		);

		DrawTextCentered(ImVec2(screen->X(), screen->Y() - 8.f), info.GetPlayerInfo()->color->Hex(), info.GetPlayerInfo()->status, list);

		// draw info

		int y_offset = 0;

		if (espDrawDormantPlayers && info.isDormant)
			DrawTextCentered(
				ImVec2(screen->X(), screen->Y() + INFO_OFFSET + (ImGui::GetCurrentContext()->Font->FontSize * y_offset++)),
				0xCCCCCCCC,
				UTIL_XOR("*Dormant*"),
				list
			);

		if (espDrawDeadPlayers && info.GetPlayerInfo()->isDead)
			DrawTextCentered(
				ImVec2(screen->X(), screen->Y() + INFO_OFFSET + (ImGui::GetCurrentContext()->Font->FontSize * y_offset++)),
				0xFF1111CC,
				UTIL_XOR("*Dead*"),
				list
			);

		if (espHealth)
			DrawTextCentered(
				ImVec2(screen->X(), screen->Y() + INFO_OFFSET + (ImGui::GetCurrentContext()->Font->FontSize * y_offset++)),
				espHealthColor.Hex(),
				info.GetPlayerInfo()->healthText.c_str(),
				list
			);

		if (espName)
			DrawTextCentered(
				ImVec2(screen->X(), screen->Y() + INFO_OFFSET + (ImGui::GetCurrentContext()->Font->FontSize * y_offset++)),
				espNameColor.Hex(),
				info.GetPlayerInfo()->nameText.c_str(),
				list
			);

		if (espActiveWeapon &&
			info.GetPlayerInfo()->activeWeaponText.has_value())
			DrawTextCentered(
				ImVec2(screen->X(), screen->Y() + INFO_OFFSET + (ImGui::GetCurrentContext()->Font->FontSize * y_offset++)),
				espActiveWeaponColor.Hex(),
				info.GetPlayerInfo()->activeWeaponText->c_str(),
				list
			);

		//
		// debug skeleton
		//
		#if _DEBUG
		if (dbg_espSkeleton && info.dbg_entity)
		{
			auto player = (BasePlayer*)info.dbg_entity;

			player->ToRenderable()->EnumerateBones([&player, &list] (int idx, const Studio::Bone* bone) -> bool
			{
				if (Vector3 pos; player->ToRenderable()->GetBonePosition(idx, pos))
					if (auto screen = pos.ToScreen())
					{
						const auto bone_name = bone->GetName();
						const auto str_idx = UTIL_FORMAT(idx << ' ' << (bone_name ? bone_name : "?"));

						list->AddText(ImVec2(screen->X(), screen->Y()), IM_COL32_WHITE, str_idx.c_str());
					}

				return false;
			});
		}
		#endif

		if (espSkeleton && !info.GetPlayerInfo()->skeletonPath.empty())
			for (auto& pair : info.GetPlayerInfo()->skeletonPath)
				if (auto parent = pair.first.ToScreen())
					if (auto child = pair.second.ToScreen())
						list->AddLine(
							ImVec2(parent->X(), parent->Y()),
							ImVec2(child->X(), child->Y()),
							info.GetPlayerInfo()->color->Hex(),
							*espSkeletonThickness
						);
	}
}

static void EspRenderEntity(const DrawInfo& info, ImDrawList* list)
{
	UTIL_DEBUG_ASSERT(list);
	UTIL_DEBUG_ASSERT(!info.IsPlayer());

	if (auto screen = info.center.ToScreen())
	{
		list->AddRect(
			ImVec2(screen->X() - 4.f, screen->Y() - 4.f),
			ImVec2(screen->X() + 4.f, screen->Y() + 4.f),
			info.GetEntityInfo()->color
		);

		DrawTextCentered(ImVec2(screen->X(), screen->Y() - 8.f), info.GetEntityInfo()->color, info.GetEntityInfo()->classname, list);

		int y_offset = 0;

		if (info.isDormant)
			DrawTextCentered(
				ImVec2(screen->X(), screen->Y() + INFO_OFFSET + (ImGui::GetCurrentContext()->Font->FontSize * y_offset++)),
				0xCCCCCCCC,
				UTIL_XOR("*Dormant*"),
				list
			);
	}
}

void Features::Esp::Draw(ImDrawList* list)
{
	UTIL_DEBUG_ASSERT(list);

	if (!interfaces->clientstate->IsInGame() || !IsEspEnabled())
		return;

	drawInfoMutex.lock();

	if (espUpdatePerFrame)
		UpdateESP();

	for (auto& info : drawInfo)
	{
		if (info.IsPlayer())
			EspRenderPlayer(info, list);
		else
			EspRenderEntity(info, list);
	}

	drawInfoMutex.unlock();
}

