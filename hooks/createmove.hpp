#pragma once
#include "../valvesdk/interfaces.hpp"

#include "../util/vmt.hpp"

namespace SourceSDK
{
	struct UserCmd;
	class ClientModeShared;
}

namespace Hooks
{
	VALVE_SDK_INTERFACE_DECL(Util::Vmt::HookedMethod, oldCreateMove);
	extern bool __fastcall CreateMove(SourceSDK::ClientModeShared*, void*, float input_sample_frametime, SourceSDK::UserCmd* cmd) noexcept;
}
