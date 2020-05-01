#include "reset.hpp"

#include "../shutdown.hpp"

#include "../ui/ui.hpp"

HRESULT __stdcall Hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
	CREATE_SHUTDOWN_HOOK_GUARD(L"Reset", oldReset);
	MAKE_BUSY_SHUTDOWN_GUARD;

	UI::Reset();

	auto result = reinterpret_cast<decltype(Hooks::Reset)*>
		(oldReset->GetOriginal())
		(device, params);

	return result;
}
