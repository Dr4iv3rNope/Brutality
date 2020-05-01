#include "surface.hpp"
#include "sdk.hpp"

#include "../util/memory.hpp"
#include "../util/vmt.hpp"
#include "../util/xorstr.hpp"

bool SourceSDK::MatSystemSurface::IsCursorLocked()
{
	#if SOURCE_SDK_IS_GMOD
	/* "VGUI Layout" "CEngineVGui::Simulate"
	
	call    sub_10202FE0    ; #STR: "%3i:  %s\n"
	call    sub_10208DE0    ; #STR: "<shouldn't see this>"
	call    sub_100BAC10    ; #STR: "Texture Memory Usage: %s Kb / %s Kb"
	mov     ecx, g_pSurface
	mov     eax, [ecx]
	call    dword ptr [eax+178h]
	cmp     g_pClientDLL, 0
	pop     esi
	jz      short loc_10204E22
	mov     ecx, off_10415DD8
	mov     eax, [ecx]
	mov     eax, [eax+40h]
	call    eax
	test    al, al
	jz      short loc_10204E17
	mov     ecx, g_pSurface
	mov     eax, [ecx]
	mov     eax, [eax+1A4h] ; 1A4 - offset
	call    eax
	test    al, al
	*/
	static const auto offset
	{
		(*(int*)UTIL_XFIND_PATTERN(
			"engine.dll", "8B 80 ?? ?? ?? ?? FF D0 84 C0 74 ?? 80 3D ?? ?? ?? ?? 00 75 ?? 8B 0D", 2
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<bool>(this, offset);
}

std::size_t SourceSDK::MatSystemSurface::GetLockCursorIndex()
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(int*)UTIL_XFIND_PATTERN(
			"vguimatsurface.dll", "FF 90 ?? ?? ?? ?? 8B 07 8B CF FF 90 ?? ?? ?? ?? 5F 8B E5 5D C3", 12
		)) / 4
	};
	#endif

	return offset;
}

std::size_t SourceSDK::MatSystemSurface::GetSetCursorIndex()
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(int*)UTIL_XFIND_PATTERN(
			"vguimatsurface.dll", "8B 07 8B CF 6A 02 FF 90 ?? ?? ?? ?? 5F 8B E5", 8
		)) / 4
	};
	#endif

	return offset;
}

/*std::size_t SourceSDK::MatSystemSurface::GetPaintTraverseExIndex()
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("8B 06 8B CE 6A 01 6A 01 6A 00 FF 50 ?? F3 0F")
	};
	#endif

	return offset;
}*/

void SourceSDK::MatSystemSurface::UnlockCursor()
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(int*)UTIL_XFIND_PATTERN(
			"vguimatsurface.dll", "8B CF 8B 07 5E 5B 74 ?? FF 90", 10
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void>(this, offset);
}
