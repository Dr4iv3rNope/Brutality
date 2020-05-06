#include "netsetconvar.hpp"
#include "sdk.hpp"

#include "clientstate.hpp"
#include "netchannel.hpp"

#include "../util/memory.hpp"
#include "../util/pattern.hpp"
#include "../util/strings.hpp"

#include "../util/debug/labels.hpp"

SourceSDK::NetSetConVar* SourceSDK::NetSetConVar::Create(const char* name, const char* value)
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Creating NET_SetConVar"));

	NetSetConVar* msg = (NetSetConVar*)new char[64] {};

	using ConstructorFn = void(__thiscall*)
		(NetSetConVar*, const char*, const char*);

	// "Custom user info value"
	#if SOURCE_SDK_IS_GMOD
	static const auto constructor
	{
		Util::GetAbsAddress<ConstructorFn>(UTIL_XFIND_PATTERN(
			"engine.dll",
			"53 57 8D 4D ?? E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8D 55 ?? 6A 00 6A 00",
			5
		))
	};
	#endif
	
	UTIL_XLOG(L"Running constructor");
	constructor(msg, name, value);
	
	UTIL_LABEL_OK();
	return msg;
}

bool SourceSDK::SendConVarValue(const char* name, const char* value)
{
	UTIL_LABEL_ENTRY(UTIL_WFORMAT(
		UTIL_XOR(L"Sending \"changed\" convar: ") <<
		Util::ToWideChar(name) << ' ' <<
		Util::ToWideChar(value)
	));

	if (!clientState->netChannel)
	{
		UTIL_XLOG(L"No channel!");
		UTIL_LABEL_OK();
		return false;
	}

	auto msg = NetSetConVar::Create(name, value);

	UTIL_XLOG(L"Sending message");
	clientState->netChannel->SendNetMsg(msg);

	delete msg;
	UTIL_LABEL_OK();
	return true;
}
