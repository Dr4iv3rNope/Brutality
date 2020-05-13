#pragma once
#include "../valvesdk/interfaces.hpp"

#include "../util/vmt.hpp"

#include "../sourcesdk/modelrender.hpp"

namespace Hooks
{
	extern void __fastcall DrawModelExecute(SourceSDK::ModelRender* edx, void* ecx,
											const SourceSDK::DrawModelState& state,
											const SourceSDK::ModelRenderInfo& info,
											SourceSDK::Matrix3x4* boneToWorld);

	extern void OldDrawModelExecute(const SourceSDK::DrawModelExecuteArgs& args);
}
