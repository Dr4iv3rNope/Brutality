#include "netchannel.hpp"
#include "sdk.hpp"

#include "../util/vmt.hpp"

#include "../util/debug/labels.hpp"

void SourceSDK::NetChannel::Shutdown(const char* reason)
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Shutdown net channel"));

	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("8B 01 8B CB C1 FF ?? 8D 14 B8")
	};
	#endif

	Util::Vmt::CallMethod<void, const char*>(this, offset, reason);

	UTIL_LABEL_OK();
}

void SourceSDK::NetChannel::SendNetMsg(INetMessage* message, bool force_reliable, bool voice)
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Sending net message"));

	// "Custom user info value"
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(int*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"8D 4D ?? E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8D 55 ?? 6A 00 6A 00 52 8B 01 FF 90", 
			26
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, INetMessage*, bool, bool>(this, offset, message, force_reliable, voice);

	UTIL_LABEL_OK();
}
