#pragma once
#include "../valvesdk/interfaces.hpp"

#include "../util/vmt.hpp"

#include "../sourcesdk/framestage.hpp"

namespace Hooks
{
	VALVE_SDK_INTERFACE_DECL(Util::Vmt::HookedMethod, oldFrameStageNotify);
	void __stdcall FrameStageNotify(SourceSDK::ClientFrameStage);
}
