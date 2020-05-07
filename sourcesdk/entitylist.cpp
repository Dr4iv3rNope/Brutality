#pragma once
#include "entitylist.hpp"
#include "sdk.hpp"

#include "../util/vmt.hpp"
#include "../util/xorstr.hpp"

SourceSDK::BaseEntity* SourceSDK::ClientEntityList::GetEntity(int idx)
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("8B 45 08 85 C0 78 ?? 3D ?? ?? ?? ?? 7D ?? 05 ?? ?? ?? ?? 03 C0")
	};
	#endif

	return Util::Vmt::CallMethod<BaseEntity*, int>(this, offset, idx);
}

SourceSDK::Networkable* SourceSDK::ClientEntityList::GetNetworkable(EntityHandle handle)
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("8B D4 89 02 8B 01 FF 50")
	};
	#endif

	return Util::Vmt::CallMethod<Networkable*, EntityHandle>(this, offset, handle);
}

int SourceSDK::ClientEntityList::GetMaxEntities()
{
	#if SOURCE_SDK_IS_GMOD
	/* "'%s'"
	
	push    ebp
	mov     ebp, esp
	mov     ecx, entitylist
	sub     esp, 200h
	mov     eax, [ecx]
	push    esi
	xor     esi, esi
	call    dword ptr [eax+20h] ; 20h - offset
	test    eax, eax
	*/
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"8B 01 56 33 F6 FF 50 ?? 85 C0", 7
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<int>(this, offset);
}
