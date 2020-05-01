#pragma once
#include "../util/vmt.hpp"

#include "../sourcesdk/framestage.hpp"

namespace Hooks
{
	extern Util::Vmt::HookedMethod* oldFrameStageNotify;
	void __stdcall FrameStageNotify(SourceSDK::ClientFrameStage);
}
