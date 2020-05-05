#include "sdk.hpp"
#include "engineclient.hpp"

#include "../util/xorstr.hpp"
#include "../util/vmt.hpp"

void SourceSDK::EngineClient::ServerCmd(const char* cmd, bool reliable)
{
	// "vehicleRole %i\n"
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"8B 0D ?? ?? ?? ?? 8D 55 ?? 83 C4 ?? 8B 01 6A 01 52 FF 50",
			19
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, const char*, bool>(this, offset, cmd, reliable);
}

bool SourceSDK::EngineClient::GetPlayerInfo(int entidx, PlayerInfo& info)
{
	// "UpdatePlayerName with bogus slot %d\n"
	// "unconnected"
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"FF D0 84 C0 74 ?? 8B 0D ?? ?? ?? ?? 8D 95 ?? ?? ?? ?? 52 56 8B 01 8B 40",
			24
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<bool, int, PlayerInfo*>(this, offset, entidx, &info);
}

void SourceSDK::EngineClient::ClientCmdUnrestricted(const char* cmd)
{
	// "gameui_preventescapetoshow\n"
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(int*)UTIL_XFIND_PATTERN(
			"client.dll",
			"FF 90 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B 01 FF 90 ?? ?? ?? ?? 8D 45",
			21
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, const char*>(this, offset, cmd);
}
