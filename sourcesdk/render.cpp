#include "render.hpp"
#include "sdk.hpp"

#include "../util/vmt.hpp"

const SourceSDK::VMatrix& SourceSDK::Render::WorldToScreenMatrix()
{
	// we need find yourself
	#if SOURCE_SDK_IS_GMOD
	/*
	push    ebp
	mov     ebp, esp
	mov     ecx, g_pRender
	mov     eax, [ecx]
	call    dword ptr [eax+34h] ; 34h - offset
	mov     ecx, [ebp+arg_0]
	mov     edx, [ebp+arg_4]
	movss   xmm0, dword ptr [eax]
	mulss   xmm0, dword ptr [ecx]
	movss   xmm2, dword ptr [eax+4]
	mulss   xmm2, dword ptr [ecx+4]
	addss   xmm2, xmm0
	*/
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll", "8B 0D ?? ?? ?? ?? 8B 01 FF 50 ?? 8B 4D 08 8B 55 0C", 10
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<const VMatrix&>(this, offset);
}
