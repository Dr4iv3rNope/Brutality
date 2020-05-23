#include "convar.hpp"
#include "../build.hpp"

#include "../util/pattern.hpp"
#include "../util/xorstr.hpp"
#include "../util/vmt.hpp"
#include "../util/strings.hpp"

#include "../util/debug/labels.hpp"

SourceSDK::IConVar* SourceSDK::CreateConVar(const char* name, const char* default_value, int flags, const char* help_string,
	bool min, float min_value,
	bool max, float max_value,
	IConVar::ChangeCallbackFn callback)
{
	UTIL_LABEL_ENTRY(UTIL_WFORMAT(
		UTIL_XOR(L"Creating convar ") << Util::ToWideChar(name)
	));

	IConVar* convar = (IConVar*)new char[IConVar::SIZE]{};

	using ConstructorFn = IConVar*(*__thiscall)(IConVar*, const char*, const char*, int, const char*,
		bool, float, bool, float,
		IConVar::ChangeCallbackFn);

	#if BUILD_GAME_IS_GMOD
	/*
	fldz
	push    offset con_logfile_callback ; int
	push    ecx
	fst     [esp+8+var_8]   ; float
	push    0               ; int
	push    ecx
	fstp    [esp+10h+var_10] ; float
	push    0               ; int
	push    offset aConsoleOutputG ; "Console output gets written to this fil"...
	push    0               ; int
	push    offset MultiByteStr ; int
	push    offset aConLogfile ; "con_logfile"
	mov     ecx, offset unk_1050A690
	call    ConVar_Constructor_1
	push    offset sub_102F8050 ; void (__cdecl *)()
	call    _atexit
	pop     ecx
	retn
	*/
	static const auto constructor
	{
		ConstructorFn UTIL_XFIND_PATTERN("engine.dll", "55 8B EC D9 45 24 56 FF 75 28", 0)
	};
	#endif

	UTIL_XLOG(L"Running constructor");
	constructor(convar, name, default_value, flags, help_string, min, min_value, max, max_value, callback);

	UTIL_LABEL_OK();
	return convar;
}
