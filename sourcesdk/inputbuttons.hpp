#pragma once
#include "../util/flags.hpp"

namespace SourceSDK
{
	// in the source sdk is IN_*
	// there we got Input_
	UTIL_FLAGS_EX(Input_, InButtons)
	{
		UTIL_MAKE_BIT_FLAG(Input, Attack, 0),
		UTIL_MAKE_BIT_FLAG(Input, Jump, 1),
		UTIL_MAKE_BIT_FLAG(Input, Duck, 2),
		UTIL_MAKE_BIT_FLAG(Input, MoveForward, 3),		// +/- forward
		UTIL_MAKE_BIT_FLAG(Input, MoveBack, 4),			// +/- back
		UTIL_MAKE_BIT_FLAG(Input, Use, 5),
		UTIL_MAKE_BIT_FLAG(Input, Cancel, 6),
		UTIL_MAKE_BIT_FLAG(Input, TurnLeft, 7),			// +/- left
		UTIL_MAKE_BIT_FLAG(Input, TurnRight, 8),		// +/- right
		UTIL_MAKE_BIT_FLAG(Input, MoveLeft, 9),			// +/- moveleft
		UTIL_MAKE_BIT_FLAG(Input, MoveRight, 10),		// +/- moveright
		UTIL_MAKE_BIT_FLAG(Input, Attack2, 11),
		UTIL_MAKE_BIT_FLAG(Input, Run, 12),
		UTIL_MAKE_BIT_FLAG(Input, Reload, 13),
		UTIL_MAKE_BIT_FLAG(Input, Alt1, 14),
		UTIL_MAKE_BIT_FLAG(Input, Alt2, 15),
		UTIL_MAKE_BIT_FLAG(Input, Score, 16),
		UTIL_MAKE_BIT_FLAG(Input, Speed, 17),			// +/- speed
		UTIL_MAKE_BIT_FLAG(Input, Walk, 18),			// +/- walk
		UTIL_MAKE_BIT_FLAG(Input, Zoom, 19),
		UTIL_MAKE_BIT_FLAG(Input, Weapon1, 20),
		UTIL_MAKE_BIT_FLAG(Input, Weapon2, 21),
		UTIL_MAKE_BIT_FLAG(Input, Bullrush, 22),
		UTIL_MAKE_BIT_FLAG(Input, Grenade1, 23),
		UTIL_MAKE_BIT_FLAG(Input, Grenade2, 24)
	};
}
