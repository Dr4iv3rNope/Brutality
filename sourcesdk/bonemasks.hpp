#pragma once
#include "../util/flags.hpp"

namespace SourceSDK
{
	// public/studio.h:319
	UTIL_FLAGS(BoneMask)
	{
		BoneMask_Anything	= 0x0007FF00,
		BoneMask_Hitbox		= 0x00000100,	// bone (or child) is used by a hit box
		BoneMask_Attachment	= 0x00000200,	// bone (or child) is used by an attachment point
		BoneMask_VertexMask	= 0x0003FC00,
		BoneMask_VertexLod0	= 0x00000400,	// bone (or child) is used by the toplevel model via skinned vertex
		BoneMask_VertexLod1	= 0x00000800,
		BoneMask_VertexLod2	= 0x00001000,
		BoneMask_VertexLod3	= 0x00002000,
		BoneMask_VertexLod4	= 0x00004000,
		BoneMask_VertexLod5	= 0x00008000,
		BoneMask_VertexLod6	= 0x00010000,
		BoneMask_VertexLod7	= 0x00020000,
		BoneMask_BoneMerge	= 0x00040000
	};
}
