#include "bhop.hpp"

#include "../sourcesdk/usercmd.hpp"
#include "../sourcesdk/inputsystem.hpp"

#include "../config/variable.hpp"

#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/player.hpp"

#include <optional>

extern Config::Bool bhopEnable;
extern Config::Bool bhopAutoStrafe;

//extern Config::LFloat bhopMinDelay;
//extern Config::LFloat bhopMaxDelay;


static inline void AutoStrafe(SourceSDK::UserCmd* cmd) noexcept
{
	if (cmd->mouseX > 0)
		cmd->move.y = SourceSDK::MAX_CMD_MOVE_SPEED;
	else if (cmd->mouseX < 0)
		cmd->move.y = -SourceSDK::MAX_CMD_MOVE_SPEED;
}

void Features::BunnyHop::Think(SourceSDK::UserCmd* cmd) noexcept
{
	if (!bhopEnable)
		return;

	if (!SourceSDK::inputsystem->IsButtonDown(SourceSDK::ButtonCode::KeySpace))
		return;

	auto localPlayer = SourceSDK::BasePlayer::GetLocalPlayer();

	if (!localPlayer || localPlayer->IsFly())
		return;

	/*static std::optional<float> nextJump;

	if (*bhopMinDelay < *bhopMaxDelay)
	{
		if (!nextJump.has_value())
		{
			const auto rand_delay = float(rand()) % int(*bhopMaxDelay / 100.f);
			const auto delay_result (*bhopMinDelay / 1000.f) + 

			float delay_result = (*bhopMinDelay / 1000.f) + (float(rand() % (int(*bhopMaxDelay / 1000.f) * 10)) / 10.f);

			nextJump = SourceSDK::globals->curTime + delay_result;
			return;
		}
		else
		{
			if (SourceSDK::globals->curTime <= *nextJump)
				return;
			else
				nextJump.reset();
		}
	}
	else
		return;*/

	if (localPlayer->IsOnGround())
		cmd->AddButton(SourceSDK::InButton::Jump);
	else
	{
		cmd->RemoveButton(SourceSDK::InButton::Jump);

		if (*bhopAutoStrafe)
			AutoStrafe(cmd);
	}
}
