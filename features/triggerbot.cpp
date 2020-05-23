#include "triggerbot.hpp"
#include "playerlist.hpp"

#include "../sourcesdk/usercmd.hpp"
#include "../sourcesdk/enginetraceclient.hpp"
#include "../sourcesdk/player.hpp"
#include "../sourcesdk/weapon.hpp"
#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/networkable.hpp"
#include "../sourcesdk/clientstate.hpp"

#include "../config/variable.hpp"

#include "../main.hpp"

#include <optional>

extern Config::Bool triggerbotEnable;
extern Config::Bool triggerbotCheckInvalidWep;
extern Config::Bool triggerbotTargetNormals;
extern Config::Bool triggerbotTargetDangerous;
extern Config::Bool triggerbotTargetFriends;
extern Config::Bool triggerbotTargetRages;
extern Config::Key triggerbotKey;
extern Config::LFloat triggerbotDelay;


static std::optional<SourceSDK::GameTrace> lastTrace;
static std::optional<float> lastDelay;

static inline bool IsTriggerBotKeyPressed() noexcept
{
	return !triggerbotKey.HasKeyValue() ||
		ImGui::Custom::GetAsyncKeyState(triggerbotKey.GetKeyValue());
}

static void InternalTraceLine(SourceSDK::BasePlayer* local_player) noexcept
{
	UTIL_DEBUG_ASSERT(local_player);

	if (*triggerbotDelay != 0.f && lastDelay)
		return;

	if (local_player->IsDead())
	{
		lastTrace = std::nullopt;
		return;
	}

	SourceSDK::Vector3 eye_pos;

	if (!local_player->GetEyePosition(eye_pos))
	{
		lastTrace = std::nullopt;
		return;
	}

	lastTrace = SourceSDK::GameTrace();

	interfaces->enginetrace->TraceRay(
		SourceSDK::Ray(eye_pos, eye_pos + interfaces->clientstate->viewAngles.Forward() * 50000.f),
		SourceSDK::Mask_Shot,
		SourceSDK::BasicTraceFilter(local_player),
		*lastTrace
	);
}

static inline bool Shoot(SourceSDK::UserCmd* cmd, float nextAttackTime) noexcept
{
	if (cmd->commandNumber % 2 &&
		interfaces->globals->curTime > nextAttackTime)
	{
		cmd->SetButton(SourceSDK::Input_Attack);
		return true;
	}
	else
	{
		cmd->RemoveButton(SourceSDK::Input_Attack);
		return false;
	}
}

void Features::TriggerBot::Think(SourceSDK::UserCmd* cmd) noexcept
{
	if (!triggerbotEnable)
		return;

	if (!IsTriggerBotKeyPressed())
	{
		lastDelay = std::nullopt;
		lastTrace = std::nullopt;
		return;
	}

	auto local_player = SourceSDK::BasePlayer::GetLocalPlayer();

	if (!local_player)
		return;

	if (local_player->IsDead())
		return;

	auto weapon = local_player->GetActiveWeapon();

	if (!weapon)
		return;

	if (lastDelay)
	{
		if (interfaces->globals->curTime > * lastDelay &&
			Shoot(cmd, weapon->GetNextPrimaryAttack()))
		{
			lastDelay = std::nullopt;
			lastTrace = std::nullopt;
		}

		return;
	}

	InternalTraceLine(local_player);

	if (!lastTrace)
		return;

	if (!lastTrace->DidHit())
		return;

	if (!lastTrace->entity)
		return;

	if (!lastTrace->entity->ToNetworkable()->GetClientClass()->IsPlayer())
		return;

	if (PlayerList::PlayerType type;
		PlayerList::GetPlayerType(
			lastTrace->entity->ToNetworkable()->GetEntityIndex() - 1,
			type
		))
	{
		switch (type)
		{
			case Features::PlayerList::PlayerType::Normal:
				if (!triggerbotTargetNormals)
					return;
				break;

			case Features::PlayerList::PlayerType::Dangerous:
				if (!triggerbotTargetDangerous)
					return;
				break;

			case Features::PlayerList::PlayerType::Friend:
				if (!triggerbotTargetFriends)
					return;
				break;

			case Features::PlayerList::PlayerType::Rage:
				if (!triggerbotTargetRages)
					return;
				break;
		}

		if (*triggerbotDelay != 0.f)
			lastDelay = interfaces->globals->curTime + *triggerbotDelay;
		else
			if (Shoot(cmd, weapon->GetNextPrimaryAttack()))
				lastTrace = std::nullopt;
	}
}
