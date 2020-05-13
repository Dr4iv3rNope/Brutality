#include "drawmodelexecute.hpp"

#include "../shutdown.hpp"

#include "../features/chams.hpp"

void __fastcall Hooks::DrawModelExecute(SourceSDK::ModelRender* edx, void* ecx,
										const SourceSDK::DrawModelState& state,
										const SourceSDK::ModelRenderInfo& info,
										SourceSDK::Matrix3x4* boneToWorld)
{
	SHUTDOWN_HOOK_GUARD(L"DrawModelExecute");
	MAKE_BUSY_SHUTDOWN_GUARD;

	if (Features::Chams::Render(SourceSDK::DrawModelExecuteArgs(state, info, boneToWorld)))
	{
		interfaces->modelrender->ForcedMaterialOverride(nullptr);
		return;
	}

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
