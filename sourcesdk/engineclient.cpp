#include "sdk.hpp"
#include "engineclient.hpp"

#include "../util/xorstr.hpp"
#include "../util/vmt.hpp"

void SourceSDK::EngineClient::ServerCmd(const char* cmd)
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("81 EC ?? ?? ?? ?? 8D 85 ?? ?? ?? ?? FF 75 08 68 ?? ?? ?? ?? 68 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4")
	};
	#endif

	Util::Vmt::CallMethod<void, const char*>(this, offset, cmd);
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
			"FF 90 ?? ?? ?? ?? 8D 45 08 8B CE 50 8D 45 FC 50 E8 ?? ?? ?? ?? 81 7D FC",
			2
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, const char*>(this, offset, cmd);
}
