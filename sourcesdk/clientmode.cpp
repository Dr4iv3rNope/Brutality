#include "clientmode.hpp"
#include "../build.hpp"

#include "../util/vmt.hpp"
#include "../util/xorstr.hpp"

std::size_t SourceSDK::ClientModeShared::GetCreateMoveIndex()
{
	// "Auto-disconnect in %.1f seconds"
	// in CL_Move
	//
	// g_ClientDLL->CreateMove( ... )
	// input->CreateMove( ... )
	// g_pClientMode->CreateMove( ... )
	//
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("E8 ?? ?? ?? ?? 8B C8 85 C9 75 06 B0 01 5D C2 08 00")
	};
	#endif

	return offset;
}

std::size_t SourceSDK::ClientModeShared::GetOverrideViewIdx()
{
	// "angles", "znear", "ortho", "right", "bottom", "drawviewer", "origin"
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		// uid: 15450069552393688749
		VMT_XFIND_METHOD("C6 05 ?? ?? ?? ?? 00 57 E8 ?? ?? ?? ?? 8B F8")
	};
	#endif

	return offset;
}
