#include "modelcache.hpp"
#include "sdk.hpp"

#include "../util/vmt.hpp"
#include "../util/pattern.hpp"

const SourceSDK::StudioHDR* SourceSDK::MDLCache::GetStudioHDR(MDLHandle handle)
{
	#if SOURCE_SDK_IS_GMOD
	/*
	"C_BaseAnimating::DrawModel"
	C_BaseAnimating::InternalDrawModel
	CModelRender::DrawModelSetup

	mov     ecx, g_pMDLCache
	mov     eax, [ebx+1Ch]
	push    edi
	mov     [ebp+var_8], esi
	mov     edx, [ecx]
	movzx   eax, word ptr [eax+38h]
	push    eax
	call    dword ptr [edx+28h] ; 28h - offset
	*/
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll", "8B 43 ?? 57 89 75 ?? 8B 11 0F B7 40 ?? 50 FF 52", 16
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<StudioHDR*, MDLHandle>(this, offset, handle);
}
