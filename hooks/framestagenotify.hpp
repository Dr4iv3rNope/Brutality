#pragma once
#include "../util/vmt.hpp"

#include "../sourcesdk/framestage.hpp"
#include "../sourcesdk/matrix.hpp"

namespace Hooks
{
	void __stdcall FrameStageNotify(SourceSDK::ClientFrameStage);

	extern void CopyLastWorldToScreenMatrix(SourceSDK::VMatrix& matrix) noexcept;
}
