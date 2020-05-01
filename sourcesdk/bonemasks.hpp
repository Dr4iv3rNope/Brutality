#pragma once

namespace SourceSDK
{
	// public/studio.h:319
	enum class BoneMask
	{
		Anything	= 0x0007FF00,
		Hitbox		= 0x00000100,	// bone (or child) is used by a hit box
		Attachment	= 0x00000200,	// bone (or child) is used by an attachment point
		VertexMask	= 0x0003FC00,
		VertexLod0	= 0x00000400,	// bone (or child) is used by the toplevel model via skinned vertex
		VertexLod1	= 0x00000800,
		VertexLod2	= 0x00001000,
		VertexLod3	= 0x00002000,
		VertexLod4	= 0x00004000,
		VertexLod5	= 0x00008000,
		VertexLod6	= 0x00010000,
		VertexLod7	= 0x00020000,
		BoneMerge	= 0x00040000
	};

	using BoneMaskFlags = int;
}
