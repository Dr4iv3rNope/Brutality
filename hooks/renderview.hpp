#pragma once
#include "../valvesdk/interfaces.hpp"

#include "../util/vmt.hpp"

#include "../sourcesdk/viewsetup.hpp"
#include "../sourcesdk/matrix.hpp"

namespace Hooks
{
	extern void __fastcall RenderView(void* ecx, void* edx, const SourceSDK::ViewSetup& setup, int clearFlags, int whatToDraw);

	extern void CopyLastWorldToScreenMatrix(SourceSDK::VMatrix& matrix) noexcept;
}
