#pragma once
#include "../build.hpp"

#include "const.hpp"

#include "../util/pad.hpp"

namespace SourceSDK
{
	enum class WeaponSound
	{
		Empty,
		Single, SingleNPC,
		WeaponDouble, DoubleNPC,
		Burst,
		Reload, ReloadNPC,
		MeleeMiss, MeleeHit, MeleeHitWorld,
		Special1, Special2, Special3
	};

	constexpr int MAX_SHOOT_SOUND_TYPES { int(WeaponSound::Special3) };

	struct FileWeaponInfo final
	{
		UTIL_PAD(vmt, 4);

		bool isParsedScript;
		bool isLoadedHudElements;

		char classname[MAX_WEAPON_STRING];
		char printname[MAX_WEAPON_STRING];
		char viewModel[MAX_WEAPON_STRING];
		char worldModel[MAX_WEAPON_STRING];
		char animationPrefix[MAX_WEAPON_PREFIX];

		struct
		{
			int slot, pos;
		} inventory;

		int maxClip1, maxClip2;
		int defaultClip1, defaultClip2;

		int weight;
		int rumbleEffect;
		
		#if BUILD_GAME_IS_GMOD
		UTIL_PAD(0, 2);
		#endif

		bool shouldAutoSwitchTo;
		bool shouldAutoSwitchFrom;

		int flags;

		char primaryAmmoName[MAX_WEAPON_AMMO_LENGTH];
		char secondaryAmmoName[MAX_WEAPON_AMMO_LENGTH];

		char shootSounds[MAX_SHOOT_SOUND_TYPES][MAX_WEAPON_STRING];

		int primaryAmmoType, secondaryAmmoType;
		
		bool isMeleeWeapon;
		bool isRightHanded;
		bool allowFlipping;

		inline auto GetShootSound(WeaponSound sound) const noexcept
		{
			return this->shootSounds[int(sound)];
		}
	};
}
