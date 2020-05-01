#include "player.hpp"

#include "../util/memory.hpp"
#include "../util/xorstr.hpp"
#include "../util/vmt.hpp"

SourceSDK::BasePlayer* SourceSDK::BasePlayer::GetLocalPlayer()
{
	using GetLocalPlayerFn = BasePlayer*(__cdecl*)();

	/* "CHudCrosshair"
	
	call GetLocalPlayer
	mov esi, eax
	test esi, esi
	*/
	#if SOURCE_SDK_IS_GMOD
	static const auto getLocalPlayer
	{
		Util::GetAbsAddress<GetLocalPlayerFn>(
			UTIL_XFIND_PATTERN(
				"client.dll",
				"E8 ?? ?? ?? ?? 8B F0 85 F6 0F 84 ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? B9",
				0
			)
		)
	};
	#endif

	return getLocalPlayer();
}

SourceSDK::BaseWeapon* SourceSDK::BasePlayer::GetActiveWeapon()
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("3B 35 ?? ?? ?? ?? 75 ?? 8B 06")
	};
	#endif

	return Util::Vmt::CallMethod<BaseWeapon*>(this, offset);
}
