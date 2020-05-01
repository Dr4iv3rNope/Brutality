#include "concommand.hpp"

#include "../util/pattern.hpp"
#include "../util/xorstr.hpp"
#include "../util/strings.hpp"

#include "../util/debug/labels.hpp"

SourceSDK::ConCommand* SourceSDK::CreateConCommand(const char* name, ConCommand::CallbackFn callback,
	const char* help_string, int flags, ConCommand::AutoCompleteFn ac_callback)
{
	UTIL_LABEL_ENTRY(UTIL_WFORMAT(
		UTIL_XOR(L"Create ConCommand ") << Util::ToWString(name)
	));

	using ConstructorFn = ConCommand*(*__thiscall)(ConCommand*,
		const char*, ConCommand::CallbackFn, const char*, int, ConCommand::AutoCompleteFn);

	//ConCommand* command = (ConCommand*)new char[ConCommand::SIZE]{};
	ConCommand* command { nullptr };

	#if SOURCE_SDK_IS_GMOD
	/*
	push    0
	push    0
	push    offset aDumpSoundsToVx ; "Dump sounds to VXConsole"
	push    offset sub_1005D030 ; #STR: "SERVER", "%02i %s l(%03d) c(%03d) r(%03d) rl(%03d) rr(%03d) vol(%03d, "CLIENT"
	push    offset aSndDumpclients ; "snd_dumpclientsounds"
	mov     ecx, offset unk_104396D8
	call    ConCommand_Constructor ; <------------ we need this
	push    offset sub_102F6F80 ; void (__cdecl *)()
	call    _atexit
	pop     ecx
	retn
	*/
	static const auto constructor
	{
		ConstructorFn UTIL_XFIND_PATTERN("engine.dll", "8B 45 08 80 E1 ?? 80 C9 ?? 89 46 0C 8B 45 10", -0x37)
	};
	#endif

	// TODO: access violation in engine.dll
	UTIL_XLOG(L"Running constructor");
	constructor(command, name, callback, help_string, flags, ac_callback);

	UTIL_LABEL_OK();
	return command;
}
