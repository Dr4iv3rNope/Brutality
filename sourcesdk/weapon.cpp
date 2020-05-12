#include "weapon.hpp"
#include "../build.hpp"

#include "../util/pattern.hpp"
#include "../util/vmt.hpp"
#include "../util/memory.hpp"

const SourceSDK::FileWeaponInfo& SourceSDK::BaseWeapon::GetWeaponInfo()
{
	using GetWeaponInfoFn = FileWeaponInfo&(__thiscall*)(void*);

	#if BUILD_GAME_IS_GMOD
	/* "CHudCrosshair"
	
	mov     byte ptr [ebp+var_8+3], 0FFh
	mov     esi, [edi]
	mov     ecx, ebx
	push    eax
	call    GetWpnData
	mov     ecx, edi
	mov     eax, [eax+6E4h]
	push    eax
	call    dword ptr [esi+3Ch]
	pop     edi
	pop     esi
	pop     ebx
	mov     esp, ebp
	*/
	static const auto getWeaponInfo
	{
		Util::GetAbsAddress<GetWeaponInfoFn>(UTIL_XFIND_PATTERN(
			"client.dll", "8B 37 8B CB 50 E8 ?? ?? ?? ?? 8B CF 8B 80", 5
		))
	};
	#endif

	return getWeaponInfo(this);
}
