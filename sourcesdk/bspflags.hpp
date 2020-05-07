#pragma once
#include "../util/flags.hpp"

namespace SourceSDK
{
	// public/bspflags.h:24
	UTIL_FLAGS(Contents)
	{
		Contents_Empty = 0,
		Contents_Solid = 0x1,
		Contents_Window = 0x2,
		Contents_Aux = 0x4,
		Contents_Grate = 0x8,
		Contents_Slime = 0x10,
		Contents_Water = 0x20,
		Contents_BlockLOS = 0x40,
		Contents_Opaque = 0x80,

		Contents_IgnoreNodrawOpaque = 0x2000,
		Contents_Moveable = 0x4000,
		Contents_AreaPortal = 0x8000,

		Contents_PlayerClip = 0x10000,
		Contents_MonsterClip = 0x20000,

		Contents_Origin = 0x1000000,
		Contents_Monster = 0x2000000,
		Contents_Debris = 0x4000000,
		Contents_Detail = 0x8000000,
		Contents_Translucent = 0x10000000,
		Contents_Ladder = 0x20000000,
		Contents_Hitbox = 0x40000000,
	};

	// public/bspflags.hpp:104
	UTIL_FLAGS(Mask)
	{
		Mask_All = 0xFFFFFFFF,

		Mask_Solid =
			Contents_Solid |
			Contents_Moveable |
			Contents_Window |
			Contents_Monster |
			Contents_Grate,

		Mask_PlayerSolid =
			Contents_Solid |
			Contents_Moveable |
			Contents_PlayerClip |
			Contents_Window |
			Contents_Monster |
			Contents_Grate,

		Mask_Shot =
			Contents_Solid |
			Contents_Moveable |
			Contents_Monster |
			Contents_Window |
			Contents_Debris |
			Contents_Grate
	};
}
