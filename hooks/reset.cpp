#include "reset.hpp"

#include "../shutdown.hpp"

#include "../ui/ui.hpp"

HRESULT __stdcall Hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
	SHUTDOWN_HOOK_GUARD(L"Reset");
	MAKE_BUSY_SHUTDOWN_GUARD;

	UI::Reset();

	return reinterpret_cast<decltype(Hooks::Reset)*>
		(hooks->oldReset->GetOriginal())
		(device, params);
}
