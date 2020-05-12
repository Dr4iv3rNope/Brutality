#include "overrideview.hpp"

#include "../sourcesdk/viewsetup.hpp"

#include "../shutdown.hpp"

void __stdcall Hooks::OverrideView(SourceSDK::ViewSetup* view_setup)
{
	SHUTDOWN_HOOK_GUARD(L"OverrideView");
	MAKE_BUSY_SHUTDOWN_GUARD;

	reinterpret_cast<decltype(Hooks::OverrideView)*>
		(hooks->oldOverrideView->GetOriginal())
		(view_setup);

	//Features::ThirdPerson::Think(view_setup);
}
