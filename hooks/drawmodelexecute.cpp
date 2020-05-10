#include "drawmodelexecute.hpp"

#include "../shutdown.hpp"

#include "../features/chams.hpp"

void __fastcall Hooks::DrawModelExecute(SourceSDK::ModelRender* edx, void* ecx,
										const SourceSDK::DrawModelState& state,
										const SourceSDK::ModelRenderInfo& info,
										SourceSDK::Matrix3x4* boneToWorld)
{
	CREATE_SHUTDOWN_HOOK_GUARD(L"DrawModelExecute", oldDrawModelExecute);
	MAKE_BUSY_SHUTDOWN_GUARD;

	if (Features::Chams::Render(state, info, boneToWorld))
		return;

	reinterpret_cast<decltype(DrawModelExecute)*>
		(oldDrawModelExecute->GetOriginal())
		(edx, ecx, state, info, boneToWorld);
}

void Hooks::OldDrawModelExecute(const SourceSDK::DrawModelState& state,
								const SourceSDK::ModelRenderInfo& info,
								SourceSDK::Matrix3x4* boneToWorld)
{
	UTIL_ASSERT(oldDrawModelExecute, "Tried to run old dme but it's not hooked");

	reinterpret_cast<void(__thiscall*)(void*, const SourceSDK::DrawModelState&, const SourceSDK::ModelRenderInfo&, SourceSDK::Matrix3x4*)>
		(oldDrawModelExecute->GetOriginal())
		(SourceSDK::modelrender, state, info, boneToWorld);
}
