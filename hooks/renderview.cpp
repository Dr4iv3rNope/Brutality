#include "renderview.hpp"

#include "../shutdown.hpp"

void __fastcall Hooks::RenderView(void* ecx, void* edx, const SourceSDK::ViewSetup& setup, int clearFlags, int whatToDraw)
{
	_SHUTDOWN_GUARD;

	reinterpret_cast<decltype(RenderView)*>
		(hooks->oldRenderView->GetOriginal())
		(ecx, edx, setup, clearFlags, whatToDraw);
}
