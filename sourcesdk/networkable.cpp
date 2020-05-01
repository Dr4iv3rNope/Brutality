#include "networkable.hpp"

#include "../util/vmt.hpp"
#include "../util/pattern.hpp"
#include "../util/xorstr.hpp"

// "(%i) %s" in CEntityReportPanel_Paint
bool SourceSDK::Networkable::IsDormant()
{
	#if SOURCE_SDK_IS_GMOD
	/* "(%i) %s"
	
	mov     edx, [edi]
	mov     ecx, edi
	mov     edx, [edx+20h] ; 20h - offset
	call    edx
	test    al, al
	setz    [ebp+var_5]
	test    al, al
	jnz     short loc_100A4552
	movss   xmm0, dword_1049362C[esi*4]
	comiss  xmm0, ds:dword_10301898
	jb      short loc_100A4543
	mov     ebx, 0FAh
	lea     edi, [ebx-32h]
	mov     eax, edi
	jmp     short loc_100A4561
	*/
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"8B 17 8B CF 8B 52 ?? FF D2 84 C0 0F 94", 6
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<bool>(this, offset);
}

const SourceSDK::ClientClass* SourceSDK::Networkable::GetClientClass()
{
	#if SOURCE_SDK_IS_GMOD
	/* "mod_studio: MOVETYPE_FOLLOW with no model."

	mov     edx, [edi+8]
	lea     ecx, [edi+8]
	mov     edx, [edx+20h] ; 20h - offset
	call    edx
	test    al, al
	*/
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"8B 57 08 8D 4F 08 8B 52 ?? FF D2 84 C0", 8
		)) / 4
	};
	#endif
	
	return Util::Vmt::CallMethod<ClientClass*>(this, offset);
}
