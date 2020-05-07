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
		local_player && local_player->IsOnGround())
	{
		// if we moving only forward or backward
		if ((cmd->HasButton(SourceSDK::InButton::Forward) || cmd->HasButton(SourceSDK::InButton::Back))
			&&
			!(cmd->HasButton(SourceSDK::InButton::MoveLeft) || cmd->HasButton(SourceSDK::InButton::MoveRight)))
		{
			if (cmd->commandNumber % 2)
				cmd->move.Y() = SourceSDK::MAX_CMD_MOVE_SPEED / 2.f;
			else
				cmd->move.Y() = -(SourceSDK::MAX_CMD_MOVE_SPEED / 2.f);
		}
		// if we moving only left or right
		else if ((cmd->HasButton(SourceSDK::InButton::MoveLeft) || cmd->HasButton(SourceSDK::InButton::MoveRight))
				 &&
				 !(cmd->HasButton(SourceSDK::InButton::Forward) || cmd->HasButton(SourceSDK::InButton::Back)))
		{
			if (cmd->commandNumber % 2)
				cmd->move.X() = SourceSDK::MAX_CMD_MOVE_SPEED / 2.f;
			else
				cmd->move.X() = -(SourceSDK::MAX_CMD_MOVE_SPEED / 2.f);
		}
	}
}
