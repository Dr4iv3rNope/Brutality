#include "netchannel.hpp"
#include "../build.hpp"

#include "../util/vmt.hpp"

#include "../util/debug/labels.hpp"

void SourceSDK::NetChannel::Shutdown(const char* reason)
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Shutdown net channel"));

	// "Disconnect by server.\n"
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(int*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"56 8B F1 8B 8E ?? ?? ?? ?? 85 C9 74 ?? 8B 01 68 ?? ?? ?? ?? FF 90",
			22
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, const char*>(this, offset, reason);

	UTIL_LABEL_OK();
}

void SourceSDK::NetChannel::SendNetMsg(INetMessage* message, bool force_reliable, bool voice)
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Sending net message"));

	// "Custom user info value"
	#if BUILD_GAME_IS_GMOD
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

float SourceSDK::NetChannel::GetLatency(Flow flow)
{
	// "StartLagCompensation"
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"server.dll",
			"FF D0 8B C8 85 C9 74 ?? 8B 01 6A 00 8B 40 ?? FF D0",
			14
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<float, Flow>(this, offset, flow);
}
