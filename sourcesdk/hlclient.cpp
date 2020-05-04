#include "hlclient.hpp"
#include "sdk.hpp"

#include "../util/vmt.hpp"

std::size_t SourceSDK::HLClient::GetFrameStageNotifyIndex()
{
	// find "CL_ProcessPacketEntities: frame window too big (>%i)\n"
	// in CL_ProcessPacketEntities
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("A3 ?? ?? ?? ?? 83 F8 ?? 0F 87", 2)
	};
	#endif

	return offset;
}

std::size_t SourceSDK::HLClient::GetCreateMoveIndex()
{
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"51 8B 0D ?? ?? ?? ?? F3 ?? ?? ?? ?? 57 FF 56",
			15
		)) / 4
	};
	#endif

	return offset;
}

const SourceSDK::ClientClass* SourceSDK::HLClient::GetHeadClientClass()
{
	return Util::Vmt::CallMethod<ClientClass*>(this, 8);
}
