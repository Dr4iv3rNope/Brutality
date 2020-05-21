#include "fastwalk.hpp"

#include "../sourcesdk/usercmd.hpp"
#include "../sourcesdk/player.hpp"

#include "../config/variable.hpp"

extern Config::Bool fastWalkEnable;

void Features::FastWalk::Think(SourceSDK::UserCmd* cmd) noexcept
{
	if (!fastWalkEnable)
		return;

	if (auto local_player = SourceSDK::BasePlayer::GetLocalPlayer();
		local_player && local_player->IsAlive() && local_player->IsOnGround())
	{
		// if we moving only forward or backward
		if ((cmd->HasButton(SourceSDK::Input_MoveForward) || cmd->HasButton(SourceSDK::Input_MoveBack))
			&&
			!(cmd->HasButton(SourceSDK::Input_MoveLeft) || cmd->HasButton(SourceSDK::Input_MoveRight)))
		{
			if (cmd->commandNumber % 2)
				cmd->move.Y() = SourceSDK::GetMaxSideSpeed() / 2.f;
			else
				cmd->move.Y() = -(SourceSDK::GetMaxSideSpeed() / 2.f);
		}
		// if we moving only left or right
		else if ((cmd->HasButton(SourceSDK::Input_MoveLeft) || cmd->HasButton(SourceSDK::Input_MoveRight))
				 &&
				 !(cmd->HasButton(SourceSDK::Input_MoveForward) || cmd->HasButton(SourceSDK::Input_MoveBack)))
		{
			if (cmd->commandNumber % 2)
				cmd->move.X() = SourceSDK::GetMaxForwardSpeed() / 2.f;
			else
				cmd->move.X() = -(SourceSDK::GetMaxForwardSpeed() / 2.f);
		}
	}
}
