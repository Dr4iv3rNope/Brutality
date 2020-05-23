#include "drawmodelexecute.hpp"

#include "../shutdown.hpp"

#include "../features/chams.hpp"
#include "../features/backtrack.hpp"

void __fastcall Hooks::DrawModelExecute(SourceSDK::ModelRender* edx, void* ecx,
										const SourceSDK::DrawModelState& state,
										const SourceSDK::ModelRenderInfo& info,
										SourceSDK::Matrix3x4* boneToWorld)
{
	_SHUTDOWN_GUARD;

	bool chams = Features::Chams::Render(SourceSDK::DrawModelExecuteArgs(state, info, boneToWorld));
	Features::Backtrack::DrawBacktrack(SourceSDK::DrawModelExecuteArgs(state, info, boneToWorld));

	if (chams)
		interfaces->modelrender->ForcedMaterialOverride(nullptr);
	else
		reinterpret_cast<decltype(DrawModelExecute)*>
		(hooks->oldDrawModelExecute->GetOriginal())
		(edx, ecx, state, info, boneToWorld);
}

void Hooks::OldDrawModelExecute(const SourceSDK::DrawModelExecuteArgs& args)
{
	UTIL_ASSERT(hooks->oldDrawModelExecute, "Tried to run old dme but it's not hooked");

	reinterpret_cast<void(__thiscall*)(void*, const SourceSDK::DrawModelState&, const SourceSDK::ModelRenderInfo&, SourceSDK::Matrix3x4*)>
		(hooks->oldDrawModelExecute->GetOriginal())
		(interfaces->modelrender, *args.state, *args.info, args.boneToWorld);
}
