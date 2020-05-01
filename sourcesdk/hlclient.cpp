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
		// uid: 8963575431541765024
		VMT_XFIND_METHOD("A3 ?? ?? ?? ?? 83 F8 ?? 0F 87", 2)
	};
	#endif

	return offset;
}

std::size_t SourceSDK::HLClient::GetRenderViewIndex()
{
	// "%f seconds (%f fps)\n"
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"6A 03 6A 01 52 D9 9D ?? ?? ?? ?? 8B 01 FF 50",
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
