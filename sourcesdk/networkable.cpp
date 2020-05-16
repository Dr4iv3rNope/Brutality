#include "networkable.hpp"

#include "../build.hpp"

#include "../util/vmt.hpp"
#include "../util/pattern.hpp"
#include "../util/xorstr.hpp"

// "(%i) %s" in CEntityReportPanel_Paint
bool SourceSDK::Networkable::IsDormant()
{
	#if BUILD_GAME_IS_GMOD
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
	#if BUILD_GAME_IS_GMOD
	// "'%s'"
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"85 C0 74 ?? C6 85 ?? ?? ?? ?? 00 8B C8 8B 10 FF 52",
			17
		)) / 4
	};
	#endif
	
	return Util::Vmt::CallMethod<ClientClass*>(this, offset);
}

int SourceSDK::Networkable::GetEntityIndex()
{
	// "'%s' (entindex %d) %s" in CC_CL_Find_Ent
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"BA ?? ?? ?? ?? 0F 44 D0 8B 46 08 52 FF 50",
			14
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<int>(this, offset);
}
