#include "hlinput.hpp"
#include "../build.hpp"

#include "../util/vmt.hpp"
#include "../util/pattern.hpp"

SourceSDK::UserCmd* SourceSDK::Input::GetUserCmd(int sequence_number)
{
	// "StorePredictionResults"
	// in PerformPrediction
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN("client.dll", "53 8B 01 FF 50 ?? 8B D0 89 55 ?? 85 D2", 5)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<UserCmd*, int>(this, offset, sequence_number);
}
