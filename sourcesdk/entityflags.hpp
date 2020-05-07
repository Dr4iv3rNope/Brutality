#pragma once
#include "../util/flags.hpp"

namespace SourceSDK
{
	// public/const.h:97
	UTIL_FLAGS(EntityFlags)
	{
		EntityFlags_OnGround = 1 << 0,
		EntityFlags_Ducking = 1 << 1,
		EntityFlags_WaterJump = 1 << 2,
		EntityFlags_OnTrain = 1 << 3,
		EntityFlags_InRain = 1 << 4,
		EntityFlags_Frozen = 1 << 5,
		EntityFlags_AtControls = 1 << 6,
		EntityFlags_Client = 1 << 7,
		EntityFlags_FakeClient = 1 << 8,
		EntityFlags_InWater = 1 << 9,
		EntityFlags_Fly = 1 << 10,
		EntityFlags_Swim = 1 << 11,
		EntityFlags_Conveyor = 1 << 12,
		EntityFlags_NPC = 1 << 13,
		EntityFlags_Godmode = 1 << 14,
		EntityFlags_OnFire = 1 << 27
	};
}