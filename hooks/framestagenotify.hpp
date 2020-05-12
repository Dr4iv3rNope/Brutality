#pragma once
#include "../util/vmt.hpp"

#include "../sourcesdk/framestage.hpp"

namespace Hooks
{
	void __stdcall FrameStageNotify(SourceSDK::ClientFrameStage);
}
