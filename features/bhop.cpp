#include "bhop.hpp"

#include "../main.hpp"

#include "../sourcesdk/usercmd.hpp"
#include "../sourcesdk/inputsystem.hpp"

#include "../config/variable.hpp"

#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/player.hpp"

#include <optional>

extern Config::Bool bhopEnable;
extern Config::Bool bhopAutoStrafe;
extern Config::LUInt32 bhopSkipTicks;

static inline void AutoStrafe(SourceSDK::UserCmd* cmd) noexcept
{
	if (cmd->mouseX > 0)
		cmd->move.Y() = SourceSDK::GetMaxSideSpeed();
	else if (cmd->mouseX < 0)
		cmd->move.Y() = -SourceSDK::GetMaxSideSpeed();
}

void Features::BunnyHop::Think(SourceSDK::UserCmd* cmd) noexcept
{
	static std::uint32_t skippedTicks = 0;

	if (!bhopEnable)
		return;

	if (!interfaces->inputsystem->IsButtonDown(SourceSDK::ButtonCode::KeySpace))
	{
		skippedTicks = 0;
		return;
	}

	auto localPlayer = SourceSDK::BasePlayer::GetLocalPlayer();

	if (!localPlayer)
		return;

	if (localPlayer->IsDead())
		return;

	if (localPlayer->IsFly())
		return;

	if (*bhopSkipTicks == 0 || skippedTicks >= *bhopSkipTicks)
	{
		if (localPlayer->IsOnGround())
		{
			cmd->SetButton(SourceSDK::Input_Jump);
			skippedTicks = 0;
		}
		else
		{
			cmd->RemoveButton(SourceSDK::Input_Jump);

			if (*bhopAutoStrafe)
				AutoStrafe(cmd);
		}
	}
	else
		skippedTicks++;
}
