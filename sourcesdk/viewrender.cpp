#include "viewrender.hpp"
#include "../build.hpp"

#include "../util/vmt.hpp"

std::size_t SourceSDK::ViewRender::GetRenderViewIndex()
{
	// "Couldn't write bitmap data snapshot.\n"
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"F3 0F 11 85 ?? ?? ?? ?? FF 50 ?? 8B C7",
			10
		)) / 4
	};
	#endif

	return offset;
}
