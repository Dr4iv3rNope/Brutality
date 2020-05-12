#pragma once
#include "const.hpp"
#include "vector.hpp"
#include "../build.hpp"

#include "../util/pad.hpp"

namespace SourceSDK
{
	using MDLHandle = unsigned short;

	struct Model final
	{
		enum class Type { Bad, Brush, Sprite, Studio };

		FileHandle fileHandle;

		#if BUILD_GAME_IS_GMOD
		const char* name;
		#else
		char name[MAX_QPATH];
		#endif

		int loadFlags;

		#if BUILD_GAME_IS_GMOD
		UTIL_PAD(0, 12);
		#endif

		int serverCount;

		#if BUILD_GAME_IS_GMOD
		UTIL_PAD(1, 4);
		#endif

		Type type;

		#if BUILD_GAME_IS_GMOD
		UTIL_PAD(2, 4);
		#endif

		Vector3 mins, maxs;
		float radius;

		union
		{
			void* brushData;
			MDLHandle studioHandle;
			void* spriteData;
		};
	};
}
