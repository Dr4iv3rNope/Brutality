#include "cvar.hpp"
#include "../build.hpp"

#include "../util/vmt.hpp"
#include "../util/xorstr.hpp"

#include <cstdarg>

SourceSDK::IConVar* SourceSDK::CVar::FindVar(const char* name)
{
	/* "name"

	mov     rcx, cs:g_pCVar
	lea     rdx, aName      ; "name"
	mov     rsi, r8
	mov     rax, [rcx]
	call    qword ptr [rax+88h]
	test    rax, rax
	*/
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"8B D9 8B 0D ?? ?? ?? ?? 57 68 ?? ?? ?? ?? 8B 01 FF 50",
			18
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<IConVar*, const char*>(this, offset, name);
}
