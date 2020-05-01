#pragma once

namespace SourceSDK
{
	constexpr int MAX_PLAYER_NAME_LENGTH{ 128 };	// public/const.h:38
	constexpr int SIGNED_GUID_LEN{ 32 };			// public/const.h:63
	constexpr int MAX_CUSTOM_FILES{ 4 };			// public/const.h:87
	constexpr int MAX_STUDIO_BONES { 128 };			// public/studio.h:86
	constexpr int MAX_QPATH { 96 };					// common/qlimits.h:20
	constexpr int MAX_STUDIO_NAME_LENGTH { 64 };
	constexpr int MAX_WEAPONS { 48 };				// game/shared/shareddefs.h:102
	constexpr int MAX_WEAPON_STRING { 80 };			// game/shared/weapon_parse.h:48
	constexpr int MAX_WEAPON_PREFIX { 16 };			// game/shared/weapon_parse.h:49
	constexpr int MAX_WEAPON_AMMO_LENGTH { 32 };	// game/shared/weapon_parse.h:50

	using CRC32 = unsigned long;
	using FileHandle = void*;
}
