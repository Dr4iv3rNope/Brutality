#include "globals.hpp"
#include "clientstate.hpp"
#include "player.hpp"

#include "../main.hpp"

#include "../util/debug/assert.hpp"

int SourceSDK::GlobalVars::TimeToTicks(float time) noexcept
{
	return int(0.5f + time / intervalPerTick);
}

float SourceSDK::GlobalVars::TicksToTime(int ticks) noexcept
{
	return intervalPerTick * ticks;
}

float SourceSDK::GlobalVars::GetServerTime() noexcept
{
	if (interfaces->clientstate->IsInGame())
	{
		if (auto local_player = SourceSDK::BasePlayer::GetLocalPlayer())
			return local_player->GetTickBase() * intervalPerTick;
		else
			return curTime;
	}
	else
	{
		// trying to get time when you're not in game, huh?
		UTIL_DEBUG_ASSERT(false);
		return 0.f;
	}
}
