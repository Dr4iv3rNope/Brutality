#pragma once

namespace SourceSDK
{
	enum class WeaponState
	{
		NotCarried, // weapon is on the ground
		CarriedByPlayer,
		Active,

		IsOnTarget = 0x40
	};
}
