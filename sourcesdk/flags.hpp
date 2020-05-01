#pragma once

namespace SourceSDK
{
	// public/const.h:97
	enum class Flags : int
	{
		OnGround = 1 << 0,
		Ducking = 1 << 1,
		WaterJump = 1 << 2,
		OnTrain = 1 << 3,
		InRain = 1 << 4,
		Frozen = 1 << 5,
		AtControls = 1 << 6,
		Client = 1 << 7,
		FakeClient = 1 << 8,
		InWater = 1 << 9,
		Fly = 1 << 10,
		Swim = 1 << 11,
	};
}