#pragma once
#include "../util/flags.hpp"

namespace SourceSDK
{
	UTIL_FLAGS(MaterialVar)
	{
		UTIL_MAKE_BIT_FLAG(MaterialVar, Debug, 0),
		UTIL_MAKE_BIT_FLAG(MaterialVar, NoDrawOverride, 1),
		UTIL_MAKE_BIT_FLAG(MaterialVar, NoDraw, 2),
		UTIL_MAKE_BIT_FLAG(MaterialVar, UseInFillrateMode, 3),

		UTIL_MAKE_BIT_FLAG(MaterialVar, VertexColor, 4),
		UTIL_MAKE_BIT_FLAG(MaterialVar, VertexAlpha, 5),
		UTIL_MAKE_BIT_FLAG(MaterialVar, SelFillum, 6),
		UTIL_MAKE_BIT_FLAG(MaterialVar, Additive, 7),
		UTIL_MAKE_BIT_FLAG(MaterialVar, AlphaTest, 8),
		UTIL_MAKE_BIT_FLAG(MaterialVar, Multipass, 9),
		UTIL_MAKE_BIT_FLAG(MaterialVar, ZNearer, 10),
		UTIL_MAKE_BIT_FLAG(MaterialVar, Model, 11),
		UTIL_MAKE_BIT_FLAG(MaterialVar, Flat, 12),
		UTIL_MAKE_BIT_FLAG(MaterialVar, NoCull, 13),
		UTIL_MAKE_BIT_FLAG(MaterialVar, NoFog, 14),
		UTIL_MAKE_BIT_FLAG(MaterialVar, IgnoreZ, 15),
		UTIL_MAKE_BIT_FLAG(MaterialVar, Decal, 16),
		UTIL_MAKE_BIT_FLAG(MaterialVar, EnvmapSphere, 17),
		UTIL_MAKE_BIT_FLAG(MaterialVar, NoAlphaMod, 18),
		UTIL_MAKE_BIT_FLAG(MaterialVar, EnvmapCameraSpace, 19),
		UTIL_MAKE_BIT_FLAG(MaterialVar, BaseAlphaEnvmapMask, 20),
		UTIL_MAKE_BIT_FLAG(MaterialVar, Translucent, 21),
		UTIL_MAKE_BIT_FLAG(MaterialVar, NormalMapAlphaEnvmapMask, 22),
		UTIL_MAKE_BIT_FLAG(MaterialVar, NeedsSoftwareSkinning, 23),
		UTIL_MAKE_BIT_FLAG(MaterialVar, OpaqueTexture, 24),
		UTIL_MAKE_BIT_FLAG(MaterialVar, EnvmapMode, 25),
		UTIL_MAKE_BIT_FLAG(MaterialVar, SuppressDecals, 26),
		UTIL_MAKE_BIT_FLAG(MaterialVar, HalfLambert, 27),
		UTIL_MAKE_BIT_FLAG(MaterialVar, WireFrame, 28),
		UTIL_MAKE_BIT_FLAG(MaterialVar, AllowAlphaToCoverage, 29)
	};
}
