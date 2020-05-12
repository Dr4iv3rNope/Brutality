#pragma once
#include "../util/vmt.hpp"

namespace SourceSDK
{
	struct UserCmd;
	class ClientModeShared;
}

namespace Hooks
{
	extern bool __fastcall CreateMove(SourceSDK::ClientModeShared*, void*, float input_sample_frametime, SourceSDK::UserCmd* cmd) noexcept;
}
